#include "oledcm3.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h> 


void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 8000; i++) {
        __asm__("nop");
    }
}
void oledcm3_WriteCommand(uint8_t byte) {
	gpio_clear(OLED_DC_Port, OLED_DC_Pin);
	spi_enable(SPI1);
	spi_xfer(SPI1,byte);
	spi_disable(SPI1);
}
void oledcm3_WriteData(uint8_t byte) {
	gpio_set(OLED_DC_Port,OLED_DC_Pin);
	spi_enable(SPI1);
	spi_xfer(SPI1,byte);
	spi_disable(SPI1);
}
void oledcm3_Reset(void) {
	gpio_clear(OLED_Reset_Port,OLED_Reset_Pin);
	delay_ms(5);
	gpio_set(OLED_Reset_Port,OLED_Reset_Pin);
}
void oledcm3_WriteDataBuff(uint8_t* buffer, size_t buff_size) {

        for (size_t i = 0; i < buff_size; i++) {
        oledcm3_WriteData(buffer[i]);

        // waits empty buffer
        // while (!(SPI_SR(OLED_SPI_PORT) & SPI_SR_TXE));

        // // waits inactive SPI
        // while (SPI_SR(OLED_SPI_PORT) & SPI_SR_BSY);
    }


}
static uint8_t OLEDCM3_Buffer[OLEDCM3_BUFFER_SIZE];


// Screen object
static OLEDCM3_t OLEDCM3;
OLEDCM3_Error_t oledcm3_FillBuffer(uint8_t* buf, uint32_t len) {
    OLEDCM3_Error_t ret = OLEDCM3_ERR;
    if (len <= OLEDCM3_BUFFER_SIZE) {
        memcpy(OLEDCM3_Buffer,buf,len);
        ret = OLEDCM3_OK;
    }
    return ret;
}
void init(){
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);

	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_SPI1);

    gpio_set_mode(OLED_Reset_Pin,
		GPIO_MODE_OUTPUT_2_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL,
		OLED_Reset_Pin|OLED_DC_Pin);
	// Activate OLED Reset line
	gpio_clear(OLED_Reset_Port,OLED_Reset_Pin); 

    	// PB5=MOSI, PB3=SCK
	gpio_set_mode(
		OLED_Mosi_Port,GPIO_MODE_OUTPUT_50_MHZ,
        GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
        OLED_Mosi_Pin|OLED_Clk_Pin
	);
    	// NSS=PA15
	gpio_set_mode(
		OLED_CS_Port,GPIO_MODE_OUTPUT_50_MHZ,
        GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
        OLED_CS_Pin
	);
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);

    spi_init_master(
		SPI1,
                SPI_CR1_BAUDRATE_FPCLK_DIV_256,
                SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
		SPI_CR1_CPHA_CLK_TRANSITION_1,
	        SPI_CR1_DFF_8BIT,
	        SPI_CR1_MSBFIRST
	);
	spi_disable_software_slave_management(SPI1);
	spi_enable_ss_output(SPI1);

}
void oledcm3_Init(void) {
    //init();
    // Reset OLED
    oledcm3_Reset();

    // Wait for the screen to boot
    //HAL_Delay(100);
   // delay_ms(100);

    // Init OLED
    oledcm3_SetDisplayOn(0); //display off

    oledcm3_WriteCommand(0x20); //Set Memory Addressing Mode
    oledcm3_WriteCommand(0x00); // 00b,Horizontal Addressing Mode; 01b,Vertical Addressing Mode;
                                // 10b,Page Addressing Mode (RESET); 11b,Invalid

    oledcm3_WriteCommand(0xB0); //Set Page Start Address for Page Addressing Mode,0-7

    oledcm3_WriteCommand(0xC8); //Set COM Output Scan Direction


    oledcm3_WriteCommand(0x00); //---set low column address
    oledcm3_WriteCommand(0x10); //---set high column address

    oledcm3_WriteCommand(0x40); //--set start line address - CHECK

    oledcm3_SetContrast(0xFF);


    oledcm3_WriteCommand(0xA1); //--set segment re-map 0 to 127 - CHECK
    oledcm3_WriteCommand(0xA6); //--set normal color


// Set multiplex ratio.
    oledcm3_WriteCommand(0xA8); //--set multiplex ratio(1 to 64) - CHECK



#if (OLEDCM3_HEIGHT == 64)
    oledcm3_WriteCommand(0x3F); //
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

    oledcm3_WriteCommand(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

    oledcm3_WriteCommand(0xD3); //-set display offset - CHECK
    oledcm3_WriteCommand(0x00); //-not offset

    oledcm3_WriteCommand(0xD5); //--set display clock divide ratio/oscillator frequency
    oledcm3_WriteCommand(0xF0); //--set divide ratio

    oledcm3_WriteCommand(0xD9); //--set pre-charge period
    oledcm3_WriteCommand(0x22); //

    oledcm3_WriteCommand(0xDA); //--set com pins hardware configuration - CHECK

#if (OLEDCM3_HEIGHT == 64)
    oledcm3_WriteCommand(0x12);
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

    oledcm3_WriteCommand(0xDB); //--set vcomh
    oledcm3_WriteCommand(0x20); //0x20,0.77xVcc

    oledcm3_WriteCommand(0x8D); //--set DC-DC enable
    oledcm3_WriteCommand(0x14); //
    oledcm3_SetDisplayOn(1); //--turn on SSD1306 panel

    // Clear screen
    oledcm3_Fill(Black);
    
    // Flush buffer to screen
    oledcm3_UpdateScreen();
    
    // Set default values for screen object
    OLEDCM3.CurrentX = 0;
    OLEDCM3.CurrentY = 0;
    
    OLEDCM3.Initialized = 1;
}
void oledcm3_Fill(OLEDCM3_COLOR color) {
    memset(OLEDCM3_Buffer, (color == Black) ? 0x00 : 0xFF, sizeof(OLEDCM3_Buffer));
}

void oledcm3_UpdateScreen(void) {
    // Write data to each page of RAM. Number of pages
    // depends on the screen height:
    //
    //  * 32px   ==  4 pages
    //  * 64px   ==  8 pages
    //  * 128px  ==  16 pages
    for(uint8_t i = 0; i < OLEDCM3_HEIGHT/8; i++) {
        oledcm3_WriteCommand(0xB0 + i); // Set the current RAM page address.
        oledcm3_WriteCommand(0x00 + OLEDCM3_X_OFFSET_LOWER);
        oledcm3_WriteCommand(0x10 + OLEDCM3_X_OFFSET_UPPER);
        oledcm3_WriteDataBuff(&OLEDCM3_Buffer[OLEDCM3_WIDTH*i],OLEDCM3_WIDTH);
    }
}
void oledcm3_SetDisplayOn(const uint8_t on) {
    uint8_t value;
    if (on) {
        value = 0xAF;   // Display on
        OLEDCM3.DisplayOn = 1;
    } else {
        value = 0xAE;   // Display off
        OLEDCM3.DisplayOn = 0;
    }
    oledcm3_WriteCommand(value);
}
void oledcm3_SetContrast(const uint8_t value) {
    const uint8_t kSetContrastControlRegister = 0x81;
    oledcm3_WriteCommand(kSetContrastControlRegister);
    oledcm3_WriteCommand(value);
}
void oledcm3_DrawPixel(uint8_t x, uint8_t y, OLEDCM3_COLOR color) {
    if(x >= OLEDCM3_WIDTH || y >= OLEDCM3_HEIGHT) {
        // Don't write outside the buffer
        return;
    }
   
    // Draw in the right color
    if(color == White) {
        OLEDCM3_Buffer[x + (y / 8) * OLEDCM3_WIDTH] |= 1 << (y % 8);
    } else { 
        OLEDCM3_Buffer[x + (y / 8) * OLEDCM3_WIDTH] &= ~(1 << (y % 8));
    }
}
char oledcm3_WriteChar(char ch, OLEDCM3_Font_t Font, OLEDCM3_COLOR color) {
    uint32_t i, b, j;
    
    // Check if character is valid
    if (ch < 32 || ch > 126)
        return 0;
    
    // Check remaining space on current line
    if (OLEDCM3_WIDTH < (OLEDCM3.CurrentX + Font.width) ||
        OLEDCM3_HEIGHT < (OLEDCM3.CurrentY + Font.height))
    {
        // Not enough space on current line
        return 0;
    }
    
    // Use the font to write
    for(i = 0; i < Font.height; i++) {
        b = Font.data[(ch - 32) * Font.height + i];
        for(j = 0; j < Font.width; j++) {
            if((b << j) & 0x8000)  {
                oledcm3_DrawPixel(OLEDCM3.CurrentX + j, (OLEDCM3.CurrentY + i), (OLEDCM3_COLOR) color);
            } else {
                oledcm3_DrawPixel(OLEDCM3.CurrentX + j, (OLEDCM3.CurrentY + i), (OLEDCM3_COLOR)!color);
            }
        }
    }
    
    // The current space is now taken
    OLEDCM3.CurrentX += Font.char_width ? Font.char_width[ch - 32] : Font.width;
    
    // Return written char for validation
    return ch;
}






