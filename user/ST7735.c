#include "ST7735.h"
#include "systick.h"
#include "stdio.h"
#include "string.h"

// 定义横屏分辨率 (160x80)
#define LCD_WIDTH 160
#define LCD_HEIGHT 110
// 数字字模 (8x16像素)
const unsigned char digit_font[10][16] = {
    // '0'
    {0x00, 0x1C, 0x22, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x22, 0x1C, 0x00, 0x00},
    // '1'
    {0x00, 0x08, 0x18, 0x28, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1C, 0x00, 0x00},
    // '2'
    {0x00, 0x3C, 0x42, 0x81, 0x01, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0xFF, 0x00, 0x00},
    // '3'
    {0x00, 0x7E, 0x81, 0x01, 0x01, 0x02, 0x0C, 0x02, 0x01, 0x01, 0x01, 0x01, 0x82, 0x7C, 0x00, 0x00},
    // '4'
    {0x00, 0x06, 0x0A, 0x0A, 0x12, 0x22, 0x22, 0x42, 0x42, 0x82, 0xFF, 0x02, 0x02, 0x02, 0x00, 0x00},
    // '5'
    {0x00, 0xFF, 0x80, 0x80, 0x80, 0xBC, 0xC2, 0x01, 0x01, 0x01, 0x01, 0x01, 0x82, 0x7C, 0x00, 0x00},
    // '6'
    {0x00, 0x1F, 0x20, 0x40, 0x40, 0x80, 0x9C, 0xA2, 0xC1, 0x81, 0x81, 0x81, 0x42, 0x3C, 0x00, 0x00},
    // '7'
    {0x00, 0xFF, 0x81, 0x02, 0x02, 0x04, 0x04, 0x08, 0x08, 0x10, 0x10, 0x20, 0x20, 0x40, 0x00, 0x00},
    // '8'
    {0x00, 0x3C, 0x42, 0x81, 0x81, 0x81, 0x42, 0x3C, 0x42, 0x81, 0x81, 0x81, 0x42, 0x3C, 0x00, 0x00},
    // '9'
    {0x00, 0x3C, 0x42, 0x81, 0x81, 0x81, 0x81, 0x43, 0x21, 0x11, 0x09, 0x05, 0x02, 0x00, 0x00, 0x00}
};
const unsigned char uppercase_font[26][16] = {
    // 'A'
    {0x00, 0x18, 0x24, 0x42, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00},
    // 'B'
    {0x00, 0x7C, 0x42, 0x42, 0x42, 0x42, 0x7C, 0x42, 0x42, 0x42, 0x42, 0x7C, 0x00, 0x00, 0x00, 0x00},
    // 'C'
    {0x00, 0x3C, 0x42, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00},
    // 'D'
    {0x00, 0x7C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x7C, 0x00, 0x00, 0x00, 0x00},
    // 'E'
    {0x00, 0x7E, 0x40, 0x40, 0x40, 0x40, 0x7C, 0x40, 0x40, 0x40, 0x40, 0x7E, 0x00, 0x00, 0x00, 0x00},
    // 'F'
    {0x00, 0x7E, 0x40, 0x40, 0x40, 0x40, 0x7C, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00},
    // 'G'
    {0x00, 0x3C, 0x42, 0x80, 0x80, 0x80, 0x8E, 0x82, 0x82, 0x82, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00},
    // 'H'
    {0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00},
    // 'I'
    {0x00, 0x7C, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00, 0x00, 0x00},
    // 'J'
    {0x00, 0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00},
    // 'K'
    {0x00, 0x42, 0x44, 0x48, 0x50, 0x60, 0x60, 0x50, 0x48, 0x44, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00},
    // 'L'
    {0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7E, 0x00, 0x00, 0x00, 0x00},
    // 'M'
    {0x00, 0x82, 0xC6, 0xC6, 0xAA, 0xAA, 0x92, 0x92, 0x82, 0x82, 0x82, 0x82, 0x00, 0x00, 0x00, 0x00},
    // 'N'
    {0x00, 0x42, 0x62, 0x62, 0x52, 0x52, 0x4A, 0x4A, 0x46, 0x46, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00},
    // 'O'
    {0x00, 0x3C, 0x42, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00},
    // 'P'
    {0x00, 0x7C, 0x42, 0x42, 0x42, 0x42, 0x7C, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00},
    // 'Q'
    {0x00, 0x3C, 0x42, 0x81, 0x81, 0x81, 0x81, 0x81, 0x91, 0x89, 0x46, 0x3D, 0x01, 0x00, 0x00, 0x00},
    // 'R'
    {0x00, 0x7C, 0x42, 0x42, 0x42, 0x42, 0x7C, 0x48, 0x44, 0x44, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00},
    // 'S'
    {0x00, 0x3C, 0x42, 0x80, 0x80, 0x60, 0x1C, 0x02, 0x01, 0x01, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00},
    // 'T'
    {0x00, 0xFE, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00},
    // 'U'
    {0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00},
    // 'V'
    {0x00, 0x42, 0x42, 0x42, 0x24, 0x24, 0x24, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00},
    // 'W'
    {0x00, 0x82, 0x82, 0x82, 0x92, 0x92, 0x92, 0xAA, 0xAA, 0x44, 0x44, 0x44, 0x00, 0x00, 0x00, 0x00},
    // 'X'
    {0x00, 0x42, 0x42, 0x24, 0x24, 0x18, 0x18, 0x18, 0x24, 0x24, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00},
    // 'Y'
    {0x00, 0x82, 0x44, 0x44, 0x28, 0x28, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00},
    // 'Z'
    {0x00, 0x7E, 0x02, 0x04, 0x08, 0x08, 0x10, 0x20, 0x20, 0x40, 0x40, 0x7E, 0x00, 0x00, 0x00, 0x00}
};

// 标点符号字模 (8x16像素)
const unsigned char punct_font[7][16] = {
    // 空格 (ASCII 32)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    // '!' (33)
    {0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00},
    // ':' (58)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00},
    // '.' (46)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00},
    // '-' (45)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    // 'V' (86) - 用于"V"
    {0x00, 0x42, 0x42, 0x42, 0x24, 0x24, 0x24, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00},
    // 'W' (87) - 用于"W"
    {0x00, 0x82, 0x82, 0x82, 0x92, 0x92, 0x92, 0xAA, 0xAA, 0x44, 0x44, 0x44, 0x00, 0x00, 0x00, 0x00}
};

void lcd_gpio_init(void)
{
    rcu_periph_clock_enable(LCD_RCU);
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(LCD_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCD_PIN_RES | LCD_PIN_A0 | LCD_PIN_LED);
    gpio_init(LCD_PORTA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCD_PIN_CS | LCD_PIN_SDA | LCD_PIN_SCK);
    SET_LCD_LED;
}

void lcd_st7735s_init(void)
{
    lcd_gpio_init();
    
    // 加强复位时序
    SET_LCD_RES;
    delay_1ms(150);
    CLR_LCD_RES;
    delay_1ms(150);
    SET_LCD_RES;
    delay_1ms(200);
    
    lcd_write_register(0x11); // Sleep out 
    delay_1ms(150);
    
    lcd_write_register(0xB1); 
    lcd_write_data(0x05); 
    lcd_write_data(0x3C);
    lcd_write_data(0x3C); 
    
    lcd_write_register(0xB2); 
    lcd_write_data(0x05); 
    lcd_write_data(0x3C); 
    lcd_write_data(0x3C); 
    
    lcd_write_register(0xB3); 
    lcd_write_data(0x05); 
    lcd_write_data(0x3C); 
    lcd_write_data(0x3C); 
    lcd_write_data(0x05); 
    lcd_write_data(0x3C); 
    lcd_write_data(0x3C); 
    
    lcd_write_register(0xB4);
    lcd_write_data(0x03); 
    
    lcd_write_register(0xC0); 
    lcd_write_data(0x28); 
    lcd_write_data(0x08); 
    lcd_write_data(0x04); 
    
    lcd_write_register(0xC1); 
    lcd_write_data(0XC0); 
    
    lcd_write_register(0xC2); 
    lcd_write_data(0x0D); 
    lcd_write_data(0x00); 
    
    lcd_write_register(0xC3); 
    lcd_write_data(0x8D); 
    lcd_write_data(0x2A); 
    
    lcd_write_register(0xC4); 
    lcd_write_data(0x8D); 
    lcd_write_data(0xEE); 
    
    lcd_write_register(0xC5);
    lcd_write_data(0x1A); 
    
    lcd_write_register(0x36);
    lcd_write_data(0xA0); // 横屏模式
    
    lcd_write_register(0xE0); 
    lcd_write_data(0x04); 
    lcd_write_data(0x22); 
    lcd_write_data(0x07); 
    lcd_write_data(0x0A); 
    lcd_write_data(0x2E); 
    lcd_write_data(0x30); 
    lcd_write_data(0x25); 
    lcd_write_data(0x2A); 
    lcd_write_data(0x28); 
    lcd_write_data(0x26); 
    lcd_write_data(0x2E); 
    lcd_write_data(0x3A); 
    lcd_write_data(0x00); 
    lcd_write_data(0x01); 
    lcd_write_data(0x03); 
    lcd_write_data(0x13); 
    
    lcd_write_register(0xE1); 
    lcd_write_data(0x04); 
    lcd_write_data(0x16); 
    lcd_write_data(0x06); 
    lcd_write_data(0x0D); 
    lcd_write_data(0x2D); 
    lcd_write_data(0x26); 
    lcd_write_data(0x23); 
    lcd_write_data(0x27); 
    lcd_write_data(0x27); 
    lcd_write_data(0x25); 
    lcd_write_data(0x2D); 
    lcd_write_data(0x3B); 
    lcd_write_data(0x00); 
    lcd_write_data(0x01); 
    lcd_write_data(0x04); 
    lcd_write_data(0x13); 
    
    lcd_write_register(0x3A);
    lcd_write_data(0x05);
    
    lcd_write_register(0x29); 
}

void lcd_write_register(unsigned char data)
{
    CLR_LCD_CS;
    CLR_LCD_A0;
 
    for(int i = 0; i < 8; i++)
    {
        if(data & 0x80)
            SET_LCD_SDA;
        else 
            CLR_LCD_SDA;
        
        CLR_LCD_SCK;
        delay_us(2);
        SET_LCD_SCK;
        delay_us(2);
        data = data << 1;
    }
    SET_LCD_CS;
}

void lcd_write_data(unsigned char data)
{
    CLR_LCD_CS;
    SET_LCD_A0;

    for(int i = 0; i < 8; i++)
    {
        if(data & 0x80)
            SET_LCD_SDA;
        else 
            CLR_LCD_SDA;
        
        CLR_LCD_SCK;
        delay_us(2);
        SET_LCD_SCK;
        delay_us(2);
        data = data << 1;
    }
    SET_LCD_CS;
    delay_us(10);
}

void lcd_write_data_u16(unsigned short data)
{
    lcd_write_data(data>>8);
    lcd_write_data(data);
}

void lcd_set_address(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2)
{
    // 边界检查 (160x80)
    if(x1 >= LCD_WIDTH) x1 = LCD_WIDTH - 1;
    if(y1 >= LCD_HEIGHT) y1 = LCD_HEIGHT - 1;
    if(x2 >= LCD_WIDTH) x2 = LCD_WIDTH - 1;
    if(y2 >= LCD_HEIGHT) y2 = LCD_HEIGHT - 1;
    
    lcd_write_register(0x2A); 
    lcd_write_data(x1>>8); 
    lcd_write_data(x1&0xFF);
    lcd_write_data(x2>>8); 
    lcd_write_data(x2&0xFF);

    lcd_write_register(0x2B); 
    lcd_write_data(y1>>8); 
    lcd_write_data(y1&0xFF);
    lcd_write_data(y2>>8); 
    lcd_write_data(y2&0xFF);
    
    lcd_write_register(0x2C);
}

void lcd_clear(unsigned short color)
{
    lcd_set_address(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    
    // 计算像素总数
    uint32_t total_pixels = LCD_WIDTH * LCD_HEIGHT;
    
    for(uint32_t i = 0; i < total_pixels; i++)
    {
        lcd_write_data_u16(color);
    }
}

void lcd_draw_point(unsigned short x, unsigned short y, unsigned short color)
{
    // 边界检查
    if(x >= LCD_WIDTH || y >= LCD_HEIGHT) return;
    
    lcd_set_address(x, y, x, y);
    lcd_write_data_u16(color);
}

void lcd_fast_draw_point(unsigned short x, unsigned short y, unsigned short color)
{
    lcd_write_data_u16(color);
}

void lcd_display_on(void)
{
    lcd_write_register(0x29);
}

void lcd_display_off(void)
{
    lcd_write_register(0x28);
}

void lcd_fill(unsigned short sx, unsigned short sy, unsigned short ex, unsigned short ey, unsigned short color)
{
    // 边界检查
    if(sx >= LCD_WIDTH) sx = LCD_WIDTH - 1;
    if(sy >= LCD_HEIGHT) sy = LCD_HEIGHT - 1;
    if(ex >= LCD_WIDTH) ex = LCD_WIDTH - 1;
    if(ey >= LCD_HEIGHT) ey = LCD_HEIGHT - 1;
    
    // 确保起始坐标小于结束坐标
    if(sx > ex) {
        unsigned short temp = sx;
        sx = ex;
        ex = temp;
    }
    if(sy > ey) {
        unsigned short temp = sy;
        sy = ey;
        ey = temp;
    }
    
    lcd_set_address(sx, sy, ex, ey);
    
    // 计算区域大小
    unsigned short width = ex - sx + 1;
    unsigned short height = ey - sy + 1;
    uint32_t total = width * height;
    
    for(uint32_t i = 0; i < total; i++)
    {
        lcd_write_data_u16(color);
    }
}

void lcd_draw_line(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned short color)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    int e2;
    
    while(1) {
        lcd_draw_point(x1, y1, color);
        if(x1 == x2 && y1 == y2) break;
        
        e2 = 2 * err;
        if(e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if(e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void lcd_draw_rectangle(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned short color)
{
    lcd_draw_line(x1, y1, x2, y1, color);
    lcd_draw_line(x1, y1, x1, y2, color);
    lcd_draw_line(x1, y2, x2, y2, color);
    lcd_draw_line(x2, y1, x2, y2, color);
}

void lcd_draw_circle(unsigned short x0, unsigned short y0, unsigned char r, unsigned short color)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);
    while(a <= b)
    {
        lcd_draw_point(x0+a, y0-b, color);
        lcd_draw_point(x0+b, y0-a, color);
        lcd_draw_point(x0+b, y0+a, color);
        lcd_draw_point(x0+a, y0+b, color);
        lcd_draw_point(x0-a, y0+b, color);
        lcd_draw_point(x0-b, y0+a, color);
        lcd_draw_point(x0-a, y0-b, color);
        lcd_draw_point(x0-b, y0-a, color);
        a++;
        if(di < 0)
        {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

void lcd_draw_full_circle(unsigned short Xpos, unsigned short Ypos, unsigned short Radius, unsigned short Color)
{
    uint16_t x, y;
    for(y = 0; y < LCD_HEIGHT; y++)
    {
        for(x = 0; x < LCD_WIDTH; x++)
        {
            if(((x - Xpos) * (x - Xpos) + (y - Ypos) * (y - Ypos)) <= Radius * Radius)
            {
                lcd_draw_point(x, y, Color);
            }
        }
    }
}

void lcd_draw_char(unsigned short x, unsigned short y, char c, unsigned short fg_color, unsigned short bg_color)
{
    int index = -1;
    const unsigned char *char_data = NULL;
    
    // 处理数字
    if (c >= '0' && c <= '9') {
        index = c - '0';
        char_data = digit_font[index];
    }
    // 处理大写字母
    else if (c >= 'A' && c <= 'Z') {
        index = c - 'A';
        char_data = uppercase_font[index];
    }
    // 处理小写字母 - 使用大写字母的字模
    else if (c >= 'a' && c <= 'z') {
        index = c - 'a'; // 使用大写字母的字模
        char_data = uppercase_font[index];
    }
    // 处理标点符号
    else {
        switch (c) {
            case ' ': index = 0; break;  // 空格
            case '!': index = 1; break;  // 感叹号
            case ':': index = 2; break;  // 冒号
            case '.': index = 3; break;  // 句点
            case '-': index = 4; break;  // 减号
            case 'V': index = 5; break;  // V (电压单位)
            case 'W': index = 6; break;  // W (功率单位)
            default: return;  // 不支持字符
        }
        char_data = punct_font[index];
    }
    
    // 设置显示区域 (字符大小为8x16)
    lcd_set_address(x, y, x + 7, y + 15);
    
    // 遍历字模并绘制
    for (int row = 0; row < 16; row++) {
        unsigned char row_data = char_data[row];
        for (int col = 0; col < 8; col++) {
            if (row_data & (0x80 >> col)) {
                lcd_fast_draw_point(x + col, y + row, fg_color);
            } else {
                lcd_fast_draw_point(x + col, y + row, bg_color);
            }
        }
    }
}

void lcd_draw_string(unsigned short x, unsigned short y, const char *str, unsigned short fg_color, unsigned short bg_color)
{
    const int char_width = 8;
    const int char_height = 16;
    const int space_width = 1;  // 字符间距
    
    unsigned short curr_x = x;
    
    while (*str) {
        // 处理换行符
        if (*str == '\n') {
            y += char_height + 2;
            curr_x = x;
            str++;
            continue;
        }
        
        // 特殊处理单位字符
        if (str[0] == 'V' || str[0] == 'W') {
            // 确保不是"VOLT"或"WATT"中的V/W
            if ((str == str || *(str-1) == ' ' || *(str-1) == ':') && 
                (*(str+1) == ' ' || *(str+1) == '\0')) {
                lcd_draw_char(curr_x, y, *str, fg_color, bg_color);
            } else {
                lcd_draw_char(curr_x, y, *str, fg_color, bg_color);
            }
        } else {
            lcd_draw_char(curr_x, y, *str, fg_color, bg_color);
        }
        
        // 移动到下一个字符位置
        curr_x += char_width + space_width;
        str++;
    }
}
void lcd_display_value(unsigned short x, unsigned short y, float value, const char *unit, unsigned short fg_color, unsigned short bg_color)
{
    char buffer[20];
    
    if (strcmp(unit, "V") == 0) {
        sprintf(buffer, "%.2f V", value);
    } else if (strcmp(unit, "mA") == 0) {
        sprintf(buffer, "%.1f mA", value);
    } else if (strcmp(unit, "W") == 0) {
        sprintf(buffer, "%.2f W", value);
    } else {
        sprintf(buffer, "%.2f %s", value, unit);
    }
    
    lcd_draw_string(x, y, buffer, fg_color, bg_color);
}
void lcd_draw_int(unsigned short x, unsigned short y, int num, unsigned short fg_color, unsigned short bg_color)
{
    char buffer[12];
    sprintf(buffer, "%d", num);
    lcd_draw_string(x, y, buffer, fg_color, bg_color);
}

void lcd_draw_float(unsigned short x, unsigned short y, float num, unsigned char decimals, unsigned short fg_color, unsigned short bg_color)
{
    char format[10];
    sprintf(format, "%%.%df", decimals);
    
    char buffer[20];
    sprintf(buffer, format, num);
    lcd_draw_string(x, y, buffer, fg_color, bg_color);
}

void lcd_draw_time(unsigned short x, unsigned short y, unsigned char hour, unsigned char minute, unsigned short fg_color, unsigned short bg_color)
{
    char buffer[6];
    sprintf(buffer, "%02d:%02d", hour, minute);
    lcd_draw_string(x, y, buffer, fg_color, bg_color);
}