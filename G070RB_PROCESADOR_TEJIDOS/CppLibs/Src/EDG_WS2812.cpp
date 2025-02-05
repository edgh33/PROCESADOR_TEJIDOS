/*
 * EDG_WS2812.cpp
 *
 *  Created on: Mar 19, 2023
 *      Author: edgh3
 */


#include "EDG_WS2812.h"

EDG_WS2812_HandleTypeDef hedgWs2812;

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_WS2812_Init(EDG_WS2812_HandleTypeDef * ptrhedgWs2812)
{

    memset(ptrhedgWs2812->ws2812_buffer, 0, EDG_WS2812_BUFFER_SIZE);
    EDG_WS2812_SendSpi(ptrhedgWs2812);
    return;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_WS2812_SendSpi(EDG_WS2812_HandleTypeDef * ptrhedgWs2812)
{

    HAL_SPI_Transmit(&EDG_WS2812_SPI_HANDLE, ptrhedgWs2812->ws2812_buffer, EDG_WS2812_BUFFER_SIZE, HAL_MAX_DELAY);
    return;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
#define EDG_WS2812_FILL_BUFFER(COLOR) \
    for( uint8_t mask = 0x80; mask; mask >>= 1 ) { \
        if( COLOR & mask ) { \
            *ptr++ = 0xfc; \
        } else { \
            *ptr++ = 0x80; \
        } \
    }

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_WS2812_Pixel(EDG_WS2812_HandleTypeDef * ptrhedgWs2812, uint16_t led_no, uint8_t r, uint8_t g, uint8_t b)
{

    uint8_t * ptr = &ptrhedgWs2812->ws2812_buffer[24 * led_no];
    EDG_WS2812_FILL_BUFFER(g);
    EDG_WS2812_FILL_BUFFER(r);
    EDG_WS2812_FILL_BUFFER(b);
    return;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_WS2812_PixelAll(EDG_WS2812_HandleTypeDef * ptrhedgWs2812, uint8_t r, uint8_t g, uint8_t b)
{

    uint8_t * ptr = ptrhedgWs2812->ws2812_buffer;
    for( uint16_t i = 0; i < EDG_WS2812_NUM_LEDS; ++i) {
    	EDG_WS2812_FILL_BUFFER(g);
    	EDG_WS2812_FILL_BUFFER(r);
    	EDG_WS2812_FILL_BUFFER(b);
    }
    return;

}
