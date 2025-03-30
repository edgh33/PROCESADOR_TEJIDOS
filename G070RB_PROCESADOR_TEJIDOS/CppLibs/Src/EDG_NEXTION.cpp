/*
 * EDG_NEXTION.cpp
 *
 *  Created on: 08/04/2023
 *      Author: Elkin Granados
 */

/*** Include header file ***/
#include "EDG_NEXTION.h"

EDG_NEXTION_HandleTypeDef hedgNextion;

const uint8_t EDG_NEXTION_WeekdayNames[][10] = { "", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado", "Domingo"};

const uint8_t EDG_NEXTION_MonthNames[][11] = { "", "Enero", "Febrero", "Marzo",  "Abril", "Mayo", "Junio",
								   	   	       "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"};

const uint8_t EDG_NEXTION_AmPmNames[][3] = {"am", "pm"};

/**
  * @brief
  * @param
  * @retval
  */
void EDG_NEXTION_Init(EDG_NEXTION_HandleTypeDef * ptrhedgNextion)
{

	memset(ptrhedgNextion->RxFrame, 0, sizeof(ptrhedgNextion->RxFrame));
	memset(ptrhedgNextion->TxFrame, 0, sizeof(ptrhedgNextion->TxFrame));
	memset(ptrhedgNextion->DataReceived, 0, (EDG_NEXTION_MAX_DATA_RECEIVED * 4));
	ptrhedgNextion->CurrentPage = EDG_NEXTION_PAGE_START;
	ptrhedgNextion->RxFrameStatus = EDG_NEXTION_RX_FRAME_STATUS_NOT_RECEIVED;
	ptrhedgNextion->CommandReceived = EDG_NEXTION_COMMAND_NONE;
	ptrhedgNextion->DataReceivedLen = 0;
	ptrhedgNextion->Endflag[0]= 0xFF;
	ptrhedgNextion->Endflag[1]= 0xFF;
	ptrhedgNextion->Endflag[2]= 0xFF;
	ptrhedgNextion->Endflag[3]= 0x00;

	ptrhedgNextion->Bright.Status = EDG_NEXTION_BRIGHT_STATUS_NORMAL;
	ptrhedgNextion->Bright.Level = 100;

	EDG_NEXTION_RestartBrightTim(&hedgNextion);

	return;

}

/**
  * @brief
  * @param
  * @retval
  */
EDG_NEXTION_StatusTypeDef EDG_NEXTION_StartReceiveFrame(EDG_NEXTION_HandleTypeDef * ptrhedgNextion)
{

	/*** Clear Flags ***/
	__HAL_UART_CLEAR_OREFLAG(&EDG_NEXTION_USART_PORT_HANDLE);
	__HAL_UART_CLEAR_FEFLAG(&EDG_NEXTION_USART_PORT_HANDLE);
	__HAL_UART_CLEAR_NEFLAG(&EDG_NEXTION_USART_PORT_HANDLE);
	__HAL_UART_CLEAR_OREFLAG(&EDG_NEXTION_USART_PORT_HANDLE);
	/*** Flush Register ***/
	__HAL_UART_FLUSH_DRREGISTER(&EDG_NEXTION_USART_PORT_HANDLE);
	/*** Set RX Buffer ***/
	memset(ptrhedgNextion->RxFrame, 0, sizeof(ptrhedgNextion->RxFrame));
	ptrhedgNextion->RxFrameStatus = EDG_NEXTION_RX_FRAME_STATUS_NOT_RECEIVED;
	/*** Start RX ***/
	if(HAL_UART_Receive_IT(&EDG_NEXTION_USART_PORT_HANDLE, ptrhedgNextion->RxFrame, 1) == HAL_OK)
	{
		return EDG_NEXTION_STATUS_OK;
	}
	else
	{
		return EDG_NEXTION_STATUS_ERROR;
	}

}

/**
  * @brief
  * @param
  * @retval
  */
EDG_NEXTION_StatusTypeDef EDG_NEXTION_StopReceiveFrame(void)
{

	if(HAL_UART_AbortReceive_IT(&EDG_NEXTION_USART_PORT_HANDLE) == HAL_OK)
	{
		return EDG_NEXTION_STATUS_OK;
	}
	else
	{
		return EDG_NEXTION_STATUS_ERROR;
	}

}

/**
  * @brief
  * @param
  * @retval
  */
EDG_NEXTION_StatusTypeDef EDG_NEXTION_ReceiveFrame(EDG_NEXTION_HandleTypeDef * ptrhedgNextion)
{

	uint8_t FlagError = 0;
	uint8_t * pbuffer_rx = ptrhedgNextion->RxFrame;
	uint8_t * CheckPtr;
	/***Chech if the first char is the start flag***/
	if(*pbuffer_rx == EDG_NEXTION_FRAME_START_CHAR)
	{

		/*** Receive the other chars till the CR NL **/
		do
		{
			if(HAL_UART_Receive(&EDG_NEXTION_USART_PORT_HANDLE, pbuffer_rx, 1, EDG_NEXTION_RX_TIME_OUT_MS) != HAL_OK)
			{
				FlagError = 1;
				break;
			}
			else
			{
				CheckPtr = pbuffer_rx;
				pbuffer_rx++;
			}

		}while((*CheckPtr != 0x0D) && (FlagError == 0));

		pbuffer_rx--;
		*pbuffer_rx = 0;

		if(FlagError == 0)
		{
			if(HAL_UART_Receive(&EDG_NEXTION_USART_PORT_HANDLE, pbuffer_rx, 1, EDG_NEXTION_RX_TIME_OUT_MS) != HAL_OK)
			{
				return EDG_NEXTION_STATUS_ERROR;
			}
			else
			{
				if(*pbuffer_rx == 0x0A)
				{
					*pbuffer_rx = 0;
					ptrhedgNextion->RxFrameStatus = EDG_NEXTION_RX_FRAME_STATUS_RECEIVED;
					return EDG_NEXTION_STATUS_OK;
				}
				else
				{
					ptrhedgNextion->RxFrameStatus = EDG_NEXTION_RX_FRAME_STATUS_NOT_RECEIVED;
					return EDG_NEXTION_STATUS_ERROR;
				}
			}
		}
		else
		{
			ptrhedgNextion->RxFrameStatus = EDG_NEXTION_RX_FRAME_STATUS_NOT_RECEIVED;
			return EDG_NEXTION_STATUS_ERROR;
		}
	}
	else
	{
		ptrhedgNextion->RxFrameStatus = EDG_NEXTION_RX_FRAME_STATUS_NOT_RECEIVED;
		return EDG_NEXTION_STATUS_ERROR;
	}

}

/**
  * @brief
  * @param
  * @retval
  */
EDG_NEXTION_StatusTypeDef EDG_NEXTION_SendFrame(EDG_NEXTION_HandleTypeDef * ptrhedgNextion)
{

	strcat((char *)ptrhedgNextion->TxFrame, (const char *)ptrhedgNextion->Endflag);
	if(HAL_UART_Transmit(&EDG_NEXTION_USART_PORT_HANDLE, ptrhedgNextion->TxFrame, strlen((char *)ptrhedgNextion->TxFrame), EDG_NEXTION_TX_TIME_OUT_MS) == HAL_OK)
	{
		HAL_Delay(EDG_NEXTION_DELAY_MS);
		return EDG_NEXTION_STATUS_OK;
	}
	else
	{
		HAL_Delay(EDG_NEXTION_DELAY_MS);
		return EDG_NEXTION_STATUS_ERROR;
	}

}

/**
  * @brief
  * @param
  * @retval
  */
EDG_NEXTION_StatusTypeDef EDG_NEXTION_GetAllDataReceived(EDG_NEXTION_HandleTypeDef * ptrhedgNextion)
{

	static uint8_t FrameData[10];
	uint8_t * PtrFrameData = FrameData;
	uint8_t * PtrRxFrame = ptrhedgNextion->RxFrame;
	uint8_t * PtrCheckChar;

	ptrhedgNextion->DataReceivedLen = 0;

	do
	{
		if((*PtrRxFrame != ':') && (*PtrRxFrame != 0))
		{
			*PtrFrameData = *PtrRxFrame;
			PtrCheckChar = PtrRxFrame;
			PtrFrameData++;
			PtrRxFrame++;
		}
		else
		{
			*PtrFrameData = 0;
			ptrhedgNextion->DataReceived[ptrhedgNextion->DataReceivedLen] = EDG_NEXTION_Atoi((char *)FrameData);
			ptrhedgNextion->DataReceivedLen++;
			PtrCheckChar = PtrRxFrame;
			PtrRxFrame++;
			PtrFrameData = FrameData;
		}

	}while(*PtrCheckChar != 0);

	ptrhedgNextion->CommandReceived = (EDG_NEXTION_CommandTypeDef)ptrhedgNextion->DataReceived[EDG_NEXTION_POS_COMMAND];
	return EDG_NEXTION_STATUS_OK;
}

/**
  * @brief
  * @param
  * @retval
  */
EDG_NEXTION_PageTypeDef EDG_NEXTION_GetCurrentePage(EDG_NEXTION_HandleTypeDef * ptrhedgNextion)
{
	EDG_NEXTION_PageTypeDef Page;

	EDG_NEXTION_StopReceiveFrame();
	sprintf((char *)ptrhedgNextion->TxFrame, "sendme");

	if(EDG_NEXTION_SendFrame(ptrhedgNextion) == EDG_NEXTION_STATUS_OK)
	{

		if(HAL_UART_Receive(&EDG_NEXTION_USART_PORT_HANDLE, ptrhedgNextion->RxFrame, 5, EDG_NEXTION_TX_TIME_OUT_MS) == HAL_OK)
		{

			if(ptrhedgNextion->RxFrame[0] == 0x66)
			{
				Page = (EDG_NEXTION_PageTypeDef)ptrhedgNextion->RxFrame[1];
			}
			else
			{
				Page =  EDG_NEXTION_PAGE_LAST;
			}
		}
		else
		{
			Page =  EDG_NEXTION_PAGE_LAST;
		}
	}
	else
	{
		Page =  EDG_NEXTION_PAGE_LAST;
	}

	EDG_NEXTION_StartReceiveFrame(ptrhedgNextion);
	return Page;

}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_NEXTION_EnableTouch(EDG_NEXTION_HandleTypeDef * ptrhedgNextion)
{
	sprintf((char *)ptrhedgNextion->TxFrame, "tsw 255,1");
	EDG_NEXTION_SendFrame(ptrhedgNextion);
	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_NEXTION_DisableTouch(EDG_NEXTION_HandleTypeDef * ptrhedgNextion)
{
	sprintf((char *)ptrhedgNextion->TxFrame, "tsw 255,0");
	EDG_NEXTION_SendFrame(ptrhedgNextion);
	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_NEXTION_ChangePage(EDG_NEXTION_HandleTypeDef * ptrhedgNextion, EDG_NEXTION_PageTypeDef Page)
{
	/*** Check if it is a page in range ***/
	if(Page < EDG_NEXTION_PAGE_LAST)
	{

		/*** Send the command to change the page ***/
		sprintf((char *)ptrhedgNextion->TxFrame, "page %d", Page);
		if(EDG_NEXTION_SendFrame(ptrhedgNextion) == EDG_NEXTION_STATUS_OK)
		{
			ptrhedgNextion->CurrentPage = Page;
		}
	}
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_NEXTION_RestartBrightTim(EDG_NEXTION_HandleTypeDef * ptrhedgNextion)
{
	HAL_TIM_Base_Stop_IT(&EDG_NEXTION_TIM_BRIGHT_HANDLE);
	__HAL_TIM_SET_COUNTER(&EDG_NEXTION_TIM_BRIGHT_HANDLE, 0);
	HAL_TIM_Base_Start_IT(&EDG_NEXTION_TIM_BRIGHT_HANDLE);
	ptrhedgNextion->Bright.Status = EDG_NEXTION_BRIGHT_STATUS_NORMAL;

	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_NEXTION_StopBrightTim(void)
{
	HAL_TIM_Base_Stop_IT(&EDG_NEXTION_TIM_BRIGHT_HANDLE);
	__HAL_TIM_SET_COUNTER(&EDG_NEXTION_TIM_BRIGHT_HANDLE, 0);

	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_NEXTION_SetCurrentBright(EDG_NEXTION_HandleTypeDef * ptrhedgNextion)
{
	if(ptrhedgNextion->Bright.Status == EDG_NEXTION_BRIGHT_STATUS_CHANGED)
	{
		/*** Send the command to change the bright***/
		sprintf((char *)ptrhedgNextion->TxFrame, "dim=%d", ptrhedgNextion->Bright.Level);
		EDG_NEXTION_SendFrame(ptrhedgNextion);
		ptrhedgNextion->Bright.Status = EDG_NEXTION_BRIGHT_STATUS_NORMAL;
	}
	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_NEXTION_SetLowBright(EDG_NEXTION_HandleTypeDef * ptrhedgNextion)
{

	/*** Send the command to change the bright***/
	sprintf((char *)ptrhedgNextion->TxFrame, "dim=%d", EDG_NEXTION_BRIGHT_LEVEL_LOW);
	EDG_NEXTION_SendFrame(ptrhedgNextion);
	ptrhedgNextion->Bright.Status = EDG_NEXTION_BRIGHT_STATUS_CHANGED;
	EDG_NEXTION_StopBrightTim();
	return;

}

/**
  * @brief A simple atoi() function
  * @param
  * @retval
  */
uint32_t EDG_NEXTION_Atoi(char* str)
{
    // Initialize result
	uint32_t res = 0;

    // Iterate through all characters
    // of input string and update result
    // take ASCII character of corresponding digit and
    // subtract the code from '0' to get numerical
    // value and multiply res by 10 to shuffle
    // digits left to update running total
    for (int i = 0; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';

    // return result.
    return res;
}
