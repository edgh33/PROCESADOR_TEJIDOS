/*
 * ENG_MEMORY.h
 *
 *  Created on: 20/01/2023
 *      Author: Elkin Granados
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EDG_MEMORY_H_
#define EDG_MEMORY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "EDG_CONFIG.h"

#if ENG_CONFIG_IDE_SEL == ENG_CONFIG_IDE_CUBEIDE

#include "main.h"
#include "i2c.h"

#endif

/* MEMORY Definitions -------------------------------------------------------*/

#define	EDG_MEMORY_I2C_PORT			I2C1	//Puerto I2C
#define EDG_MEMORY_I2C_HANDLER		hi2c1
#define EDG_MEMORY_TIMEOUT_MS		(100)
#define EDG_MEMORY_WAIT_TIME_MS		(8)

#define EDG_MEMORY_ADDRESS_MEM1		(0x50)

#define EDG_MEMORY_MEM_ADD_TEST		(0xFFFF)
#define	EDG_MEMORY_MEM_DATA_TEST	(0xAF)

/**
  * @brief enum Memory State type definition
  * @note  Enumeracion del estado de la memoria
  */
typedef enum
{
	EDG_MEMORY_STATE_OK,
	EDG_MEMORY_STATE_ERROR,

}EDG_MEMORY_StateTypeDef;


/* MEMORY Functions -------------------------------------*/

EDG_MEMORY_StateTypeDef	EDG_MEMORY_InitMemory(uint8_t Address);
EDG_MEMORY_StateTypeDef	EDG_MEMORY_ReadMemory(uint8_t Address, uint32_t MemAddress, uint8_t * ptrData, uint16_t Lenght);
EDG_MEMORY_StateTypeDef	EDG_MEMORY_WriteMemory(uint8_t Address, uint32_t MemAddress, uint8_t * ptrData, uint16_t Lenght);
EDG_MEMORY_StateTypeDef	EDG_MEMORY_CheckCommI2C(uint8_t Address);

#ifdef __cplusplus
}
#endif

#endif /* EDG_MEMORY_H_ */
