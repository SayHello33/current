#ifndef _LCD_ST7735S_H_
#define _LCD_ST7735S_H_

#include "gd32f10x.h"

// 只声明 extern，不定义
extern const unsigned char digit_font[10][16];
// 大写字母字模
extern const unsigned char uppercase_font[26][16];


#define LCD_RCU RCU_GPIOB

#define LCD_PORT GPIOB
#define LCD_PORTA GPIOA

#define LCD_PIN_SCK GPIO_PIN_5
#define LCD_PIN_SDA GPIO_PIN_7
#define LCD_PIN_RES GPIO_PIN_1
#define LCD_PIN_A0 GPIO_PIN_0
#define LCD_PIN_CS GPIO_PIN_4

#define LCD_PIN_LED GPIO_PIN_2

#define SET_LCD_CS 	gpio_bit_set(LCD_PORTA, LCD_PIN_CS)
#define SET_LCD_RES gpio_bit_set(LCD_PORT, LCD_PIN_RES)
#define SET_LCD_A0 	gpio_bit_set(LCD_PORT, LCD_PIN_A0)
#define SET_LCD_SDA gpio_bit_set(LCD_PORTA, LCD_PIN_SDA)
#define SET_LCD_SCK gpio_bit_set(LCD_PORTA, LCD_PIN_SCK)
#define SET_LCD_LED 	gpio_bit_set(LCD_PORT, LCD_PIN_LED)


#define	CLR_LCD_CS  gpio_bit_reset(LCD_PORTA, LCD_PIN_CS)
#define	CLR_LCD_RES  gpio_bit_reset(LCD_PORT, LCD_PIN_RES)
#define	CLR_LCD_A0  gpio_bit_reset(LCD_PORT, LCD_PIN_A0)
#define	CLR_LCD_SDA  gpio_bit_reset(LCD_PORTA, LCD_PIN_SDA)
#define	CLR_LCD_SCK  gpio_bit_reset(LCD_PORTA, LCD_PIN_SCK)
#define CLR_LCD_LED 	gpio_bit_reset(LCD_PORT, LCD_PIN_LED)
#define WHITE	0xFFFF
#define BLACK	0x0000	  
#define BLUE	0x001F  
#define BRED	0XF81F
#define GRED	0XFFE0
#define GBLUE	0X07FF
#define RED		0xF800
#define MAGENTA	0xF81F
#define GREEN	0x07E0
#define CYAN	0x7FFF
#define YELLOW	0xFFE0
#define BROWN	0XBC40 //???
#define BRRED	0XFC07 //????
#define GRAY	0X8430 //???
#define DARKBLUE	0X01CF	//?????
#define LIGHTBLUE	0X7D7C	//????  
#define GRAYBLUE	0X5458 //?????
#define LCD_WIDTH 160
#define LCD_HEIGHT 80
void lcd_gpio_init(void);
void lcd_st7735s_init(void);
void lcd_write_register(unsigned char data);
void lcd_write_data(unsigned char data);
void lcd_write_data_u16(unsigned short data);

void lcd_set_address(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2);
void lcd_clear(unsigned short color);

void lcd_draw_point(unsigned short x, unsigned short y, unsigned short color);

void lcd_display_on(void); //?????
void lcd_display_off(void); //?????
void lcd_draw_circle(unsigned short x0, unsigned short y0, unsigned char r, unsigned short color); //???
void lcd_draw_full_circle(unsigned short Xpos, unsigned short Ypos, unsigned short Radius, unsigned short Color);
void lcd_fill(unsigned short sx, unsigned short sy, unsigned short ex, unsigned short ey, unsigned short color); //???????
void lcd_draw_line(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned short color); //????
void lcd_draw_rectangle(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned short color); //??????
//void lcd_show_char(unsigned short x, unsigned short y, unsigned char num, unsigned char size, unsigned char mode, unsigned short pen_color, unsigned short back_color); //?ASCII???
//void lcd_show_string(unsigned short x, unsigned short y, unsigned short width, unsigned short height, unsigned char size, unsigned char *p, unsigned short pen_color, unsigned short back_color); //?ASCII?????

void GUI_sprintf_hz1616(unsigned short x, unsigned short y, unsigned char c[2], unsigned short pen_color, unsigned short back_color);
void GUI_sprintf_hz16x(unsigned short x1, unsigned short y1, unsigned char *str, unsigned short dcolor, unsigned short bgcolor);

void GUI_sprintf_hz3232(unsigned short x, unsigned short y, unsigned char c[2], unsigned short pen_color, unsigned short back_color);
void GUI_sprintf_hz32x(unsigned short x1, unsigned short y1, unsigned char *str, unsigned short pen_color, unsigned short back_color);
void lcd_draw_int(unsigned short x, unsigned short y, int num, unsigned short fg_color, unsigned short bg_color) ;
void lcd_draw_string(unsigned short x, unsigned short y, const char *str, unsigned short fg_color, unsigned short bg_color) ;
void lcd_draw_float(unsigned short x, unsigned short y, float num, unsigned char decimals, unsigned short fg_color, unsigned short bg_color);



void lcd_display_value(unsigned short x, unsigned short y, float value, const char *unit, unsigned short fg_color, unsigned short bg_color);






#endif
