/*
 * EDG_MAX6675.cpp
 *
 *  Created on: Mar 19, 2023
 *      Author: Elkin Granados
 */


#include "EDG_MAX6675.h"

EDG_MAX6675_HandleTypeDef hedgMAX6675;

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_MAX6675_Init(EDG_MAX6675_HandleTypeDef *ptrhedgMAX6675)
{

	ptrhedgMAX6675->ChipsToRead = EDG_MAX6675_ALL_CHIPS_ENABLED_MASK;
	ptrhedgMAX6675->NumChipsEnabled = EDG_MAX6675_MAX_NUM_CHIPS;

	//Se dejan valores por defecto en todas las banderas antes de iniciarlas
	for(ptrhedgMAX6675->Counter = 0; ptrhedgMAX6675->Counter < EDG_MAX6675_MAX_NUM_CHIPS; ptrhedgMAX6675->Counter++)
	{
		ptrhedgMAX6675->Chip[ptrhedgMAX6675->Counter].ChipStatus= EDG_MAX6675_CHIP_STATUS_DEFAULT;
		ptrhedgMAX6675->Chip[ptrhedgMAX6675->Counter].Temperature = 0.0;
	}

	EDG_MAX6675_ReadAllChips(ptrhedgMAX6675);

	return;
}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_MAX6675_ReadAllChips(EDG_MAX6675_HandleTypeDef *ptrhedgMAX6675)
{

	for(ptrhedgMAX6675->Counter = 0; ptrhedgMAX6675->Counter < EDG_MAX6675_MAX_NUM_CHIPS; ptrhedgMAX6675->Counter++)
	{
		EDG_MAX6675_ReadChip(ptrhedgMAX6675, ptrhedgMAX6675->Counter);
	}
	return;
}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_MAX6675_ReadChip(EDG_MAX6675_HandleTypeDef *ptrhedgMAX6675, uint8_t NumChip)
{

	HAL_StatusTypeDef res;
	uint16_t value;

	//Se verifica si el chip si esta habilitado para lectura
	if((ptrhedgMAX6675->ChipsToRead) & (1U<<NumChip))
	{
		EDG_MAX6675_WriteCsChip(NumChip, GPIO_PIN_RESET);

		res = HAL_SPI_Receive(&EDG_MAX6675_SPI_HANDLE, (uint8_t *)&ptrhedgMAX6675->max6675_buffer, 1, EDG_MAX6675_SPI_TIMEOUT_MS);

		if(res == HAL_OK)
		{
			value = ptrhedgMAX6675->max6675_buffer;
			if(value == 0x0000)
			{
				ptrhedgMAX6675->Chip[NumChip].Temperature = 0.0;
				ptrhedgMAX6675->Chip[NumChip].ChipStatus = EDG_MAX6675_CHIP_STATUS_CHIP_ERROR;
			}
			else
			{
				if(value & EDG_MAX6675_OPEN_TC_MASK)
				{
					ptrhedgMAX6675->Chip[NumChip].Temperature  = 0;
					ptrhedgMAX6675->Chip[NumChip].ChipStatus = EDG_MAX6675_CHIP_STATUS_TC_OPEN;
				}
				else
				{
					ptrhedgMAX6675->Chip[NumChip].Temperature = (float)((value >> 5) & 0x03FF) + ((float)((value >> 3) & 0x0003) * 0.25);
					ptrhedgMAX6675->Chip[NumChip].ChipStatus = EDG_MAX6675_CHIP_STATUS_OK;
				}
			}
		}
		else
		{
			ptrhedgMAX6675->Chip[NumChip].ChipStatus = EDG_MAX6675_CHIP_STATUS_COMM_ERROR;
		}

		EDG_MAX6675_WriteCsChip(NumChip, GPIO_PIN_SET);

	}
	else
	{

		ptrhedgMAX6675->Chip[NumChip].ChipStatus = EDG_MAX6675_CHIP_STATUS_NOT_ENABLE;

	}

	return;
}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_MAX6675_WriteCsChip(uint8_t NumChip, GPIO_PinState PinState)
{
	switch(NumChip)
	{
		case 0:
			HAL_GPIO_WritePin(EDG_MAX6675_CS0_PORT, EDG_MAX6675_CS0_PIN, PinState);
			break;

		case 1:
			HAL_GPIO_WritePin(EDG_MAX6675_CS1_PORT, EDG_MAX6675_CS1_PIN, PinState);
			break;

		case 2:
			HAL_GPIO_WritePin(EDG_MAX6675_CS2_PORT, EDG_MAX6675_CS2_PIN, PinState);
			break;

		case 3:
			HAL_GPIO_WritePin(EDG_MAX6675_CS3_PORT, EDG_MAX6675_CS3_PIN, PinState);
			break;

		case 4:
			HAL_GPIO_WritePin(EDG_MAX6675_CS4_PORT, EDG_MAX6675_CS4_PIN, PinState);
			break;

		case 5:
			HAL_GPIO_WritePin(EDG_MAX6675_CS5_PORT, EDG_MAX6675_CS5_PIN, PinState);
			break;

		default:

			break;
	}

	return;
}


