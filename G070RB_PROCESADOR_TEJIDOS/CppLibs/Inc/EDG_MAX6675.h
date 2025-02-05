/*
 * EDG_MAX6675.h
 *
 *  Created on: Mar 19, 2023
 *      Author: Elkin Granados
 */

#ifndef INC_EDG_MAX6675_H_
#define INC_EDG_MAX6675_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "EDG_CONFIG.h"

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

#include <string.h>
#include "main.h"
#include "spi.h"

#endif

#include "EDG_WS2812.h"

#define EDG_MAX6675_MAX_NUM_CHIPS 			(6)
#define EDG_MAX6675_ALL_CHIPS_ENABLED_MASK 	(0x3F)
#define EDG_MAX6675_OPEN_TC_MASK 			(0x0004)
#define EDG_MAX6675_SPI_HANDLE 				hspi1
#define EDG_MAX6675_SPI_TIMEOUT_MS 			(50)

#define EDG_MAX6675_CS0_PORT	CS1_GPIO_Port
#define EDG_MAX6675_CS0_PIN		CS1_Pin
#define EDG_MAX6675_CS1_PORT	CS2_GPIO_Port
#define EDG_MAX6675_CS1_PIN		CS2_Pin
#define EDG_MAX6675_CS2_PORT	CS3_GPIO_Port
#define EDG_MAX6675_CS2_PIN		CS3_Pin
#define EDG_MAX6675_CS3_PORT	CS4_GPIO_Port
#define EDG_MAX6675_CS3_PIN		CS4_Pin
#define EDG_MAX6675_CS4_PORT	CS5_GPIO_Port
#define EDG_MAX6675_CS4_PIN		CS5_Pin
#define EDG_MAX6675_CS5_PORT	CS6_GPIO_Port
#define EDG_MAX6675_CS5_PIN		CS6_Pin


typedef enum
{
	EDG_MAX6675_CHIP_STATUS_OK,
	EDG_MAX6675_CHIP_STATUS_TC_OPEN,
	EDG_MAX6675_CHIP_STATUS_COMM_ERROR,
	EDG_MAX6675_CHIP_STATUS_CHIP_ERROR,
	EDG_MAX6675_CHIP_STATUS_NOT_ENABLE,
	EDG_MAX6675_CHIP_STATUS_DEFAULT,

}EDG_MAX6675_ChipStatusTypeDef;


typedef struct __EDG_MAX6675_ChipStructTypeDef
{

	EDG_MAX6675_ChipStatusTypeDef ChipStatus;
	float Temperature;

}EDG_MAX6675_ChipStructTypeDef;


/**
  * @brief MAX6675 Handle Structure definition
  */
typedef struct __EDG_MAX6675_HandleTypeDef
{

	uint8_t Counter;
	uint8_t NumChipsEnabled;
	uint8_t ChipsToRead;
	uint16_t max6675_buffer;
	EDG_MAX6675_ChipStructTypeDef Chip[EDG_MAX6675_MAX_NUM_CHIPS];

}EDG_MAX6675_HandleTypeDef;

/**
  * @brief  RTC Handler Definition
  */

extern EDG_MAX6675_HandleTypeDef hedgMAX6675;

/* MAX6675 Functions -------------------------------------*/

void EDG_MAX6675_Init(EDG_MAX6675_HandleTypeDef *ptrhedgMAX6675);
void EDG_MAX6675_ReadAllChips(EDG_MAX6675_HandleTypeDef *ptrhedgMAX6675);
void EDG_MAX6675_ReadChip(EDG_MAX6675_HandleTypeDef *ptrhedgMAX6675, uint8_t NumChip);
void EDG_MAX6675_WriteCsChip( uint8_t NumChip, GPIO_PinState PinState);

#ifdef __cplusplus
}
#endif

#endif /* INC_EDG_WS2812_H_ */
