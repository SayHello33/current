#include "INA226.h"
#include "I2C.h"
#include "systick.h"
#include <math.h>

// 全局变量
static float zero_offset_mA = 0.0f;
static uint8_t first_measure = 1;
uint16_t last_config = 0;
uint16_t last_calib = 0;
// 精确计算LSB和校准值

static const float CURRENT_LSB = MAX_CURRENT / 32768.0f;  // ≈30.52μA/bit
static const uint16_t CALIB_VALUE = (uint16_t)(0.00512f / (CURRENT_LSB * SHUNT_RESISTOR));
// I²C读取函数 (需根据实际I2C驱动调整)
uint16_t INA226_Read2Byte(uint8_t reg_addr)
{
    uint8_t data[2];
    uint8_t status;
    
    // 启动传输
    I2C_Start();
    status = I2C_SendByte(WRITE_ADDR);
    if(status) goto error;
    
    status = I2C_SendByte(reg_addr);
    if(status) goto error;
    
    // 重新启动读传输
    I2C_Start();
    status = I2C_SendByte(READ_ADDR);
    if(status) goto error;
    
    data[0] = I2C_ReadByte(0); // MSB带ACK
    data[1] = I2C_ReadByte(1); // LSB带NACK
    
    I2C_Stop();
    return ((uint16_t)data[0] << 8) | data[1];
    
error:
    I2C_Stop();
    return 0xFFFF;
}

// I²C写入函数
uint8_t INA226_Write2Byte(uint8_t reg_addr, uint16_t reg_data)
{
    uint8_t status;
    
    I2C_Start();
    status = I2C_SendByte(WRITE_ADDR);
    if(status) goto error;
    
    status = I2C_SendByte(reg_addr);
    if(status) goto error;
    
    status = I2C_SendByte(reg_data >> 8);
    if(status) goto error;
    
    status = I2C_SendByte(reg_data & 0xFF);
    
error:
    I2C_Stop();
    delay_1ms(1);
    return status;
}

// INA226初始化
void INA226_Init(void)
{
    printf("INA226 Initializing (100mΩ shunt)...\r\n");
    
    // 复位
    if(INA226_Write2Byte(CONFIG_REG, 0x8000)) {
        printf("Reset failed!\r\n");
        return;
    }
    delay_1ms(10);
    
    // 配置：AVG=16, VBUS_CT=1.1ms, VSH_CT=1.1ms, 连续测量模式
    uint16_t config = 0x4527;  // 0x45 27 更合适的配置
    if(INA226_Write2Byte(CONFIG_REG, config)) {
        printf("Configuration failed!\r\n");
        return;
    }
    
    // 写入校准值
    if(INA226_Write2Byte(CALIB_REG, CALIB_VALUE)) {
        printf("Calibration failed!\r\n");
        return;
    }
    
    // 验证配置
    uint16_t cal_val = INA226_Read2Byte(CALIB_REG);
    uint16_t config_val = INA226_Read2Byte(CONFIG_REG);
    
    printf("Config reg: 0x%04X (calib:0x%04X, shunt:%.1fΩ)\r\n", 
           config_val, cal_val, SHUNT_RESISTOR);
    
    // 等待首次测量执行零点校准
    first_measure = 1;
}
// 总线电压读取
float INA226_ReadVoltage(void)
{
    uint16_t raw = INA226_Read2Byte(BUS_V_REG);
    if(raw == 0xFFFF) {
        printf("Bus voltage read error\n");
        return 0.0f;
    }
    return raw * 0.00125f; // LSB = 1.25mV
}
// 毫安级电流测量 (专为100mΩ电阻优化)
float INA226_ReadCurrent_mA(void)
{
    // 读取分流寄存器原始值
    int16_t shunt_raw = (int16_t)INA226_Read2Byte(SHUNT_V_REG);
    
    // 转换成分流电压 (μV), LSB = 2.5μV
    float shunt_uV = shunt_raw * 2.5f;
    
    // 计算电流 (mA): I(mA) = shunt_uV / (0.1 * 1000) 
    // 简化: shunt_uV * 0.01
    float current_mA = shunt_uV * 0.01f;
    
    // 首次上电执行零点校准
    if(first_measure) {
        // 采样16次计算零点偏移
        float sum = 0.0f;
        for(int i = 0; i < 16; i++) {
            int16_t raw_val = (int16_t)INA226_Read2Byte(SHUNT_V_REG);
            sum += (raw_val * 2.5f * 0.01f); // 立即转换为mA
            delay_1ms(10);
        }
        // 计算零点偏移 (mA)
        zero_offset_mA = sum / 16.0f;
        first_measure = 0;
        printf("Zero offset calibrated: %.2f mA\n", zero_offset_mA);
    }
    
    // 应用零点补偿
    current_mA -= zero_offset_mA;
    
    // 数字滤波 (4点移动平均)
    #define FILTER_SIZE 4
    static float filter_buf[FILTER_SIZE] = {0};
    static uint8_t filter_idx = 0;
    
    filter_buf[filter_idx] = current_mA;
    filter_idx = (filter_idx + 1) % FILTER_SIZE;
    
    float sum = 0;
    for(int i = 0; i < FILTER_SIZE; i++) {
        sum += filter_buf[i];
    }
    
    return sum / FILTER_SIZE;
}

// 修复的功率读取函数
float INA226_ReadPower(void)
{
    // 方法1：使用内部功率寄存器
    uint16_t raw = INA226_Read2Byte(POWER_REG);
    if(raw == 0xFFFF) {
        printf("Power read error\n");
        return 0.0f;
    }
    
    // 功率LSB = 25 × 电流LSB（校准后）
    float power_lsb = 25.0f * CURRENT_LSB; // CURRENT_LSB已定义为全局变量
    
    // 方法2：使用电压和电流计算
    float voltage = INA226_ReadVoltage();
    float current_A = INA226_ReadCurrent_mA() / 1000.0f; // 转换为安培
    float calculated_power = voltage * current_A;
    
    // 如果功率寄存器值异常（为0），使用计算结果
    if(raw == 0) {
        return calculated_power;
    }
    
    return raw * power_lsb;
}
// 增加调试函数
void INA226_Debug(void)
{
    printf("==== INA226 Diagnostic ====\n");
    printf("Config: 0x%04X\n", INA226_Read2Byte(CONFIG_REG));
    printf("Calib:  0x%04X\n", INA226_Read2Byte(CALIB_REG));
    printf("Shunt:  0x%04X\n", INA226_Read2Byte(SHUNT_V_REG));
    printf("BusV:   0x%04X\n", INA226_Read2Byte(BUS_V_REG));
    printf("Current:0x%04X\n", INA226_Read2Byte(CURRENT_REG));
    printf("Power:  0x%04X\n", INA226_Read2Byte(POWER_REG));
}