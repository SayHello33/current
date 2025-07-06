/*!
    \file    main.c
    \brief   led spark with systick, USART print and key example

    \version 2024-12-20, V2.5.0, firmware for GD32F10x
*/

/*
    Copyright (c) 2024, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32f10x.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "gd32f10x_eval.h"
#include "ST7735.h"
#include "INA226.h"
#include "i2c.h"
/*!
    \brief      toggle the led every 500ms
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_spark(void)
{
    static __IO uint32_t timingdelaylocal = 0U;

    if(timingdelaylocal){

        if(timingdelaylocal < 500U){
            gd_eval_led_on(LED2);
        }else{
            gd_eval_led_off(LED2);
        }

        timingdelaylocal--;
    }else{
        timingdelaylocal = 1000U;
    }
}
void lcd_init()
{
	lcd_st7735s_init();
	lcd_clear(BLACK);
	// lcd_fill(0,0,79,160,WHITE);
}

int main(void)
{
#ifdef __FIRMWARE_VERSION_DEFINE
    uint32_t fw_ver = 0;
#endif
    /* configure systick */
    systick_config();
    /* initialize the LEDs, USART and key */
    gd_eval_led_init(LED2); 
    gd_eval_com_init(EVAL_COM0);
		rcu_periph_clock_enable(RCU_GPIOB);
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_GPIO);
	
		lcd_init();

//  
//    // 绘制垂直线（左边缘）
//    lcd_draw_line(20, 0, 20, LCD_HEIGHT-1, WHITE);
    
    // 显示横向标题
    lcd_draw_string(0, 5, "VOLTAGE", WHITE, BLACK);
    lcd_draw_string(0, 25, "CURRENT", WHITE, BLACK);
    lcd_draw_string(0, 45, "POWER", WHITE, BLACK);
    


    
//	while(1);
    
#ifdef __FIRMWARE_VERSION_DEFINE
    fw_ver = gd32f10x_firmware_version_get();
    /* print firmware version */
    printf("\r\nGD32F10x series firmware version: V%d.%d.%d", (uint8_t)(fw_ver >> 24), (uint8_t)(fw_ver >> 16), (uint8_t)(fw_ver >> 8));
#endif /* __FIRMWARE_VERSION_DEFINE */
    
    /* print out the clock frequency of system, AHB, APB1 and APB2 */
    printf("\r\nCK_SYS is %d", rcu_clock_freq_get(CK_SYS));
    printf("\r\nCK_AHB is %d", rcu_clock_freq_get(CK_AHB));
    printf("\r\nCK_APB1 is %d", rcu_clock_freq_get(CK_APB1));
    printf("\r\nCK_APB2 is %d", rcu_clock_freq_get(CK_APB2));

		I2C_Init();

		I2C_BusScan();
		INA226_Init();
		
//     // 清除旧数值区域
//        lcd_fill(100, 5, 159, 20, BLACK); // 电压
//        lcd_fill(100, 25, 159, 40, BLACK); // 电流
//        lcd_fill(100, 45, 159, 60, BLACK); // 功率

    // 用于存储格式化字符串的缓冲区
    char voltage_str[20];
    char current_str[20];
    char power_str[20];




    while(1){
        if(RESET == gd_eval_key_state_get(KEY_WAKEUP)){
            gd_eval_led_on(LED3);
            delay_1ms(500);
            gd_eval_led_off(LED3);
            gd_eval_led_toggle(LED4);
        }
		    // 读取传感器数据
        float voltage = INA226_ReadVoltage();
        float current_mA = INA226_ReadCurrent_mA();
        float power = INA226_ReadPower();
//				  // 格式化数据为字符串
//        sprintf(voltage_str, "%.2f V", voltage);
//        sprintf(current_str, "%.2f MA", current_mA);
//        sprintf(power_str, "%.2f W", power);
//				
				// 在LCD上显示数据（分三行）
        // 清除旧数值区域 (x=100, y=30, 宽度=60, 高度=15)
//      +
//        lcd_draw_string(80, 30, voltage_str, GREEN, BLACK);
				 lcd_display_value(70, 5, voltage, "   V", GREEN, BLACK);
//				  // 清除旧数值区域 (x=100, y=50, 宽度=60, 高度=15)
//       
        lcd_display_value(70, 25, current_mA, "  mA", YELLOW, BLACK);
//        
//        // 清除旧数值区域 (x=100, y=70, 宽度=60, 高度=15)
//        
        lcd_display_value(70, 45, power, "  W", CYAN, BLACK);
				
        
        printf("Bus Voltage: %.3f V\r\n", voltage);
        printf("Current:     %.1f mA\r\n", current_mA);
        printf("Power:       %.3f W\r\n", power);
        
       
//        static int count = 0;
//        if(++count % 10 == 0) {
//            INA226_Debug();
//        }
//        
      
   
        
					
    }
}

#ifdef GD_ECLIPSE_GCC
/* retarget the C library printf function to the USART, in Eclipse GCC environment */
int __io_putchar(int ch)
{
    usart_data_transmit(EVAL_COM0, (uint8_t) ch );
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
#else
/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
#endif /* GD_ECLIPSE_GCC */
