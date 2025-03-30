/*
 * EDG_UART_CALLBACKS.cpp
 *
 *  Created on: 08/04/2023
 *      Author: Elkin Granados
 */

/*** Include header file ***/
#include "EDG_UART_CALLBACKS.h"

/**
  * @brief  Rx Transfer completed callback.
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if(huart->Instance == EDG_NEXTION_USART_PORT)
	{
		hedgNextion.RxFrameStatus = EDG_NEXTION_RX_FRAME_STATUS_INCOMING_FRAME;
	}

	return;

}


