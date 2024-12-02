#ifndef __OLEDCM3_H__
#define __OLEDCM3_H__

#include <stddef.h>
#include <stdint.h>
#include <_ansi.h>

#include "oledcm3_conf.h"


#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#define OLEDCM3_BUFFER_SIZE   OLEDCM3_WIDTH * OLEDCM3_HEIGHT / 8


typedef enum {
    Black = 0x00, // Black color, no pixel
    White = 0x01  // Pixel is set. Color depends on OLED
} OLEDCM3_COLOR;

typedef enum {
    OLEDCM3_OK = 0x00,
    OLEDCM3_ERR = 0x01  // Generic error.
} OLEDCM3_Error_t;

// Struct to store transformations
typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Initialized;
    uint8_t DisplayOn;
} OLEDCM3_t;

typedef struct {
    uint8_t x;
    uint8_t y;
} OLEDCM3_VERTEX;

/** Font */
typedef struct {
	const uint8_t width;                /**< Font width in pixels */
	const uint8_t height;               /**< Font height in pixels */
	const uint16_t *const data;         /**< Pointer to font data array */
    const uint8_t *const char_width;    /**< Proportional character width in pixels (NULL for monospaced) */
} OLEDCM3_Font_t;

// methods
void oledcm3_Init(void);
void oledcm3_Fill(OLEDCM3_COLOR color);
void oledcm3_UpdateScreen(void);

void oledcm3_Reset(void);
void oledcm3_WriteCommand(uint8_t byte);
void oledcm3_WriteData(uint8_t byte);
OLEDCM3_Error_t oledcm3_FillBuffer(uint8_t* buf, uint32_t len);

void delay_ms(uint32_t delay);
void oledcm3_WriteDataBuff(uint8_t* buffer, size_t buff_size);
void oledcm3_SetDisplayOn(const uint8_t on);
void init();
void oledcm3_SetContrast(const uint8_t value);
void oledcm3_DrawPixel(uint8_t x, uint8_t y, OLEDCM3_COLOR color);
char oledcm3_WriteChar(char ch, OLEDCM3_Font_t Font, OLEDCM3_COLOR color);

#define OLEDCM3_X_OFFSET_LOWER 0
#define OLEDCM3_X_OFFSET_UPPER 0

#endif