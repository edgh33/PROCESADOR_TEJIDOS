/*
 * EDG_WS2812.h
 *
 *  Created on: Mar 19, 2023
 *      Author: edgh3
 */

#ifndef INC_EDG_WS2812_H_
#define INC_EDG_WS2812_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "EDG_CONFIG.h"

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

#include <string.h>
#include "main.h"
#include "spi.h"

#endif

#define EDG_WS2812_NUM_LEDS 8
#define EDG_WS2812_SPI_HANDLE hspi2

#define EDG_WS2812_RESET_PULSE 60
#define EDG_WS2812_BUFFER_SIZE ((EDG_WS2812_NUM_LEDS * 24) + EDG_WS2812_RESET_PULSE)




/**
  * @brief WS2812 Handle Structure definition
  */
typedef struct __EDG_WS2812_HandleTypeDef
{

	uint8_t ws2812_buffer[EDG_WS2812_BUFFER_SIZE];

}EDG_WS2812_HandleTypeDef;

extern EDG_WS2812_HandleTypeDef hedgWs2812;

void EDG_WS2812_Init(EDG_WS2812_HandleTypeDef * ptrhedgWs2812);
void EDG_WS2812_SendSpi(EDG_WS2812_HandleTypeDef * ptrhedgWs2812);
void EDG_WS2812_Pixel(EDG_WS2812_HandleTypeDef * ptrhedgWs2812, uint16_t led_no, uint8_t r, uint8_t g, uint8_t b);
void EDG_WS2812_PixelAll(EDG_WS2812_HandleTypeDef * ptrhedgWs2812, uint8_t r, uint8_t g, uint8_t b);

#ifdef __cplusplus
}
#endif

#endif /* INC_EDG_WS2812_H_ */
