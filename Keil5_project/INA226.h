#ifndef __INA226_H
#define __INA226_H

#include "gd32f10x.h"

#define INA226_ADDR     0x40   // ????(ADDR??)
#define WRITE_ADDR      (INA226_ADDR << 1)
#define READ_ADDR       ((INA226_ADDR << 1) | 1)

// ?????
#define CONFIG_REG      0x00
#define SHUNT_V_REG     0x01
#define BUS_V_REG       0x02
#define POWER_REG       0x03
#define CURRENT_REG     0x04
#define CALIB_REG       0x05
#define MASK_REG        0x06
#define ALERT_REG       0x07
#define MFG_ID_REG      0xFE    // ???ID???
#define DIE_ID_REG      0xFF    // ??ID???
#define OPTIMAL_CONFIG 0x4C27  

// 硬件参数 (100mΩ分流电阻)
#define SHUNT_RESISTOR  0.1f     // 分流电阻100mΩ (0.1Ω)
#define MAX_CURRENT     1.0f     // 最大电流1.0A (根据INA226量程限制)
// ????
uint16_t INA226_Read2Byte(uint8_t reg_addr);
uint8_t INA226_Write2Byte(uint8_t reg_addr, uint16_t reg_data);
void INA226_Init(void);
float INA226_ReadCurrent(void);
float INA226_ReadVoltage(void);
float INA226_ReadCurrent_mA(void);

//float INA226_ReadCurrent(void);
float INA226_ReadPower(void);
void INA226_Debug(void);

#endif