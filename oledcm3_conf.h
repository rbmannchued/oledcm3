#ifndef __OLED_CONF_H__
#define __OLED_CONF_H__

/*
    spi1 = 
    cs - a4
    dc - a3
    res - a6
    mosi - a7
    clk - a5
*/

#define OLED_SPI_PORT        SPI1
#define OLED_CS_Port         GPIOA
#define OLED_CS_Pin          GPIO4
#define OLED_DC_Port         GPIOA
#define OLED_DC_Pin          GPIO3
#define OLED_Reset_Port      GPIOA
#define OLED_Reset_Pin       GPIO6

#define OLED_Mosi_Port       GPIOA   
#define OLED_Mosi_Pin        GPIO7
#define OLED_Clk_Port        GPIOA
#define OLED_Clk_Pin         GPIO5


#define SSD1306_INCLUDE_FONT_7x10

#define OLEDCM3_HEIGHT          64
#define OLEDCM3_WIDTH           128

#endif
