#ifndef _I2C_H
#define _I2C_H


#include "gd32f10x.h"                   // Device header


#include <stdio.h>
// I2C.h ??????????
#define I2C_SCL_PORT    GPIOB
#define I2C_SCL_PIN     GPIO_PIN_10
#define I2C_SDA_PORT    GPIOB
#define I2C_SDA_PIN     GPIO_PIN_11

// GPIO???
#define SCL_H()         gpio_bit_set(I2C_SCL_PORT, I2C_SCL_PIN)
#define SCL_L()         gpio_bit_reset(I2C_SCL_PORT, I2C_SCL_PIN)
#define SDA_H()         gpio_bit_set(I2C_SDA_PORT, I2C_SDA_PIN)
#define SDA_L()         gpio_bit_reset(I2C_SDA_PORT, I2C_SDA_PIN)

// GPIO?????(????)
#define SDA_IN()        do { \
                            gpio_init(I2C_SDA_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, I2C_SDA_PIN); \
                        } while(0)
#define SDA_OUT()       do { \
                            gpio_init(I2C_SDA_PORT, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, I2C_SDA_PIN); \
                        } while(0)
#define SDA_READ()      (gpio_input_bit_get(I2C_SDA_PORT, I2C_SDA_PIN) != RESET)


uint8_t I2C_Write(uint8_t addr, uint8_t reg, uint8_t data);
uint8_t I2C_Read(uint8_t addr, uint8_t reg, uint8_t *data);
uint8_t I2C_ReadByte(uint8_t ack);
uint8_t I2C_SendByte(uint8_t byte);
void I2C_Init(void);
/* ?????? */
void I2C_Start(void);
void I2C_BusScan(void);
/* ?????? */
void I2C_Stop(void);
void I2C_AddressCheck(void);

/* ?????? */
void I2C_Ack(uint8_t ack);


/* ?????? */
uint8_t I2C_Wait_Ack(void);
#endif