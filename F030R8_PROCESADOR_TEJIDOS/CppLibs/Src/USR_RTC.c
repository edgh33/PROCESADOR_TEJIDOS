/*
 * USR_RTC.c
 *
 *  Created on: 1/09/2018
 *      Author: edgh3
 */

#include "USR_RTC.h"

uint8_t usr_rtc_config[USR_RTC_REGISTRO_TAM]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t usr_rtc_read[USR_RTC_REGISTRO_TAM]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t usr_rtc_set_reg[1]={0x00};

usr_rtc_estado usr_rtc_estado_actual = USR_RTC_ESTADO_OK;

void USR_RTC_Leer_Fecha_Hora(void)
{

	HAL_StatusTypeDef error_hal = HAL_ERROR;

	error_hal = HAL_I2C_Mem_Read(&hi2c2, (uint16_t)(USR_RTC_DEV_ADDRESS<<1), (uint16_t)0x00, 1, usr_rtc_read, sizeof(usr_rtc_read), 10);
	if(error_hal != HAL_OK)
	{
		if (HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)
		{
		Error_Handler();
		}
	}

	if(usr_rtc_read[7] & USR_RTC_STOP_FLAG)
	{
		error_hal = HAL_I2C_Mem_Write(&hi2c2, (uint16_t)(USR_RTC_DEV_ADDRESS<<1), (uint16_t)0x07, 1, usr_rtc_set_reg, sizeof(usr_rtc_set_reg), 10);

		if(error_hal != HAL_OK)
		{
			if (HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)
			{
			Error_Handler();
			}
		}

	}

	if(RTC_Bcd2ToByte(usr_rtc_read[USR_RTC_POSICION_ANHO]) < USR_RTC_ANHO_ACTUAL)
	{
		usr_rtc_estado_actual = USR_RTC_ESTADO_ERROR;
	}
	else
	{
		usr_rtc_estado_actual = USR_RTC_ESTADO_OK;
	}

}

void USR_RTC_Escribir_Fecha_Hora(void)
{

	HAL_StatusTypeDef error_hal = HAL_ERROR;

	error_hal = HAL_I2C_Mem_Write(&hi2c2, (uint16_t)(USR_RTC_DEV_ADDRESS<<1), (uint16_t)0x00, 1, usr_rtc_config, sizeof(usr_rtc_config), 10);
	if(error_hal != HAL_OK)
	{
		if (HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)
		{
			usr_rtc_estado_actual = USR_RTC_ESTADO_ERROR;
		}
	}
	else
	{
		usr_rtc_estado_actual = USR_RTC_ESTADO_OK;
	}
}

usr_rtc_estado USR_RTC_Comparar(void)
{
	uint8_t contador = 1;

	USR_RTC_Leer_Fecha_Hora();
	for(contador = 1; contador < USR_RTC_REGISTRO_TAM-1; contador++)
	{
		if(usr_rtc_config[contador] != usr_rtc_read[contador])
		{
			return USR_RTC_ESTADO_ERROR;
		}
	}

	return USR_RTC_ESTADO_OK;

}
