/*
 * EDG_DS18B20.cpp
 *
 *  Created on: Apr 07, 2025
 *      Author: Elkin Granados
 */

#include "EDG_DS18B20.h"

GPIO_TypeDef * Ports[EDG_DS18B20_MAX_NUM_CHIPS] = {TEMP1_GPIO_Port, TEMP2_GPIO_Port};
uint16_t  Pins[EDG_DS18B20_MAX_NUM_CHIPS] = {TEMP1_Pin, TEMP2_Pin};

EDG_DS18B20_HandleTypeDef hedgDS18B20;

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_DS18B20_Init(EDG_DS18B20_HandleTypeDef *ptrhedgDS18B20)
{

	ptrhedgDS18B20->ChipsToRead = EDG_DS18B20_MAX_NUM_CHIPS;
	ptrhedgDS18B20->NumChipsEnabled = EDG_DS18B20_ALL_CHIPS_ENABLED_MASK;
	EDG_DS18B20_TimStart();

	//Se dejan valores por defecto en todas las banderas antes de iniciarlas
	for(ptrhedgDS18B20->Counter = 0; ptrhedgDS18B20->Counter < ptrhedgDS18B20->ChipsToRead; ptrhedgDS18B20->Counter++)
	{
		ptrhedgDS18B20->Chip[ptrhedgDS18B20->Counter].GPIOx = Ports[ptrhedgDS18B20->Counter];
		ptrhedgDS18B20->Chip[ptrhedgDS18B20->Counter].GPIO_Pin = Pins[ptrhedgDS18B20->Counter];
		EDG_DS18B20_ChangePinOutput(ptrhedgDS18B20, ptrhedgDS18B20->Counter);   // set the pin as output
		HAL_GPIO_WritePin (ptrhedgDS18B20->Chip[ptrhedgDS18B20->Counter].GPIOx, ptrhedgDS18B20->Chip[ptrhedgDS18B20->Counter].GPIO_Pin, GPIO_PIN_SET);  // pull the pin low
		if(EDG_DS18B20_Start(ptrhedgDS18B20, ptrhedgDS18B20->Counter))
		{

			//EDG_DS18B20_ReadChipTemperature(ptrhedgDS18B20, ptrhedgDS18B20->Counter);
			EDG_DS18B20_FirstReadChipTemperature(ptrhedgDS18B20, ptrhedgDS18B20->Counter);
			//ptrhedgDS18B20->Chip[ptrhedgDS18B20->Counter].ChipStatus= EDG_DS18B20_STATUS_OK;
		}
		else
		{
			ptrhedgDS18B20->Chip[ptrhedgDS18B20->Counter].ChipStatus= EDG_DS18B20_STATUS_ERROR;
		}

	}

	//EDG_DS18B20_DelayUs(10);
	//EDG_DS18B20_ReadAllChipsTemperature(ptrhedgDS18B20);

	return;
}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
uint8_t EDG_DS18B20_Start(EDG_DS18B20_HandleTypeDef *ptrhedgDS18B20, uint16_t NumChip)
{

	uint8_t Response = 0;
	EDG_DS18B20_ChangePinOutput(ptrhedgDS18B20, NumChip);   // set the pin as output
	HAL_GPIO_WritePin (ptrhedgDS18B20->Chip[NumChip].GPIOx, ptrhedgDS18B20->Chip[NumChip].GPIO_Pin, GPIO_PIN_RESET);  // pull the pin low
	EDG_DS18B20_DelayUs(480);   // delay according to datasheet

	EDG_DS18B20_ChangePinInput(ptrhedgDS18B20, NumChip);    // set the pin as input
	EDG_DS18B20_DelayUs(80);    // delay according to datasheet
	if (!(HAL_GPIO_ReadPin (ptrhedgDS18B20->Chip[NumChip].GPIOx, ptrhedgDS18B20->Chip[NumChip].GPIO_Pin)))
	{
		Response = 1;    // if the pin is low i.e the presence pulse is detected
	}
	else
	{
		Response = 0;
	}

	EDG_DS18B20_DelayUs(400); // 480 us delay totally.

	EDG_DS18B20_ChangePinOutput(ptrhedgDS18B20, NumChip);   // set the pin as output
	HAL_GPIO_WritePin (ptrhedgDS18B20->Chip[NumChip].GPIOx, ptrhedgDS18B20->Chip[NumChip].GPIO_Pin, GPIO_PIN_SET);  // pull the pin high
	return Response;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_DS18B20_WriteByte(EDG_DS18B20_HandleTypeDef *ptrhedgDS18B20, uint16_t NumChip, uint8_t data)
{

	for (int i=0; i<8; i++)
	{
		if ((data & (1<<i))!=0)  // if the bit is high
		{
			// write 1
			EDG_DS18B20_ChangePinOutput(ptrhedgDS18B20, NumChip);  // set as output
			HAL_GPIO_WritePin (ptrhedgDS18B20->Chip[NumChip].GPIOx, ptrhedgDS18B20->Chip[NumChip].GPIO_Pin, GPIO_PIN_RESET);  // pull the pin LOW
			EDG_DS18B20_DelayUs(1);  // wait for 1 us
			EDG_DS18B20_ChangePinInput(ptrhedgDS18B20, NumChip); // set as input
		}
		else  // if the bit is low
		{
			// write 0
			EDG_DS18B20_ChangePinOutput(ptrhedgDS18B20, NumChip);  // set as output
			HAL_GPIO_WritePin (ptrhedgDS18B20->Chip[NumChip].GPIOx, ptrhedgDS18B20->Chip[NumChip].GPIO_Pin, GPIO_PIN_RESET);  // pull the pin LOW
			EDG_DS18B20_DelayUs(60);  // wait for 60 us
			EDG_DS18B20_ChangePinInput(ptrhedgDS18B20, NumChip); // set as input
		}
		EDG_DS18B20_DelayUs(10);  // wait for 10 us
	}

	return;
}

uint8_t EDG_DS18B20_ReadByte(EDG_DS18B20_HandleTypeDef *ptrhedgDS18B20, uint16_t NumChip)
{
	uint8_t value=0;

	for (int i=0;i<8;i++)
	{

		EDG_DS18B20_ChangePinOutput(ptrhedgDS18B20, NumChip);   // set as output
		HAL_GPIO_WritePin (ptrhedgDS18B20->Chip[NumChip].GPIOx, ptrhedgDS18B20->Chip[NumChip].GPIO_Pin, GPIO_PIN_RESET);  // pull the pin LOW
		__HAL_TIM_SET_COUNTER(&EDG_DS18B20_TIM_BASE_HANDLER, 0);
		EDG_DS18B20_ChangePinInput(ptrhedgDS18B20, NumChip); // set as input takes aprox 6us
		while((!HAL_GPIO_ReadPin (ptrhedgDS18B20->Chip[NumChip].GPIOx, ptrhedgDS18B20->Chip[NumChip].GPIO_Pin))
				&& (__HAL_TIM_GET_COUNTER(&EDG_DS18B20_TIM_BASE_HANDLER) < (16 * 64)));
		if(__HAL_TIM_GET_COUNTER(&EDG_DS18B20_TIM_BASE_HANDLER) < (16 * 64)) // check that the time of changhe is less than ~15us
		{
			value |= 1<<i;  // read = 1
		}

//		EDG_DS18B20_ChangePinOutput(ptrhedgDS18B20, NumChip);   // set as output
//		HAL_GPIO_WritePin (ptrhedgDS18B20->Chip[NumChip].GPIOx, ptrhedgDS18B20->Chip[NumChip].GPIO_Pin, GPIO_PIN_RESET);  // pull the pin LOW
//		EDG_DS18B20_DelayUs(1);  // wait for 1 us
//		EDG_DS18B20_ChangePinInput(ptrhedgDS18B20, NumChip); // set as input takes aprox 6us
//		//EDG_DS18B20_DelayUs(1);  // wait for 1 us
////		if (HAL_GPIO_ReadPin (ptrhedgDS18B20->Chip[NumChip].GPIOx, ptrhedgDS18B20->Chip[NumChip].GPIO_Pin))  // if the pin is HIGH
////		{
////			value |= 1<<i;  // read = 1
////		}
//
//		__HAL_TIM_SET_COUNTER(&EDG_DS18B20_TIM_BASE_HANDLER, 0);
//		while(!HAL_GPIO_ReadPin (ptrhedgDS18B20->Chip[NumChip].GPIOx, ptrhedgDS18B20->Chip[NumChip].GPIO_Pin));
//		if(__HAL_TIM_GET_COUNTER(&EDG_DS18B20_TIM_BASE_HANDLER) < (7 * 64)) // check that the time of changhe is less than ~15us
//		{
//			value |= 1<<i;  // read = 1
//		}

		EDG_DS18B20_DelayUs(60);  // wait for 60 us
	}

	return value;
}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_DS18B20_FirstReadChipTemperature(EDG_DS18B20_HandleTypeDef *ptrhedgDS18B20, uint8_t NumChip)
{
	uint16_t Temp_byte1 = 0;
	uint16_t Temp_byte2 = 0;
	uint16_t TEMP = 0;
	uint8_t read_tries = 0;
	uint8_t read_errors = 0;
	float TempTemp = 0;

	ptrhedgDS18B20->Chip[NumChip].Temperature = 0;
	ptrhedgDS18B20->Chip[NumChip].ChipStatus = EDG_DS18B20_STATUS_ERROR;
	ptrhedgDS18B20->Chip[NumChip].ReadStatus = EDG_DS18B20_READ_STATUS_ERROR;

	do
	{
		if(EDG_DS18B20_Start(ptrhedgDS18B20, NumChip) == 0)
		{
			read_errors++;
		}
		else
		{
			EDG_DS18B20_WriteByte(ptrhedgDS18B20, NumChip, 0xCC);  // skip ROM
			EDG_DS18B20_WriteByte(ptrhedgDS18B20, NumChip, 0x44);  // convert t

			if(EDG_DS18B20_Start(ptrhedgDS18B20, NumChip) == 0)
			{
				read_errors++;
			}
			else
			{
				EDG_DS18B20_WriteByte(ptrhedgDS18B20, NumChip, 0xCC);  // skip ROM
				EDG_DS18B20_WriteByte(ptrhedgDS18B20, NumChip, 0xBE);  // Read Scratch-pad

				Temp_byte1 = EDG_DS18B20_ReadByte(ptrhedgDS18B20, NumChip);
				Temp_byte2 = EDG_DS18B20_ReadByte(ptrhedgDS18B20, NumChip);
				TEMP = ((Temp_byte2<<8))|Temp_byte1;
				if(TEMP != 0xFFFF)
				{
					TempTemp = ((float)(TEMP & 0x7FF) * 0.0625);
					if(((TEMP >> 11) & 0x001F) == 0x001F)
					{
						TempTemp *= -1;
					}
					ptrhedgDS18B20->Chip[NumChip].Temperature += TempTemp;
					read_tries++;
				}
				else
				{
					read_errors++;
				}
			}
		}

		EDG_DS18B20_DelayUs(EDG_DS18B20_DELAY_BETWEEN_READ_US);  // wait for a time if need to read again

	}while((read_tries < EDG_DS18B20_MAX_READ_TRIES) && (read_errors < EDG_DS18B20_MAX_READ_TRIES));

	if(read_tries == EDG_DS18B20_MAX_READ_TRIES)
	{
		ptrhedgDS18B20->Chip[NumChip].Temperature /= EDG_DS18B20_MAX_READ_TRIES;
		ptrhedgDS18B20->Chip[NumChip].ChipStatus = EDG_DS18B20_STATUS_OK;
		ptrhedgDS18B20->Chip[NumChip].ReadStatus = EDG_DS18B20_READ_STATUS_OK;
	}
	else
	{
		ptrhedgDS18B20->Chip[NumChip].Temperature = 0;
		ptrhedgDS18B20->Chip[NumChip].ChipStatus = EDG_DS18B20_STATUS_ERROR;
		ptrhedgDS18B20->Chip[NumChip].ReadStatus = EDG_DS18B20_READ_STATUS_ERROR;
	}

	return;
}


/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_DS18B20_ReadChipTemperature(EDG_DS18B20_HandleTypeDef *ptrhedgDS18B20, uint8_t NumChip)
{

	uint16_t Temp_byte1 = 0;
	uint16_t Temp_byte2 = 0;
	uint16_t TEMP = 0;
	uint8_t read_tries = 0;
	uint8_t read_errors = 0;
	float PastTemp = 0.0;
	float Error = 0;

	PastTemp = ptrhedgDS18B20->Chip[NumChip].Temperature;
	ptrhedgDS18B20->Chip[NumChip].Temperature = 0.0;
	ptrhedgDS18B20->Chip[NumChip].ChipStatus = EDG_DS18B20_STATUS_ERROR;
	ptrhedgDS18B20->Chip[NumChip].ReadStatus = EDG_DS18B20_READ_STATUS_ERROR;

	do
	{
		if(EDG_DS18B20_Start(ptrhedgDS18B20, NumChip) == 0)
		{
			read_errors++;
		}
		else
		{

			EDG_DS18B20_WriteByte(ptrhedgDS18B20, NumChip, 0xCC);  // skip ROM
			EDG_DS18B20_WriteByte(ptrhedgDS18B20, NumChip, 0x44);  // convert t

			if(EDG_DS18B20_Start(ptrhedgDS18B20, NumChip) == 0)
			{
				read_errors++;
			}
			else
			{
				EDG_DS18B20_WriteByte(ptrhedgDS18B20, NumChip, 0xCC);  // skip ROM
				EDG_DS18B20_WriteByte(ptrhedgDS18B20, NumChip, 0xBE);  // Read Scratch-pad

				Temp_byte1 = EDG_DS18B20_ReadByte(ptrhedgDS18B20, NumChip);
				Temp_byte2 = EDG_DS18B20_ReadByte(ptrhedgDS18B20, NumChip);
				TEMP = ((Temp_byte2<<8))|Temp_byte1;
				if(TEMP != 0xFFFF)
				{
					ptrhedgDS18B20->Chip[NumChip].Temperature = ((float)(TEMP & 0x7FF) * 0.0625);
					if(((TEMP >> 11) & 0x001F) == 0x001F)
					{
						ptrhedgDS18B20->Chip[NumChip].Temperature *= -1;
					}
					Error = ptrhedgDS18B20->Chip[NumChip].Temperature - PastTemp;
					if(Error < 0)
					{
						Error *= -1;
					}
//					if(Error >= EDG_DS18B20_MAX_READ_DIFFERENCE)
//					{
//						read_tries++;
//					}
//					else
//					{
//						ptrhedgDS18B20->Chip[NumChip].ChipStatus = EDG_DS18B20_STATUS_OK;
//						ptrhedgDS18B20->Chip[NumChip].ReadStatus = EDG_DS18B20_READ_STATUS_OK;
//					}
					ptrhedgDS18B20->Chip[NumChip].ChipStatus = EDG_DS18B20_STATUS_OK;
					ptrhedgDS18B20->Chip[NumChip].ReadStatus = EDG_DS18B20_READ_STATUS_OK;
				}
				else
				{
					read_tries++;
				}
			}
		}
		EDG_DS18B20_DelayUs(EDG_DS18B20_DELAY_BETWEEN_READ_US);  // wait for a time if need to read again

	}while((read_tries < EDG_DS18B20_MAX_READ_TRIES)
			&& (read_errors < EDG_DS18B20_MAX_READ_TRIES)
			&& (ptrhedgDS18B20->Chip[NumChip].ChipStatus == EDG_DS18B20_STATUS_ERROR)
			);

	if(read_tries == EDG_DS18B20_MAX_READ_TRIES)
	{
		ptrhedgDS18B20->Chip[NumChip].ChipStatus = EDG_DS18B20_STATUS_OK;
		ptrhedgDS18B20->Chip[NumChip].Temperature = PastTemp;
	}

//	if(EDG_DS18B20_Start(ptrhedgDS18B20, NumChip) == 0)
//	{
//		ptrhedgDS18B20->Chip[NumChip].ChipStatus = EDG_DS18B20_STATUS_ERROR;
//		return;
//	}
//	else
//	{
//		ptrhedgDS18B20->Chip[NumChip].ChipStatus = EDG_DS18B20_STATUS_OK;
//		EDG_DS18B20_WriteByte(ptrhedgDS18B20, NumChip, 0xCC);  // skip ROM
//		EDG_DS18B20_WriteByte(ptrhedgDS18B20, NumChip, 0x44);  // convert t
//	}
//
//	if(EDG_DS18B20_Start(ptrhedgDS18B20, NumChip) == 0)
//	{
//		ptrhedgDS18B20->Chip[NumChip].ChipStatus = EDG_DS18B20_STATUS_ERROR;
//		return;
//	}
//	else
//	{
//		ptrhedgDS18B20->Chip[NumChip].ChipStatus = EDG_DS18B20_STATUS_OK;
//		EDG_DS18B20_WriteByte(ptrhedgDS18B20, NumChip, 0xCC);  // skip ROM
//		EDG_DS18B20_WriteByte(ptrhedgDS18B20, NumChip, 0xBE);  // Read Scratch-pad
//	}
//
//	Temp_byte1 = EDG_DS18B20_ReadByte(ptrhedgDS18B20, NumChip);
//	Temp_byte2 = EDG_DS18B20_ReadByte(ptrhedgDS18B20, NumChip);
//	TEMP = ((Temp_byte2<<8))|Temp_byte1;
//
//	ptrhedgDS18B20->Chip[NumChip].Temperature = ((float)(TEMP & 0x7FF) * 0.0625);
//	if(((TEMP >> 11) & 0x001F) == 0x001F)
//	{
//		ptrhedgDS18B20->Chip[NumChip].Temperature *= -1;
//	}

	return;
}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_DS18B20_ReadAllChipsTemperature(EDG_DS18B20_HandleTypeDef *ptrhedgDS18B20)
{
	uint8_t tries = 0;

	for(ptrhedgDS18B20->Counter = 0; ptrhedgDS18B20->Counter < ptrhedgDS18B20->ChipsToRead; ptrhedgDS18B20->Counter++)
	{
		do
		{
			EDG_DS18B20_ReadChipTemperature(ptrhedgDS18B20, ptrhedgDS18B20->Counter);
			if(ptrhedgDS18B20->Chip[ptrhedgDS18B20->Counter].ReadStatus == EDG_DS18B20_READ_STATUS_ERROR)
			{
				EDG_DS18B20_FirstReadChipTemperature(ptrhedgDS18B20, ptrhedgDS18B20->Counter);
				tries++;
			}
		}while( (tries < 2) && (ptrhedgDS18B20->Chip[ptrhedgDS18B20->Counter].ReadStatus == EDG_DS18B20_READ_STATUS_ERROR));
		//EDG_DS18B20_ReadChipTemperature(ptrhedgDS18B20, ptrhedgDS18B20->Counter);
	}
	return;
}


/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_DS18B20_ChangePinOutput(EDG_DS18B20_HandleTypeDef *ptrhedgDS18B20, uint16_t NumChip)
{

//	GPIO_InitTypeDef GPIO_InitStruct = {0};
//	//Configure GPIO pins : PBPin PBPin
//	GPIO_InitStruct.Pin = ptrhedgDS18B20->Chip[NumChip].GPIO_Pin;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//	HAL_GPIO_Init(ptrhedgDS18B20->Chip[NumChip].GPIOx, &GPIO_InitStruct);


	uint32_t position = 0x00u;
	uint32_t temp;

	while(!((ptrhedgDS18B20->Chip[NumChip].GPIO_Pin >> position) & 0x00000001))
	{
		position++;
	}

	/* Configure the IO Speed */
	temp = ptrhedgDS18B20->Chip[NumChip].GPIOx->OSPEEDR;
	temp &= ~(GPIO_OSPEEDR_OSPEED0 << (position * 2u));
	temp |= (GPIO_SPEED_FREQ_LOW << (position * 2u));
	ptrhedgDS18B20->Chip[NumChip].GPIOx->OSPEEDR = temp;

	/* Configure the IO Output Type */
	temp = ptrhedgDS18B20->Chip[NumChip].GPIOx->OTYPER;
	temp &= ~(GPIO_OTYPER_OT0 << position) ;
	temp |= (((GPIO_MODE_OUTPUT_OD & OUTPUT_TYPE) >> OUTPUT_TYPE_Pos) << position);
	ptrhedgDS18B20->Chip[NumChip].GPIOx->OTYPER = temp;

	/* Activate the Pull-up or Pull down resistor for the current IO */
	temp = ptrhedgDS18B20->Chip[NumChip].GPIOx->PUPDR;
	temp &= ~(GPIO_PUPDR_PUPD0 << (position * 2u));
	temp |= ((GPIO_NOPULL) << (position * 2u));
	ptrhedgDS18B20->Chip[NumChip].GPIOx->PUPDR = temp;

	/* Configure IO Direction mode (Input, Output, Alternate or Analog) */
	temp = ptrhedgDS18B20->Chip[NumChip].GPIOx->MODER;
	temp &= ~(GPIO_MODER_MODE0 << (position * 2u));
	temp |= ((GPIO_MODE_OUTPUT_OD & GPIO_MODE) << (position * 2u));
	ptrhedgDS18B20->Chip[NumChip].GPIOx->MODER = temp;

	return;
}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_DS18B20_ChangePinInput(EDG_DS18B20_HandleTypeDef *ptrhedgDS18B20, uint16_t NumChip)
{

//	GPIO_InitTypeDef GPIO_InitStruct = {0};
//	GPIO_InitStruct.Pin = ptrhedgDS18B20->Chip[NumChip].GPIO_Pin;
//	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	HAL_GPIO_Init(ptrhedgDS18B20->Chip[NumChip].GPIOx, &GPIO_InitStruct);


	uint32_t position = 0x00u;
	uint32_t temp;

	while(!((ptrhedgDS18B20->Chip[NumChip].GPIO_Pin >> position) & 0x00000001))
	{
		position++;
	}

	/* Activate the Pull-up or Pull down resistor for the current IO */
	temp = ptrhedgDS18B20->Chip[NumChip].GPIOx->PUPDR;
	temp &= ~(GPIO_PUPDR_PUPD0 << (position * 2u));
	temp |= ((GPIO_NOPULL) << (position * 2u));
	ptrhedgDS18B20->Chip[NumChip].GPIOx->PUPDR = temp;


	/* Configure IO Direction mode (Input, Output, Alternate or Analog) */
	temp = ptrhedgDS18B20->Chip[NumChip].GPIOx->MODER;
	temp &= ~(GPIO_MODER_MODE0 << (position * 2u));
	temp |= ((GPIO_MODE_INPUT & GPIO_MODE) << (position * 2u));
	ptrhedgDS18B20->Chip[NumChip].GPIOx->MODER = temp;

	return;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_DS18B20_TimStart(void)
{
	HAL_TIM_Base_Start(&EDG_DS18B20_TIM_BASE_HANDLER);
	return;
}
/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_DS18B20_DelayUs(uint16_t delay)
{
	delay = (delay * 64) - 1;
	__HAL_TIM_SET_COUNTER(&EDG_DS18B20_TIM_BASE_HANDLER, 0);
	//HAL_TIM_Base_Start(&EDG_DS18B20_TIM_BASE_HANDLER);
	while(__HAL_TIM_GET_COUNTER(&EDG_DS18B20_TIM_BASE_HANDLER) < delay);
	//HAL_TIM_Base_Stop(&EDG_DS18B20_TIM_BASE_HANDLER);
	return;

}
