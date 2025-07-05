#include "I2C.h"
#include "systick.h"

static void i2c_delay(void)
{
    __NOP(); __NOP(); __NOP(); __NOP(); // 6?NOP???125ns @48MHz
    __NOP(); __NOP();
}

void I2C_Init(void)
{
    // ????
//    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    
    // ???SCL (PA3)
    gpio_init(I2C_SCL_PORT, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, I2C_SCL_PIN);
    
    // ???SDA (PB12)
    gpio_init(I2C_SDA_PORT, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, I2C_SDA_PIN);
    
    // ????
    SCL_H();
    SDA_H();
    delay_us(50);  // ??????
}

void I2C_Start(void)
{
    SDA_OUT();
    SDA_H();
    SCL_H();
    i2c_delay();
    
    SDA_L();      // START??:SCL??SDA???
    i2c_delay();
    SCL_L();      // ????????
}

void I2C_Stop(void)
{
    SDA_OUT();
    SCL_L();
    SDA_L();
    i2c_delay();
    
    SCL_H();      // STOP??:SCL??SDA???
    i2c_delay();
    SDA_H();
    i2c_delay();
}

uint8_t I2C_SendByte(uint8_t byte)
{
    uint8_t ack;
    SDA_OUT();
    
    // ??8???
    for(uint8_t i = 0; i < 8; i++) {
        (byte & 0x80) ? SDA_H() : SDA_L();
        byte <<= 1;
        
        i2c_delay();
        SCL_H();
        i2c_delay();
        SCL_L();
    }
    
    // ??ACK
    ack = I2C_Wait_Ack();
    return ack;
}

uint8_t I2C_Wait_Ack(void)
{
    uint8_t ack;
    SDA_IN();        // ???????
    
    SCL_H();
    i2c_delay();
    
    // ??ACK??
    ack = SDA_READ() ? 1 : 0;
    
    SCL_L();
    SDA_OUT();        // ??????
    
    return ack;
}

void I2C_Ack(uint8_t ack)
{
    SDA_OUT();
    SCL_L();
    (ack) ? SDA_H() : SDA_L();  // 0:ACK, 1:NACK
    
    i2c_delay();
    SCL_H();
    i2c_delay();
    SCL_L();
    SDA_H();  // ??SDA?
}

uint8_t I2C_ReadByte(uint8_t ack)
{
    uint8_t byte = 0;
    SDA_IN();  // ??SDA???
    
    for(uint8_t i = 0; i < 8; i++) {
        SCL_L();
        i2c_delay();
        SCL_H();
        
        byte <<= 1;
        if(SDA_READ()) byte |= 0x01;
        
        i2c_delay();
    }
    
    SDA_OUT();
    I2C_Ack(ack);  // ??ACK/NACK
    
    return byte;
}

void I2C_BusScan(void)
{
    printf("I2C Bus Scan (SCL:PA3, SDA:PB12):\r\n");
    
    uint8_t found = 0;
    for(uint8_t addr = 0x08; addr < 0x78; addr++) {
        I2C_Start();
        
        // ???????????
        uint8_t ack = I2C_SendByte(addr << 1);
        I2C_Stop();
        delay_us(100);
        
        if(!ack) {
            printf("-> Found device at 0x%02X\r\n", addr);
            found++;
        }
    }
    
    if(!found) {
        printf("!! No devices found. Check:\r\n");
        printf("1. 3.3V power supply\r\n");
        printf("2. 4.7K pull-up resistors on SDA/SCL\r\n");
        printf("3. SCL(PA3)/SDA(PB12) connections\r\n");
    }
    else {
        printf("Scan complete. Found %d devices\r\n", found);
    }
}

uint8_t I2C_Write(uint8_t addr, uint8_t reg, uint8_t data)
{
    I2C_Start();
    
    // ??????(?)
    if(I2C_SendByte(addr << 1)) {
        I2C_Stop();
        return 1;
    }
    
    // ???????
    if(I2C_SendByte(reg)) {
        I2C_Stop();
        return 1;
    }
    
    // ????
    if(I2C_SendByte(data)) {
        I2C_Stop();
        return 1;
    }
    
    I2C_Stop();
    return 0;
}

uint8_t I2C_Read(uint8_t addr, uint8_t reg, uint8_t *data)
{
    I2C_Start();
    
    // ??????(?)
    if(I2C_SendByte(addr << 1)) {
        I2C_Stop();
        return 1;
    }
    
    // ???????
    if(I2C_SendByte(reg)) {
        I2C_Stop();
        return 1;
    }
    
    // ??????
    I2C_Start();
    
    // ??????(?)
    if(I2C_SendByte((addr << 1) | 0x01)) {
        I2C_Stop();
        return 1;
    }
    
    // ????(??NACK??????)
    *data = I2C_ReadByte(1);
    
    I2C_Stop();
    return 0;
}