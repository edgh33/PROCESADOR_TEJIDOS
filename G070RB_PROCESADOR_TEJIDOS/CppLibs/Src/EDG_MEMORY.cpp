/*
 * EDG_MEMORY.cpp
 *
 *  Created on: 20/01/2023
 *      Author: Elkin Granados
 */

#include "EDG_MEMORY.h"

/**
  * @brief Funcion que inicializa la memoria
  * @note
  * @param  Address Direccion I2C de la memoria
  * @retval EDG_MEMORY_StateTypeDef Estado de la memoria
  */
EDG_MEMORY_StateTypeDef	EDG_MEMORY_InitMemory(uint8_t Address)
{

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

	if(HAL_I2C_GetState(&EDG_MEMORY_I2C_HANDLER) == HAL_I2C_STATE_READY)
	{
		return EDG_MEMORY_CheckCommI2C(Address);
	}
	else
	{
		return EDG_MEMORY_STATE_ERROR;
	}

#endif
}

/**
  * @brief Funcion para leer la memoria
  * @note
  * @param  Address Direccion I2C de la memoria
  * @param  MemAddress Dirección de la memoria a ser leida
  * @param  ptrData Puntero al arreglo donde se almacenaran los datos
  * @param  Lenght cantidad de datos que van a ser leidos
  * @retval EDG_MEMORY_StateTypeDef Estado de la memoria
  */
EDG_MEMORY_StateTypeDef	EDG_MEMORY_ReadMemory(uint8_t Address,	uint32_t MemAddress, uint8_t * ptrData, uint16_t Lenght)
{

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

	static uint16_t memAdd = 0;
	static uint8_t Add = 0;

	//Arreglo para cuando se utiliza una memoria de 1Mbit o 128KBytes
	if((MemAddress > 0xFFFF) && (MemAddress <= 0x1FFFF))
	{
		memAdd = (uint16_t)(MemAddress & 0xFFFF);
		Add = (Address | 0x01)<<1;
	}
	else if (MemAddress <= 0xFFFF)
	{
		memAdd = (uint16_t)MemAddress;
		Add = (Address<<1);
	}

	if(HAL_I2C_Mem_Read(&EDG_MEMORY_I2C_HANDLER, Add, memAdd, I2C_MEMADD_SIZE_16BIT, ptrData, Lenght, EDG_MEMORY_TIMEOUT_MS) == HAL_OK)
	{
		HAL_Delay(EDG_MEMORY_WAIT_TIME_MS);
		return EDG_MEMORY_STATE_OK;
	}
	else
	{
		HAL_Delay(EDG_MEMORY_WAIT_TIME_MS);
		return EDG_MEMORY_STATE_ERROR;
	}

#endif

}

/**
  * @brief Funcion para escribir la memoria
  * @note
  * @param  Address Direccion I2C de la memoria
  * @param  MemAddress Dirección de la memoria a ser escrita
  * @param  ptrData Puntero al arreglo donde estan datos a ser escritos
  * @param  Lenght cantidad de datos que van a ser escritos
  * @retval EDG_MEMORY_StateTypeDef Estado de la memoria
  */
EDG_MEMORY_StateTypeDef	EDG_MEMORY_WriteMemory(uint8_t Address,	uint32_t MemAddress, uint8_t * ptrData, uint16_t Lenght)
{

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

	static uint16_t memAdd = 0;
	static uint8_t Add = 0;

	//Arreglo para cuando se utiliza una memoria de 1Mbit o 128KBytes
	if((MemAddress > 0xFFFF) && (MemAddress <= 0x1FFFF))
	{
		memAdd = (uint16_t)(MemAddress & 0xFFFF);
		Add = (Address | 0x01)<<1;
	}
	else if (MemAddress <= 0xFFFF)
	{
		memAdd = (uint16_t)MemAddress;
		Add = (Address<<1);
	}

	if(HAL_I2C_Mem_Write(&EDG_MEMORY_I2C_HANDLER, Add, memAdd, I2C_MEMADD_SIZE_16BIT, ptrData, Lenght, EDG_MEMORY_TIMEOUT_MS) == HAL_OK)
	{
		HAL_Delay(EDG_MEMORY_WAIT_TIME_MS);
		return EDG_MEMORY_STATE_OK;
	}
	else
	{
		HAL_Delay(EDG_MEMORY_WAIT_TIME_MS);
		return EDG_MEMORY_STATE_ERROR;
	}

#endif

}

/**
  * @brief Funcion que revisa si hat comunicación I2C con la memoria
  * @note
  * @param  Address Direccion I2C de la memoria
  * @retval EDG_MEMORY_StateTypeDef Estado de la memoria
  */
EDG_MEMORY_StateTypeDef	EDG_MEMORY_CheckCommI2C(uint8_t Address)
{
	static uint8_t dataTemp1, dataTemp2 = 0;

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

	dataTemp1 = EDG_MEMORY_MEM_DATA_TEST;
	dataTemp2 = 0;

	if(EDG_MEMORY_WriteMemory(Address, EDG_MEMORY_MEM_ADD_TEST, &dataTemp1, 1) == EDG_MEMORY_STATE_OK)
	{
		if(EDG_MEMORY_ReadMemory(Address, EDG_MEMORY_MEM_ADD_TEST, &dataTemp2, 1) == EDG_MEMORY_STATE_OK)
		{
			if(dataTemp1 == dataTemp2)
			{
				return EDG_MEMORY_STATE_OK;
			}
			else
			{
				return EDG_MEMORY_STATE_ERROR;
			}
		}
	}
	else
	{
		return EDG_MEMORY_STATE_ERROR;
	}

#endif

	return EDG_MEMORY_STATE_OK;
}
