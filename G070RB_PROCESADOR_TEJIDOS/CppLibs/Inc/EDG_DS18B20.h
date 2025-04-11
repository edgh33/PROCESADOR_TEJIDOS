/*
 * EDG_DS18B20.h
 *
 *  Created on: Apr 07, 2025
 *      Author: Elkin Granados
 */

#ifndef INC_EDG_DS18B20_H_
#define INC_EDG_DS18B20_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "EDG_CONFIG.h"

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

#include <string.h>
#include "main.h"
#include "spi.h"
#include "tim.h"

#endif

#define EDG_DS18B20_TIM_BASE				TIM17
#define EDG_DS18B20_TIM_BASE_HANDLER		htim17

#define EDG_DS18B20_MAX_NUM_CHIPS 			(2)
#define EDG_DS18B20_ALL_CHIPS_ENABLED_MASK 	(0x03)

typedef enum
{
	EDG_DS18B20_STATUS_ERROR,
	EDG_DS18B20_STATUS_OK,

}EDG_DS18B20_StatusTypeDef;


typedef struct __EDG_DS18B20_ChipStructTypeDef
{
	GPIO_TypeDef *GPIOx;
	uint16_t GPIO_Pin;
	EDG_DS18B20_StatusTypeDef ChipStatus;
	float Temperature;

}EDG_DS18B20_ChipStructTypeDef;


/**
  * @brief DS18B20 Handle Structure definition
  */
typedef struct __EDG_DS18B20_HandleTypeDef
{

	EDG_DS18B20_ChipStructTypeDef Chip[EDG_DS18B20_MAX_NUM_CHIPS];
	uint8_t Counter;
	uint8_t NumChipsEnabled;
	uint8_t ChipsToRead;
	uint16_t max6675_buffer;

}EDG_DS18B20_HandleTypeDef;

/**
  * @brief  RTC Handler Definition
  */

extern EDG_DS18B20_HandleTypeDef hedgDS18B20;

/* DS18B20 Functions -------------------------------------*/

void EDG_DS18B20_Init(EDG_DS18B20_HandleTypeDef *ptrhedgDS18B20);

uint8_t EDG_DS18B20_Start(EDG_DS18B20_HandleTypeDef *ptrhedgDS18B20, uint16_t NumChip);
void EDG_DS18B20_WriteByte (EDG_DS18B20_HandleTypeDef *ptrhedgDS18B20, uint16_t NumChip, uint8_t data);
uint8_t EDG_DS18B20_ReadByte(EDG_DS18B20_HandleTypeDef *ptrhedgDS18B20, uint16_t NumChip);
void EDG_DS18B20_ReadChipTemperature(EDG_DS18B20_HandleTypeDef *ptrhedgDS18B20, uint8_t NumChip);
void EDG_DS18B20_ReadAllChipsTemperature(EDG_DS18B20_HandleTypeDef *ptrhedgDS18B20);
void EDG_DS18B20_ChangePinOutput(EDG_DS18B20_HandleTypeDef *ptrhedgDS18B20, uint16_t NumChip);
void EDG_DS18B20_ChangePinInput(EDG_DS18B20_HandleTypeDef *ptrhedgDS18B20, uint16_t NumChip);
void EDG_DS18B20_TimStart(void);
void EDG_DS18B20_DelayUs(uint16_t delay);

#ifdef __cplusplus
}
#endif

#endif /* INC_EDG_WS2812_H_ */
