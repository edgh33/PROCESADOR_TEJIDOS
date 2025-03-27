/*
 * EDG_STATE_MACHINE.cpp
 *
 *  Created on: 06/04/2023
 *      Author: Elkin Granados
 */

/*** Include header file ***/
#include "EDG_STATE_MACHINE.h"


EDG_STATE_MACHINE_HandleTypeDef hedgStateMachine;

#if EDG_STATE_MACHINE_DEBUG_STATE == EDG_STATE_MACHINE_DEBUG_ACTIVE
uint8_t stringDebug[200] = {0};
uint8_t tempVal = 0;
#endif

/* State Machine Functions -------------------------------------*/

/**
  * @brief
  * @param  Ninguno
  * @retval Ninguno
  */
void EDG_STATE_MACHINE_Init(EDG_STATE_MACHINE_HandleTypeDef * ptrhedgStateMachine)
{
	ptrhedgStateMachine->CurrentState = EDG_STATE_MACHINE_STATE_INIT;
	ptrhedgStateMachine->PastState = EDG_STATE_MACHINE_STATE_INIT;
}

/**
  * @brief
  * @param  Ninguno
  * @retval Ninguno
  */
void EDG_STATE_MACHINE_SetNextState(EDG_STATE_MACHINE_HandleTypeDef * ptrhedgStateMachine,
									EDG_STATE_MACHINE_StateTypeDef NextState)
{
	ptrhedgStateMachine->PastState = ptrhedgStateMachine->CurrentState;
	ptrhedgStateMachine->CurrentState = NextState;
}

/**
  * @brief
  * @param  Ninguno
  * @retval Ninguno
  */
void EDG_STATE_MACHINE_Admin(EDG_STATE_MACHINE_HandleTypeDef * ptrhedgStateMachine)
{

	switch(ptrhedgStateMachine->CurrentState)
	{

		case EDG_STATE_MACHINE_STATE_INIT:

			EDG_STATE_MACHINE_InitState();

			break;

		case EDG_STATE_MACHINE_STATE_IDLE:

			EDG_STATE_MACHINE_IdleState();

			break;

		case EDG_STATE_MACHINE_STATE_EXEC_COMMAND:

			EDG_STATE_MACHINE_ExecCommandState();

			break;

		case EDG_STATE_MACHINE_STATE_TEMPERATURE_CONTROL:

			EDG_STATE_MACHINE_TemperatureControlState();

			break;

		case EDG_STATE_MACHINE_STATE_PROCESS:


			break;

		case EDG_STATE_MACHINE_STATE_MANUAL:

			EDG_STATE_MACHINE_Manual();

			break;

		case EDG_STATE_MACHINE_STATE_ALARM:

			EDG_STATE_MACHINE_AlarmState();
			break;

		case EDG_STATE_MACHINE_STATE_TEST:

			EDG_STATE_MACHINE_TestState();
			break;

		default:

			ptrhedgStateMachine->CurrentState = EDG_STATE_MACHINE_STATE_IDLE;
			ptrhedgStateMachine->PastState = EDG_STATE_MACHINE_STATE_IDLE;

			break;
	}
}

/**
  * @brief
  * @param  Ninguno
  * @retval Ninguno
  */
void EDG_STATE_MACHINE_InitState(void)
{

	/*** Keep the AC relay open while the init ***/
	EDG_AC_CONTROL_OpenAcRelay();

	EDG_WS2812_Init(&hedgWs2812);
	EDG_WS2812_PixelAll(&hedgWs2812, 0, 0, 0);
	EDG_WS2812_SendSpi(&hedgWs2812);
	EDG_NEXTION_Init(&hedgNextion);
	EDG_STATE_MACHINE_CheckRTC();
	EDG_BUZZER_Init(&hedgBuzzer);
	EDG_BUZZER_Pulse(&hedgBuzzer);

	HAL_Delay(2000);

	if(EDG_NEXTION_GetCurrentePage(&hedgNextion) != EDG_NEXTION_PAGE_START)
	{
		EDG_NEXTION_ChangePage(&hedgNextion, EDG_NEXTION_PAGE_START);
	}
	EDG_STATE_MACHINE_ShowDate();

	sprintf((char *)hedgNextion.TxFrame, "START.version.txt=\"%s\"", EDG_CONFIG_VERSION_FIRMWARE);
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame, "t99.txt=\"%s\"", EDG_CONFIG_VERSION_FIRMWARE);
	EDG_NEXTION_SendFrame(&hedgNextion);

	sprintf((char *)hedgNextion.TxFrame, "timBarra.en=0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	HAL_Delay(20);

	if(EDG_MEMORY_InitMemory(EDG_MEMORY_ADDRESS_MEM1) == EDG_MEMORY_STATE_OK)
	{
#if EDG_STATE_MACHINE_DEBUG_STATE == EDG_STATE_MACHINE_DEBUG_ACTIVE
		sprintf((char *)stringDebug, "memory initialized successfully!!!\r\n");
		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
#endif
		EDG_STATE_MACHINE_CheckMemory();
	}
	else
	{
#if EDG_STATE_MACHINE_DEBUG_STATE == EDG_STATE_MACHINE_DEBUG_ACTIVE
		sprintf((char *)stringDebug, "Error initializing memory...\r\n");
		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
#endif
	}

	sprintf((char *)hedgNextion.TxFrame, "j0.val=50");
	EDG_NEXTION_SendFrame(&hedgNextion);
	HAL_Delay(1000);

	EDG_TIMER_Init(&hedgTimer);
	EDG_MAX6675_Init(&hedgMAX6675);

	sprintf((char *)hedgNextion.TxFrame, "j0.val=75");
	EDG_NEXTION_SendFrame(&hedgNextion);
	HAL_Delay(1000);

	EDG_AC_CONTROL_Init(&hedgAccontrol, &hedgMAX6675);
	/*** Set the leds in the right color ***/
	EDG_STATE_MACHINE_SetLedColors();

	if(hedgAccontrol.AlarmStatus == EDG_AC_CONTROL_ALARM_STATUS_INACTIVE)
	{

		EDG_SCHEDULE_Init(&hedgSchedule, &hedgRTC);
		EDG_SCHEDULE_CheckActive(&hedgSchedule, &hedgRTC, &hedgAccontrol, &hedgNextion);
		EDG_SCHEDULE_CheckInactive(&hedgSchedule, &hedgRTC, &hedgAccontrol, &hedgNextion);

		sprintf((char *)hedgNextion.TxFrame, "j0.val=100");
		EDG_NEXTION_SendFrame(&hedgNextion);
		HAL_Delay(1000);
		sprintf((char *)hedgNextion.TxFrame, "vis j0,0");
		EDG_NEXTION_SendFrame(&hedgNextion);
		HAL_Delay(20);
		sprintf((char *)hedgNextion.TxFrame, "vis b0,1");
		EDG_NEXTION_SendFrame(&hedgNextion);
		HAL_Delay(20);

		EDG_NEXTION_EnableTouch(&hedgNextion);
		EDG_NEXTION_StartReceiveFrame(&hedgNextion);

		EDG_TIMER_StarBaseTime(&hedgTimer, &hedgRTC);
		/*** If all is right change to idle state ***/
		if(EDG_STATE_MACHINE_TEST_STATE == EDG_STATE_MACHINE_TEST_INACTIVE)
		{
			EDG_STATE_MACHINE_SetNextState(&hedgStateMachine, EDG_STATE_MACHINE_STATE_IDLE);
		}
		else
		{
			EDG_STATE_MACHINE_SetNextState(&hedgStateMachine, EDG_STATE_MACHINE_STATE_TEST);
		}
	}
	else
	{
		EDG_NEXTION_EnableTouch(&hedgNextion);
		EDG_NEXTION_StartReceiveFrame(&hedgNextion);
		/*** If there is a unit in alarm change to alarm state ***/
		if(EDG_STATE_MACHINE_TEST_STATE == EDG_STATE_MACHINE_TEST_INACTIVE)
		{
			EDG_STATE_MACHINE_SetNextState(&hedgStateMachine, EDG_STATE_MACHINE_STATE_ALARM);
		}
		else
		{
			EDG_STATE_MACHINE_SetNextState(&hedgStateMachine, EDG_STATE_MACHINE_STATE_TEST);
		}
	}

	return;

}


/**
  * @brief
  * @param  Ninguno
  * @retval Ninguno
  */
void EDG_STATE_MACHINE_IdleState(void)
{

//	HAL_GPIO_TogglePin(RELE1_GPIO_Port, RELE1_Pin);
//	HAL_GPIO_TogglePin(RELE2_GPIO_Port, RELE2_Pin);
//	HAL_GPIO_TogglePin(RELE_AC_GPIO_Port, RELE_AC_Pin);

//	if(hedgTimer.FlagsStatus.Flag1m == true)
//	{
//		hedgTimer.FlagsStatus.Flag1m = false;
//
//		EDG_RTC_GetDate(EDG_RTC_ADDRESS, &hedgRTC);
//#if EDG_STATE_MACHINE_DEBUG_STATE == EDG_STATE_MACHINE_DEBUG_ACTIVE
//		sprintf((char *)stringDebug, "%.2d-%.2d-%.2d / %.2d:%.2d:%.2d\r\n", hedgRTC.CurrentDate.Day, hedgRTC.CurrentDate.Month,
//																		 hedgRTC.CurrentDate.Year, hedgRTC.CurrentDate.Hour,
//																		 hedgRTC.CurrentDate.Minute, hedgRTC.CurrentDate.Second);
//		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
//#endif
//	}

//	/*** Function for check the working of the outputs ***/
//	EDG_STATE_MACHINE_TestPwmOutput();
//	/*** Function that shows the current temperature value ***/
//	EDG_STATE_MACHINE_TestTempetatureChips();

/* MAIN LOOP START ----------------------------------------------------------------------------------------*/

	if(hedgNextion.RxFrameStatus == EDG_NEXTION_RX_FRAME_STATUS_INCOMING_FRAME)
	{
		EDG_NEXTION_ReceiveFrame(&hedgNextion);
		if(hedgNextion.RxFrameStatus == EDG_NEXTION_RX_FRAME_STATUS_RECEIVED)
		{
			EDG_STATE_MACHINE_SetNextState(&hedgStateMachine, EDG_STATE_MACHINE_STATE_EXEC_COMMAND);
			return;
		}
		else
		{
			EDG_NEXTION_StartReceiveFrame(&hedgNextion);
		}

	}

	if(hedgTimer.FlagsStatus.FlagBaseTimeSecs1 == true)
	{
		hedgTimer.FlagsStatus.FlagBaseTimeSecs1 = false;
		if(hedgNextion.Bright.Status == EDG_NEXTION_BRIGHT_STATUS_LOW)
		{
			EDG_NEXTION_SetLowBright(&hedgNextion);
		}
		EDG_STATE_MACHINE_SetNextState(&hedgStateMachine, EDG_STATE_MACHINE_STATE_TEMPERATURE_CONTROL);
		/*** Restart the Nextion data receiving **/
		EDG_NEXTION_StartReceiveFrame(&hedgNextion);
		EDG_NEXTION_EnableTouch(&hedgNextion);
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		return;
	}

	if(hedgTimer.FlagsStatus.Flag1m == true)
	{
		hedgTimer.FlagsStatus.Flag1m = false;
		EDG_STATE_MACHINE_ShowDate();
		EDG_SCHEDULE_CheckChangeDay(&hedgSchedule, &hedgRTC);
		EDG_SCHEDULE_CheckActive(&hedgSchedule, &hedgRTC, &hedgAccontrol, &hedgNextion);
		EDG_SCHEDULE_CheckInactive(&hedgSchedule, &hedgRTC, &hedgAccontrol, &hedgNextion);
	}
	if(hedgTimer.FlagsStatus.Flag1h == true)
	{
		hedgTimer.FlagsStatus.Flag1h = false;

	}

/* MAIN LOOP END ----------------------------------------------------------------------------------------*/

	return;

}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_TemperatureControlState(void)
{

		/*** Perform Control of all units ***/
		EDG_AC_CONTROL_PerformAll(&hedgAccontrol, &hedgMAX6675);

#if EDG_STATE_MACHINE_DEBUG_STATE == EDG_STATE_MACHINE_DEBUG_ACTIVE
		/*** Function that shows the current temperature value ***/
		EDG_STATE_MACHINE_TestTempetatureChips();
#endif
		/*** Change to the respective state  ***/
		EDG_AC_CONTROL_CheckAlarm(&hedgAccontrol);
		if(hedgAccontrol.AlarmStatus == EDG_AC_CONTROL_ALARM_STATUS_INACTIVE)
		{
			if(EDG_AC_CONTROL_GetRelayStatus() == EDG_AC_CONTROL_RELAY_STATUS_OPEN)
			{
				/*** Come back from alarm ***/
				EDG_AC_CONTROL_CloseAcRelay();
				EDG_NEXTION_ChangePage(&hedgNextion, EDG_NEXTION_PAGE_EXECUTE);
				EDG_STATE_MACHINE_SetConfigPage();
				EDG_BUZZER_SetMode(&hedgBuzzer, EDG_BUZZER_MODE_ONE_PULSE);

			}
			else
			{
				if(hedgNextion.CurrentPage == EDG_NEXTION_PAGE_EXECUTE)
				{
					EDG_STATE_MACHINE_RefreshConfigPage();
				}
			}
			EDG_STATE_MACHINE_SetNextState(&hedgStateMachine, EDG_STATE_MACHINE_STATE_IDLE);
		}
		else
		{
			if(EDG_AC_CONTROL_GetRelayStatus() == EDG_AC_CONTROL_RELAY_STATUS_CLOSE)
			{
				EDG_STATE_MACHINE_SetNextState(&hedgStateMachine, EDG_STATE_MACHINE_STATE_ALARM);
			}
			else
			{
				EDG_STATE_MACHINE_SetNextState(&hedgStateMachine, EDG_STATE_MACHINE_STATE_IDLE);
			}
		}
		/*** Set the leds in the right color ***/
		EDG_STATE_MACHINE_SetLedColors();
		return;

}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_ExecCommandState(void)
{
	EDG_STATE_MACHINE_StateTypeDef NextState = EDG_STATE_MACHINE_STATE_IDLE;
	EDG_BUZZER_Pulse(&hedgBuzzer);
	EDG_NEXTION_GetAllDataReceived(&hedgNextion);
	HAL_Delay(50);

	switch(hedgNextion.CommandReceived)
	{
		case EDG_NEXTION_COMMAND_CHANGE_PAGE:

			EDG_STATE_MACHINE_ChangePage();
			EDG_STATE_MACHINE_ShowDate();

			break;

		case EDG_NEXTION_COMMAND_LOAD_PROGRAM_VALUES:

			//EDG_STATE_MACHINE_ChangeTemperature();

			break;

		case EDG_NEXTION_COMMAND_SET_DATE:

			EDG_STATE_MACHINE_SetDate();
			EDG_STATE_MACHINE_ShowDate();

			break;

		case EDG_NEXTION_COMMAND_PAUSE_PROCESS:

			break;

		case EDG_NEXTION_COMMAND_STOP_PROCESS:

			break;

		case EDG_NEXTION_COMMAND_SET_TEMPERATURE:

			//EDG_STATE_MACHINE_LoadProgramValues(hedgNextion.DataReceived[EDG_NEXTION_POS_PAGE]);

			break;

		case EDG_NEXTION_COMMAND_RUN_PROCESS:

			//EDG_STATE_MACHINE_SaveProgramValues(hedgNextion.DataReceived[EDG_NEXTION_POS_PAGE]);

			break;

		case EDG_NEXTION_COMMAND_SAVE_SCHEDULE:

			/*
			EDG_STATE_MACHINE_SaveScheduleValues(hedgNextion.DataReceived[EDG_NEXTION_POS_PAGE]);
			EDG_STATE_MACHINE_LoadScheduleValues();
			EDG_SCHEDULE_GetScheduleToday(&hedgSchedule, &hedgRTC);
			EDG_SCHEDULE_CheckActive(&hedgSchedule, &hedgRTC, &hedgAccontrol, &hedgNextion);
			EDG_SCHEDULE_CheckInactive(&hedgSchedule, &hedgRTC, &hedgAccontrol, &hedgNextion);
			 */
			break;

		case EDG_NEXTION_COMMAND_RESET:

			break;

		case EDG_NEXTION_COMMAND_BUZZER:

			break;

		case EDG_NEXTION_COMMAND_BUZZER_CONFIG:

			EDG_STATE_MACHINE_BuzzerConfig();

			break;

		case EDG_NEXTION_COMMAND_BRIGHT:

			EDG_STATE_MACHINE_BrightSave();

			break;

		case EDG_NEXTION_COMMAND_OFFSET:

			EDG_STATE_MACHINE_SaveOffsetValues();
			EDG_AC_CONTROL_LoadOffsetValues(&hedgAccontrol);

			break;

		case EDG_NEXTION_COMMAND_MANUAL_MODE:

			tempVal = hedgNextion.DataReceived[1];
			NextState = EDG_STATE_MACHINE_STATE_MANUAL;
			break;

		case EDG_NEXTION_COMMAND_SAVE_PROGRAM:

			break;

		default:

			break;

	}

	EDG_NEXTION_SetCurrentBright(&hedgNextion);
	EDG_NEXTION_RestartBrightTim(&hedgNextion);
	EDG_NEXTION_StartReceiveFrame(&hedgNextion);
	if(NextState == EDG_STATE_MACHINE_STATE_IDLE)
	{
		EDG_NEXTION_EnableTouch(&hedgNextion);
	}
	HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
	/*** Change to the respective state  ***/
	EDG_STATE_MACHINE_SetNextState(&hedgStateMachine, NextState);
	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_AlarmState(void)
{

	if(EDG_NEXTION_GetCurrentePage(&hedgNextion) != EDG_NEXTION_PAGE_ALARM)
	{
		EDG_NEXTION_ChangePage(&hedgNextion, EDG_NEXTION_PAGE_ALARM);
	}

	EDG_BUZZER_SetMode(&hedgBuzzer, EDG_BUZZER_MODE_REPETITIVE);
	EDG_BUZZER_Pulse(&hedgBuzzer);
	EDG_AC_CONTROL_OpenAcRelay();
	EDG_STATE_MACHINE_SetNextState(&hedgStateMachine, EDG_STATE_MACHINE_STATE_IDLE);
	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_TestState(void)
{

	EDG_WS2812_Pixel(&hedgWs2812, 0, 0, 0, 0);
	EDG_WS2812_Pixel(&hedgWs2812, 1, 0, 0, 0);
	EDG_WS2812_Pixel(&hedgWs2812, 2, 0, 0, 0);
	EDG_WS2812_Pixel(&hedgWs2812, 3, 0, 0, 0);
	EDG_WS2812_Pixel(&hedgWs2812, 4, 0, 0, 0);
	EDG_WS2812_Pixel(&hedgWs2812, 5, 0, 0, 0);

	EDG_WS2812_SendSpi(&hedgWs2812);


	// LOGICA DEL CICLO!!!!!!
	//Inicia siempre con los reles inactivos
	EDG_RELE_AGITAR_INACTIVO();
	EDG_RELE_ELEVAR_INACTIVO();

	//Se debe buscar siempre inicialmente tener el equipo con
	//el carrusel abajo para poder hacer la logica del ciclo

	//Si el carrusel esta abajo se puede comenzar a agitar
	if(EDG_SENSOR_POSICION())
	{
		EDG_RELE_AGITAR_ACTIVO();
		EDG_WS2812_Pixel(&hedgWs2812, 0, 0, 50, 0);
		EDG_WS2812_SendSpi(&hedgWs2812);
#if EDG_STATE_MACHINE_DEBUG_STATE == EDG_STATE_MACHINE_DEBUG_ACTIVE
		EDG_RTC_GetDate(&hedgRTC, EDG_RTC_ADDRESS);
		sprintf((char *)stringDebug, "[%02d:%02d%02d-%02d:%02d%02d]Procesador en posicion de agitar, iniciando ciclo...\r\n", hedgRTC.CurrentDate.Hour,
																															  hedgRTC.CurrentDate.Minute,
																															  hedgRTC.CurrentDate.Second,
																															  hedgRTC.CurrentDate.Day,
																															  hedgRTC.CurrentDate.Month,
																															  hedgRTC.CurrentDate.Year);
		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
#endif

	}
	//Si el carrusel no esta abajo se activa el rele de posicion hasta
	//que se detecte que esta abajo
	else
	{

		EDG_RELE_ELEVAR_ACTIVO();
		EDG_WS2812_Pixel(&hedgWs2812, 0, 50, 0, 50);
		EDG_WS2812_SendSpi(&hedgWs2812);
#if EDG_STATE_MACHINE_DEBUG_STATE == EDG_STATE_MACHINE_DEBUG_ACTIVE
		EDG_RTC_GetDate(&hedgRTC, EDG_RTC_ADDRESS);
		sprintf((char *)stringDebug, "[%02d:%02d:%02d-%02d/%02d/%02d]Buscando posicion de agitar...\r\n", hedgRTC.CurrentDate.Hour,
																															  hedgRTC.CurrentDate.Minute,
																															  hedgRTC.CurrentDate.Second,
																															  hedgRTC.CurrentDate.Day,
																															  hedgRTC.CurrentDate.Month,
																															  hedgRTC.CurrentDate.Year);
		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
#endif
		while(!EDG_SENSOR_POSICION());
		//Cuando se detecta que esta abajo se comienza a agitar
		EDG_RELE_ELEVAR_INACTIVO();
		EDG_RELE_AGITAR_ACTIVO();
		HAL_Delay(200);
		EDG_WS2812_Pixel(&hedgWs2812, 0, 0, 50, 0);
		EDG_WS2812_SendSpi(&hedgWs2812);
#if EDG_STATE_MACHINE_DEBUG_STATE == EDG_STATE_MACHINE_DEBUG_ACTIVE
		EDG_RTC_GetDate(&hedgRTC, EDG_RTC_ADDRESS);
		sprintf((char *)stringDebug, "[%02d:%02d:%02d-%02d/%02d/%02d]Posicion de agitar encontrada, iniciando ciclo...\r\n", hedgRTC.CurrentDate.Hour,
																														  hedgRTC.CurrentDate.Minute,
																														  hedgRTC.CurrentDate.Second,
																														  hedgRTC.CurrentDate.Day,
																														  hedgRTC.CurrentDate.Month,
																														  hedgRTC.CurrentDate.Year);
		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
#endif

	}

	//Se espera el tiempo de agitacion programado por el ciclo
	HAL_Delay(10000);

	while(1)
	{

		//Cuando se cumple el tiempo se hace el proceso de elevar el carrusel
		//deteniendo el agitar y activando el rele de posicion
		EDG_RELE_AGITAR_INACTIVO();
		EDG_RELE_ELEVAR_ACTIVO();
		HAL_Delay(500); //Se coloca delay como antirrebote pero es mejor mirar otra opcion
		EDG_WS2812_Pixel(&hedgWs2812, 0, 50, 50, 0);
		EDG_WS2812_SendSpi(&hedgWs2812);
#if EDG_STATE_MACHINE_DEBUG_STATE == EDG_STATE_MACHINE_DEBUG_ACTIVE
		EDG_RTC_GetDate(&hedgRTC, EDG_RTC_ADDRESS);
		sprintf((char *)stringDebug, "[%02d:%02d:%02d-%02d/%02d/%02d]Elevando Carrusel...\r\n", hedgRTC.CurrentDate.Hour,
																							  hedgRTC.CurrentDate.Minute,
																							  hedgRTC.CurrentDate.Second,
																							  hedgRTC.CurrentDate.Day,
																							  hedgRTC.CurrentDate.Month,
																							  hedgRTC.CurrentDate.Year);
		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
#endif
		//Se espera un proceso de no sensado de carrusel y uno de sensado para
		//saber que el carrusel esta arriba
		while(!EDG_SENSOR_CARRUSEL());
		HAL_Delay(500); //Se coloca delay como antirrebote pero es mejor mirar otra opcion
#if EDG_STATE_MACHINE_DEBUG_STATE == EDG_STATE_MACHINE_DEBUG_ACTIVE
		EDG_RTC_GetDate(&hedgRTC, EDG_RTC_ADDRESS);
		sprintf((char *)stringDebug, "[%02d:%02d:%02d-%02d/%02d/%02d]Aun Elevando Carrusel...\r\n", hedgRTC.CurrentDate.Hour,
																							  hedgRTC.CurrentDate.Minute,
																							  hedgRTC.CurrentDate.Second,
																							  hedgRTC.CurrentDate.Day,
																							  hedgRTC.CurrentDate.Month,
																							  hedgRTC.CurrentDate.Year);
		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
#endif
		while(EDG_SENSOR_CARRUSEL());
		//Cuando el carrusel esta arriba se inactiva el rele de elevar
		EDG_RELE_ELEVAR_INACTIVO();
		HAL_Delay(500); //Se coloca delay como antirrebote pero es mejor mirar otra opcion
		EDG_WS2812_Pixel(&hedgWs2812, 0, 50, 0, 0);
		EDG_WS2812_SendSpi(&hedgWs2812);
#if EDG_STATE_MACHINE_DEBUG_STATE == EDG_STATE_MACHINE_DEBUG_ACTIVE
		EDG_RTC_GetDate(&hedgRTC, EDG_RTC_ADDRESS);
		sprintf((char *)stringDebug, "[%02d:%02d:%02d-%02d/%02d/%02d]Carrusel arriba, esperando escurrido...\r\n", hedgRTC.CurrentDate.Hour,
																												  hedgRTC.CurrentDate.Minute,
																												  hedgRTC.CurrentDate.Second,
																												  hedgRTC.CurrentDate.Day,
																												  hedgRTC.CurrentDate.Month,
																												  hedgRTC.CurrentDate.Year);
		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
#endif

		//Espera de tiempo con el carrusel arriba para escurrido de la muestra
		HAL_Delay(5000);

		//Se vuelve a activar el rele de elevar para que baje el carrusel
		EDG_RELE_ELEVAR_ACTIVO();
		HAL_Delay(500); //Se coloca delay como antirrebote pero es mejor mirar otra opcion
		EDG_WS2812_Pixel(&hedgWs2812, 0, 50, 0, 50);
		EDG_WS2812_SendSpi(&hedgWs2812);
#if EDG_STATE_MACHINE_DEBUG_STATE == EDG_STATE_MACHINE_DEBUG_ACTIVE
		EDG_RTC_GetDate(&hedgRTC, EDG_RTC_ADDRESS);
		sprintf((char *)stringDebug, "[%02d:%02d:%02d-%02d/%02d/%02d]Bajando Carrusel...\r\n", hedgRTC.CurrentDate.Hour,
																							  hedgRTC.CurrentDate.Minute,
																							  hedgRTC.CurrentDate.Second,
																							  hedgRTC.CurrentDate.Day,
																							  hedgRTC.CurrentDate.Month,
																							  hedgRTC.CurrentDate.Year);
		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
#endif
		//Se espera hasta que se detecte el sensor de posicion
		while(!EDG_SENSOR_POSICION());
		//Cuando se detecta el sensor se sabe que el carrusel esta abajo y se inicia de nuevo a agitar
		EDG_RELE_ELEVAR_INACTIVO();
		EDG_RELE_AGITAR_ACTIVO();
		HAL_Delay(500); //Se coloca delay como antirrebote pero es mejor mirar otra opcion
		EDG_WS2812_Pixel(&hedgWs2812, 0, 0, 50, 0);
		EDG_WS2812_SendSpi(&hedgWs2812);
#if EDG_STATE_MACHINE_DEBUG_STATE == EDG_STATE_MACHINE_DEBUG_ACTIVE
		EDG_RTC_GetDate(&hedgRTC, EDG_RTC_ADDRESS);
		sprintf((char *)stringDebug, "[%02d:%02d:%02d-%02d/%02d/%02d]Posicion de agitar...\r\n", hedgRTC.CurrentDate.Hour,
																							  hedgRTC.CurrentDate.Minute,
																							  hedgRTC.CurrentDate.Second,
																							  hedgRTC.CurrentDate.Day,
																							  hedgRTC.CurrentDate.Month,
																							  hedgRTC.CurrentDate.Year);
		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
#endif
		//Se espera el tiempo de agitacion programado por el ciclo
		HAL_Delay(10000);

		/*
		HAL_GPIO_WritePin(RELE_MANUAL_GPIO_Port, RELE_MANUAL_Pin, GPIO_PIN_SET);

		if(!HAL_GPIO_ReadPin(SENSOR_AC_GPIO_Port, SENSOR_AC_Pin))
		{
			sprintf((char *)stringDebug, "Sensor AC activo!!!\r\n");
		}
		else
		{
			sprintf((char *)stringDebug, "Sensor AC inactivo...\r\n");
		}
		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);

		if(!HAL_GPIO_ReadPin(SENSOR_FIN_GPIO_Port, SENSOR_FIN_Pin))
		{
			sprintf((char *)stringDebug, "Sensor Fin activo!!!\r\n");
		}
		else
		{
			sprintf((char *)stringDebug, "Sensor Fin inactivo...\r\n");
		}
		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);

		HAL_GPIO_TogglePin(RELE_MANUAL_GPIO_Port, RELE_MANUAL_Pin);
		HAL_GPIO_TogglePin(RELE_START_GPIO_Port, RELE_START_Pin);

		HAL_Delay(2000);
		*/

	}

	return;
}

/* Auxiliary Functions -------------------------------------*/

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_SetLedColors(void)
{

	for(hedgAccontrol.UnitsCounter = 0; hedgAccontrol.UnitsCounter < hedgAccontrol.UnitsQty; hedgAccontrol.UnitsCounter++)
	{
		switch(hedgAccontrol.Units[hedgAccontrol.UnitsCounter].SensorStatus)
		{
			case EDG_AC_CONTROL_SENSOR_STATUS_OK:

				if(hedgAccontrol.Units[hedgAccontrol.UnitsCounter].ValueStatus == EDG_AC_CONTROL_VALUE_STATUS_OUT_OF_RANGE)
				{
					/*** If value is out of range the sensor is out of range so the led is RED ***/
					EDG_WS2812_Pixel(&hedgWs2812, (hedgAccontrol.UnitsQty - hedgAccontrol.UnitsCounter - 1) , 50, 0, 0);
				}
				else
				{
					/*** If value is OK the sensor is normal so the led is GREEN ***/
					EDG_WS2812_Pixel(&hedgWs2812, (hedgAccontrol.UnitsQty - hedgAccontrol.UnitsCounter - 1) , 0, 50, 0);
				}

				break;

			case EDG_AC_CONTROL_SENSOR_STATUS_NOT_PRESENT:

				/*** If sensor is not present the led is BLUE ***/
				EDG_WS2812_Pixel(&hedgWs2812, (hedgAccontrol.UnitsQty - hedgAccontrol.UnitsCounter - 1) , 0, 0, 50);

				break;

			case EDG_AC_CONTROL_SENSOR_STATUS_ERROR:

				/*** If sensor is in error led is YELLOW ***/
				EDG_WS2812_Pixel(&hedgWs2812, (hedgAccontrol.UnitsQty - hedgAccontrol.UnitsCounter - 1) , 0, 50, 50);

				break;

			default:

				break;

		}
	}

	EDG_WS2812_SendSpi(&hedgWs2812);

	return;

}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_CheckRTC(void)
{

	/*** Init the RTC ***/
	EDG_RTC_Init(&hedgRTC, EDG_RTC_ADDRESS);
	/*** Check if the hour format is right ***/
	if(hedgRTC.CurrentDate.Format == EDG_RTC_HOUR_FORMAT_24H)
	{
		hedgRTC.DatetoChange.Format = EDG_RTC_HOUR_FORMAT_12H;
		EDG_RTC_SetHourFormat(&hedgRTC, EDG_RTC_ADDRESS);
		EDG_RTC_GetDate(&hedgRTC, EDG_RTC_ADDRESS);
	}

	return;

}

/**
  * @brief
  * @param  Ninguno
  * @retval Ninguno
  */
void EDG_STATE_MACHINE_TestPwmOutput(void)
{

	static float CountePWM = 0.0;
	static uint16_t pwmValue = 0;

	pwmValue = EDG_AC_CONTROL_PWMEcuation(CountePWM);

#if EDG_STATE_MACHINE_DEBUG_STATE == EDG_STATE_MACHINE_DEBUG_ACTIVE
	sprintf((char *)stringDebug, "PWM=%.4d - Counter=%01.f\r\n", pwmValue, CountePWM);
	HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
#endif

	EDG_AC_CONTROL_SetPWMOutput(&hedgAccontrol, 0, pwmValue);
	EDG_AC_CONTROL_SetPWMOutput(&hedgAccontrol, 1, pwmValue);
	EDG_AC_CONTROL_SetPWMOutput(&hedgAccontrol, 2, pwmValue);
	EDG_AC_CONTROL_SetPWMOutput(&hedgAccontrol, 3, pwmValue);
	EDG_AC_CONTROL_SetPWMOutput(&hedgAccontrol, 4, pwmValue);
	EDG_AC_CONTROL_SetPWMOutput(&hedgAccontrol, 5, pwmValue);

	CountePWM += 0.5;

	if(CountePWM > 100)
	{
		CountePWM = 0.0;
	}

	HAL_Delay(100);

	return;

}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_TestTempetatureChips(void)
{

	uint8_t Counter;

	EDG_MAX6675_ReadAllChips(&hedgMAX6675);

#if EDG_STATE_MACHINE_DEBUG_STATE == EDG_STATE_MACHINE_DEBUG_ACTIVE
	for(Counter = 0; Counter < hedgMAX6675.NumChipsEnabled; Counter++)
	{
		sprintf((char *)stringDebug, "%3.02f,", hedgMAX6675.Chip[Counter].Temperature);
		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
	}
	for(Counter = 0; Counter < hedgAccontrol.UnitsQty; Counter++)
	{
		sprintf((char *)stringDebug, "%3.02f,", hedgAccontrol.Units[Counter].Pid.SetPoint);
		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
	}

	sprintf((char *)stringDebug, "\r\n");
	HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
#endif
	return;

}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_ChangePage(void)
{

	EDG_NEXTION_ChangePage(&hedgNextion, (EDG_NEXTION_PageTypeDef)hedgNextion.DataReceived[EDG_NEXTION_POS_PAGE]);
	if(hedgNextion.CurrentPage == EDG_NEXTION_PAGE_EXECUTE)
	{
		EDG_STATE_MACHINE_SetConfigPage();
	}
	else if(hedgNextion.CurrentPage == EDG_NEXTION_PAGE_PROGRAM)
	{
		EDG_STATE_MACHINE_LoadProgramValues(0);
	}
	else if(hedgNextion.CurrentPage == EDG_NEXTION_PAGE_SCHEDULE)
	{
		EDG_STATE_MACHINE_LoadScheduleValues();
		EDG_STATE_MACHINE_CheckButtonSchedule();
	}
	else if(hedgNextion.CurrentPage == EDG_NEXTION_PAGE_MENU)
	{
		if((hedgRTC.CurrentState != EDG_RTC_STATE_OK) && (hedgAccontrol.SensorsInAlarm != 0))
		{
			sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"Error en el reloj y en uno   o varios sensores...\"");
		}
		else if(hedgRTC.CurrentState != EDG_RTC_STATE_OK)
		{
			sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"Error en el reloj...\"");

		}
		else if((hedgRTC.DateState == EDG_RTC_DATE_STATE_UNCONFIGURED) && (hedgAccontrol.SensorsInAlarm != 0))
		{
			sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"  Reloj desconfigurado y     error en uno o varios          sensores...\"");
		}
		else if(hedgRTC.DateState == EDG_RTC_DATE_STATE_UNCONFIGURED)
		{
			sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"  Reloj desconfigurado...\"");
		}
		else if(hedgAccontrol.SensorsInAlarm != 0)
		{
			sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"   Error en uno o varios          sensores...\"");
		}
		else
		{
			sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"        Central en           funcionamiento normal\"");
		}

		EDG_NEXTION_SendFrame(&hedgNextion);
		EDG_STATE_MACHINE_CheckButtonBuzzer();
	}
	else if(hedgNextion.CurrentPage == EDG_NEXTION_PAGE_OFFSET)
	{
		EDG_STATE_MACHINE_LoadOffsetValues();
	}

}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_SetDate(void)
{

	hedgRTC.DatetoChange.Second = 0;
	hedgRTC.DatetoChange.Minute = (uint8_t)hedgNextion.DataReceived[EDG_NEXTION_RTC_POS_MINUTE];
	hedgRTC.DatetoChange.Hour = (uint8_t)hedgNextion.DataReceived[EDG_NEXTION_RTC_POS_HOUR];
	hedgRTC.DatetoChange.Day = (uint8_t)hedgNextion.DataReceived[EDG_NEXTION_RTC_POS_DAY];
	hedgRTC.DatetoChange.WeekDay = (uint8_t)hedgNextion.DataReceived[EDG_NEXTION_RTC_POS_WEEKDAY];
	hedgRTC.DatetoChange.Month = (uint8_t)hedgNextion.DataReceived[EDG_NEXTION_RTC_POS_MONTH];
	hedgRTC.DatetoChange.Year = (uint8_t)hedgNextion.DataReceived[EDG_NEXTION_RTC_POS_YEAR];
	hedgRTC.DatetoChange.AmPm = (EDG_RTC_AmPmTypeDef)(uint8_t)hedgNextion.DataReceived[EDG_NEXTION_RTC_POS_AMPM];

	if(EDG_RTC_SetDate(&hedgRTC, EDG_RTC_ADDRESS) == EDG_RTC_STATE_OK)
	{
		sprintf((char *)hedgNextion.TxFrame,"t5.txt=\"Hora guardada correctamente!!!\"");
	}
	else
	{
		sprintf((char *)hedgNextion.TxFrame,"t5.txt=\"Error al guardar hora...\"");
	}
	EDG_NEXTION_SendFrame(&hedgNextion);

	return;

}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_ShowDate(void)
{

	EDG_RTC_GetDate(&hedgRTC, EDG_RTC_ADDRESS);

	sprintf((char *)hedgNextion.TxFrame,"t1.txt=\"%02u:%02u%s-%s\"", hedgRTC.CurrentDate.Hour,
																	 hedgRTC.CurrentDate.Minute,
																	 (char *)EDG_NEXTION_AmPmNames[hedgRTC.CurrentDate.AmPm],
																	 (char *)EDG_NEXTION_WeekdayNames[hedgRTC.CurrentDate.WeekDay]);
	EDG_NEXTION_SendFrame(&hedgNextion);

	sprintf((char *)hedgNextion.TxFrame,"t2.txt=\"%02u/%s/20%02u\"", hedgRTC.CurrentDate.Day,
																	 (char *)EDG_NEXTION_MonthNames[hedgRTC.CurrentDate.Month],
																	 hedgRTC.CurrentDate.Year);
	EDG_NEXTION_SendFrame(&hedgNextion);

	return;

}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_SetConfigPage(void)
{

	uint8_t Counter;

	for(Counter = 0; Counter < hedgAccontrol.UnitsQty; Counter++)
	{

		if(hedgAccontrol.Units[Counter].SensorStatus == EDG_AC_CONTROL_SENSOR_STATUS_OK)
		{
			sprintf((char *)hedgNextion.TxFrame, "sl%d.val=%d", Counter, (int)hedgAccontrol.Units[Counter].Pid.SetPoint);
			EDG_NEXTION_SendFrame(&hedgNextion);
			sprintf((char *)hedgNextion.TxFrame, "tset%d.val=%d", Counter, (int)hedgAccontrol.Units[Counter].Pid.SetPoint);
			EDG_NEXTION_SendFrame(&hedgNextion);
			sprintf((char *)hedgNextion.TxFrame, "temp%d.val=%d", Counter, (int)hedgAccontrol.Units[Counter].Pid.SetPoint);
			EDG_NEXTION_SendFrame(&hedgNextion);

		}
		else
		{
			sprintf((char *)hedgNextion.TxFrame, "sl%d.val=%d", Counter, 0);
			EDG_NEXTION_SendFrame(&hedgNextion);
			sprintf((char *)hedgNextion.TxFrame, "bar%d.val=%d", Counter, 0);
			EDG_NEXTION_SendFrame(&hedgNextion);

			sprintf((char *)hedgNextion.TxFrame, "vis sl%d,0", Counter);
			EDG_NEXTION_SendFrame(&hedgNextion);
			sprintf((char *)hedgNextion.TxFrame, "vis bar%d,0", Counter);
			EDG_NEXTION_SendFrame(&hedgNextion);
			sprintf((char *)hedgNextion.TxFrame, "vis bup%d,0", Counter);
			EDG_NEXTION_SendFrame(&hedgNextion);
			sprintf((char *)hedgNextion.TxFrame, "vis bdw%d,0", Counter);
			EDG_NEXTION_SendFrame(&hedgNextion);
			sprintf((char *)hedgNextion.TxFrame, "vis tset%d,0", Counter);
			EDG_NEXTION_SendFrame(&hedgNextion);
			sprintf((char *)hedgNextion.TxFrame, "vis tcur%d,0", Counter);
			EDG_NEXTION_SendFrame(&hedgNextion);

		}

	}

	EDG_STATE_MACHINE_RefreshConfigPage();
	return;

}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_RefreshConfigPage(void)
{

	uint8_t Counter;

	for(Counter = 0; Counter < hedgAccontrol.UnitsQty; Counter++)
	{

		if(hedgAccontrol.Units[Counter].SensorStatus == EDG_AC_CONTROL_SENSOR_STATUS_OK)
		{
			/* (13/05/2023) Se ajusta para mostrar el valor real, se suma el offset*/
			sprintf((char *)hedgNextion.TxFrame, "bar%d.val=%d", Counter, (int)(hedgAccontrol.Units[Counter].Pid.CurrentValue + hedgAccontrol.Units[Counter].Pid.Offset));
			EDG_NEXTION_SendFrame(&hedgNextion);
			sprintf((char *)hedgNextion.TxFrame, "tcur%d.val=%d", Counter, (int)(hedgAccontrol.Units[Counter].Pid.CurrentValue + hedgAccontrol.Units[Counter].Pid.Offset));
			EDG_NEXTION_SendFrame(&hedgNextion);
		}

	}

	return;

}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_ChangeTemperature(void)
{

	hedgAccontrol.Units[hedgNextion.DataReceived[EDG_STATE_MACHINE_CHANGE_TEMP_POS_UNIT]].Pid.SetPoint = (float)hedgNextion.DataReceived[EDG_STATE_MACHINE_CHANGE_TEMP_POS_VALUE];
	return;

}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_CheckMemory(void)
{
	uint8_t Temp[10] = {0};
	uint8_t Counter;

	if(EDG_MEMORY_ReadMemory(EDG_MEMORY_ADDRESS_MEM1,
						  	 EDG_MEM_ADDR_BASE_PROGRAM,
							 Temp,
							 1) == EDG_MEMORY_STATE_OK)
	{
		if(Temp[0] == 0xFF)
		{
			for(Counter = 0; Counter < EDG_MEM_ADDR_VALUES_X_PROGRAM; Counter++)
			{
				Temp[Counter] = 60;
			}

			for(Counter = 0; Counter < EDG_MEM_ADDR_PROGRAM_QTY; Counter++)
			{
				EDG_MEMORY_WriteMemory(EDG_MEMORY_ADDRESS_MEM1,
									   (EDG_MEM_ADDR_BASE_PROGRAM + (EDG_MEM_ADDR_PROGRAM_OFFSET * Counter)),
									   Temp,
									   EDG_MEM_ADDR_VALUES_X_PROGRAM);
			}
		}

	}

	if(EDG_MEMORY_ReadMemory(EDG_MEMORY_ADDRESS_MEM1,
						  	 EDG_MEM_ADDR_BASE_SCHEDULE,
							 Temp,
							 1) == EDG_MEMORY_STATE_OK)
	{
		if(Temp[0] == 0xFF)
		{
			Temp[0] = 0; Temp[1] = 6; Temp[2] = 0; Temp[3] = 0;
			Temp[4] = 6; Temp[5] = 0; Temp[6] = 1; Temp[7] = 0;

			for(Counter = 0; Counter < EDG_MEM_ADDR_SCHEDULE_QTY; Counter++)
			{
				EDG_MEMORY_WriteMemory(EDG_MEMORY_ADDRESS_MEM1,
									   (EDG_MEM_ADDR_BASE_SCHEDULE + (EDG_MEM_ADDR_SCHEDULE_OFFSET * Counter)),
									   Temp,
									   EDG_MEM_ADDR_VALUES_X_SCHEDULE);
			}
		}

	}
	return;

}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_SaveProgramValues(uint32_t Program)
{

	uint8_t Temp[EDG_MEM_ADDR_VALUES_X_PROGRAM] = {0};
	uint8_t Counter;

	for(Counter = 0; Counter < EDG_MEM_ADDR_VALUES_X_PROGRAM; Counter++)
	{
		Temp[Counter] = (uint8_t)hedgNextion.DataReceived[Counter+2];
	}

	if(EDG_MEMORY_WriteMemory(EDG_MEMORY_ADDRESS_MEM1,
						  	  (EDG_MEM_ADDR_BASE_PROGRAM + (EDG_MEM_ADDR_PROGRAM_OFFSET * Program)),
							  Temp,
							  EDG_MEM_ADDR_VALUES_X_PROGRAM) == EDG_MEMORY_STATE_OK)
	{
		sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"   Programa    guardado!!!\"");
	}
	else
	{
		sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"  Error al     guardar el    programa...\"");
	}
	EDG_NEXTION_SendFrame(&hedgNextion);

	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_LoadProgramValues(uint32_t Program)
{

	uint8_t Temp[EDG_MEM_ADDR_VALUES_X_PROGRAM] = {0};
	uint8_t Counter;

	if(EDG_MEMORY_ReadMemory(EDG_MEMORY_ADDRESS_MEM1,
						  	 (EDG_MEM_ADDR_BASE_PROGRAM + (EDG_MEM_ADDR_PROGRAM_OFFSET * Program)),
							 Temp,
							 EDG_MEM_ADDR_VALUES_X_PROGRAM) != EDG_MEMORY_STATE_OK)
	{
		sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"   Error al       cargar      programa...\"");
		EDG_NEXTION_SendFrame(&hedgNextion);
		return;
	}

	for(Counter = 0; Counter < EDG_MEM_ADDR_VALUES_X_PROGRAM; Counter++)
	{

		sprintf((char *)hedgNextion.TxFrame,"h%d.val=%d", Counter, Temp[Counter]);
		EDG_NEXTION_SendFrame(&hedgNextion);
		sprintf((char *)hedgNextion.TxFrame,"n%d.val=%d", Counter, Temp[Counter]);
		EDG_NEXTION_SendFrame(&hedgNextion);
		sprintf((char *)hedgNextion.TxFrame,"temp%d.val=%d", Counter, Temp[Counter]);
		EDG_NEXTION_SendFrame(&hedgNextion);

	}

	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_SaveScheduleValues(uint32_t Day)
{

	uint8_t Temp[EDG_MEM_ADDR_VALUES_X_SCHEDULE] = {0};
	uint8_t Counter;

	for(Counter = 0; Counter < EDG_MEM_ADDR_VALUES_X_SCHEDULE; Counter++)
	{
		Temp[Counter] = (uint8_t)hedgNextion.DataReceived[Counter+2];
	}

	if(Temp[0] == 1)
	{

		if(EDG_MEMORY_WriteMemory(EDG_MEMORY_ADDRESS_MEM1,
								  (EDG_MEM_ADDR_BASE_SCHEDULE + (EDG_MEM_ADDR_SCHEDULE_OFFSET * Day)),
								  Temp,
								  EDG_MEM_ADDR_VALUES_X_SCHEDULE) == EDG_MEMORY_STATE_OK)
		{
			sprintf((char *)hedgNextion.TxFrame,"t39.txt=\" Dia configurado    como activo!!!\"");
		}
		else
		{
			sprintf((char *)hedgNextion.TxFrame,"t39.txt=\" Error al activar     el dia...\"");
		}


	}
	else
	{

		if(EDG_MEMORY_WriteMemory(EDG_MEMORY_ADDRESS_MEM1,
								  (EDG_MEM_ADDR_BASE_SCHEDULE+ (EDG_MEM_ADDR_SCHEDULE_OFFSET * Day)),
								  &Temp[0],
								  1) == EDG_MEMORY_STATE_OK)
		{
			sprintf((char *)hedgNextion.TxFrame,"t39.txt=\"  Dia configurado  como inactivo!!!\"");
		}
		else
		{
			sprintf((char *)hedgNextion.TxFrame,"t39.txt=\"     Error al        inactivar el         dia...\"");
		}
	}

	EDG_NEXTION_SendFrame(&hedgNextion);
	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_LoadScheduleValues(void)
{

	uint8_t Temp[EDG_MEM_ADDR_VALUES_X_SCHEDULE] = {0};
	uint8_t Counter;


	for(Counter = 0; Counter < EDG_MEM_ADDR_SCHEDULE_QTY; Counter++)
	{

		if(EDG_MEMORY_ReadMemory(EDG_MEMORY_ADDRESS_MEM1,
								 (EDG_MEM_ADDR_BASE_SCHEDULE+ (EDG_MEM_ADDR_SCHEDULE_OFFSET * Counter)),
								 Temp,
								 EDG_MEM_ADDR_VALUES_X_SCHEDULE) != EDG_MEMORY_STATE_OK)
		{
			sprintf((char *)hedgNextion.TxFrame,"t39.txt=\"Error al leer calendario...\"");
			EDG_NEXTION_SendFrame(&hedgNextion);
			return;
		}

		if(Temp[0] == 0)
		{
			sprintf((char *)hedgNextion.TxFrame,"ac%d.txt=\"NO\"", Counter);
		}
		else
		{
			sprintf((char *)hedgNextion.TxFrame,"ac%d.txt=\"SI\"", Counter);

		}
		EDG_NEXTION_SendFrame(&hedgNextion);

		if(Temp[3] == 0)
		{
			sprintf((char *)hedgNextion.TxFrame,"he%d.txt=\"%02d:%02dam\"", Counter, Temp[1], Temp[2]);
		}
		else
		{
			sprintf((char *)hedgNextion.TxFrame,"he%d.txt=\"%02d:%02dpm\"", Counter, Temp[1], Temp[2]);
		}
		EDG_NEXTION_SendFrame(&hedgNextion);

		if(Temp[6] == 0)
		{
			sprintf((char *)hedgNextion.TxFrame,"ha%d.txt=\"%02d:%02dam\"", Counter, Temp[4], Temp[5]);
		}
		else
		{
			sprintf((char *)hedgNextion.TxFrame,"ha%d.txt=\"%02d:%02dpm\"", Counter, Temp[4], Temp[5]);
		}
		EDG_NEXTION_SendFrame(&hedgNextion);

		sprintf((char *)hedgNextion.TxFrame,"pg%d.txt=\"%d\"", Counter, Temp[7] + 1);
		EDG_NEXTION_SendFrame(&hedgNextion);

	}

	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_SaveOffsetValues(void)
{

	uint8_t Temp[EDG_MEM_ADDR_OFFSET_QTY] = {0};
	uint8_t Counter;

	for(Counter = 0; Counter < EDG_MEM_ADDR_OFFSET_QTY; Counter++)
	{
		Temp[Counter] = (uint8_t)hedgNextion.DataReceived[Counter+1];
	}

	if(EDG_MEMORY_WriteMemory(EDG_MEMORY_ADDRESS_MEM1,
							  EDG_MEM_ADDR_BASE_OFFSET,
							  Temp,
							  EDG_MEM_ADDR_OFFSET_QTY) == EDG_MEMORY_STATE_OK)
	{
		sprintf((char *)hedgNextion.TxFrame,"t5.txt=\"Valores de offset guardados!!!\"");
	}
	else
	{
		sprintf((char *)hedgNextion.TxFrame,"t5.txt=\"Error al guardar valores de    offset...\"");
	}
	EDG_NEXTION_SendFrame(&hedgNextion);

	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_LoadOffsetValues(void)
{

	uint8_t Temp[EDG_MEM_ADDR_OFFSET_QTY] = {0};
	uint8_t Counter;

	if(EDG_MEMORY_ReadMemory(EDG_MEMORY_ADDRESS_MEM1,
						     EDG_MEM_ADDR_BASE_OFFSET,
							 Temp,
							 EDG_MEM_ADDR_OFFSET_QTY) != EDG_MEMORY_STATE_OK)
	{
		sprintf((char *)hedgNextion.TxFrame,"t5.txt=\"Error al cargar valores de     offset...\"");
		EDG_NEXTION_SendFrame(&hedgNextion);
		return;
	}
	else
	{
		for(Counter = 0; Counter < EDG_MEM_ADDR_OFFSET_QTY; Counter++)
		{
			sprintf((char *)hedgNextion.TxFrame,"of%d.val=%d", Counter, Temp[Counter]);
			EDG_NEXTION_SendFrame(&hedgNextion);
			sprintf((char *)hedgNextion.TxFrame,"off%d.val=%d", Counter, Temp[Counter]);
			EDG_NEXTION_SendFrame(&hedgNextion);
		}
	}
	return;
}
/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_BuzzerConfig(void)
{
	if(hedgNextion.DataReceived[1] == 0)
	{
		hedgBuzzer.Status = EDG_BUZZER_STATUS_INACTIVE;
	}
	else
	{
		hedgBuzzer.Status = EDG_BUZZER_STATUS_ACTIVE;
		EDG_BUZZER_Pulse(&hedgBuzzer);
	}
	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_BrightSave(void)
{

	hedgNextion.Bright.Level = hedgNextion.DataReceived[1];

	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_CheckButtonSchedule(void)
{

	uint8_t Temp;
	if(EDG_MEMORY_ReadMemory(EDG_MEMORY_ADDRESS_MEM1,
							 EDG_MEM_ADDR_BASE_SCHEDULE,
							 &Temp,
							 1) != EDG_MEMORY_STATE_OK)
	{
		sprintf((char *)hedgNextion.TxFrame,"t39.txt=\"Error al leer calendario...\"");
		EDG_NEXTION_SendFrame(&hedgNextion);
		return;
	}

	if(Temp == 1)
	{
		sprintf((char *)hedgNextion.TxFrame,"bt0.val=1");
		EDG_NEXTION_SendFrame(&hedgNextion);
	}

	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_CheckButtonBuzzer(void)
{

	if(hedgBuzzer.Status == EDG_BUZZER_STATUS_INACTIVE)
	{
		sprintf((char *)hedgNextion.TxFrame,"bt0.val=0");
	}
	else
	{
		sprintf((char *)hedgNextion.TxFrame,"bt0.val=1");
	}
	EDG_NEXTION_SendFrame(&hedgNextion);

	return;

}


/**
  * @brief
  * @param
  * @retval
  */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{

//	if(GPIO_Pin == TEST_IN_Pin)
//	{
//		EDG_AC_CONTROL_ToogleAcRelay();
//	}

	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_Manual(void)
{
	//Bajar carrusel
	if(tempVal == 0)
	{
		EDG_RELE_ELEVAR_ACTIVO();
		HAL_Delay(500); //Se coloca delay como antirrebote pero es mejor mirar otra opcion
		EDG_WS2812_Pixel(&hedgWs2812, 0, 50, 0, 50);
		EDG_WS2812_SendSpi(&hedgWs2812);
		//Se espera hasta que se detecte el sensor de posicion
		while(!EDG_SENSOR_POSICION());
		//Cuando se detecta el sensor se sabe que el carrusel esta abajo y se inicia de nuevo a agitar
		EDG_RELE_ELEVAR_INACTIVO();
		sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"CARRUSEL ABAJO\"");
		EDG_NEXTION_SendFrame(&hedgNextion);
	}
	//Subir carrusel
	else if(tempVal == 1)
	{

		EDG_RELE_ELEVAR_ACTIVO();
		HAL_Delay(500); //Se coloca delay como antirrebote pero es mejor mirar otra opcion
		EDG_WS2812_Pixel(&hedgWs2812, 0, 50, 50, 0);
		EDG_WS2812_SendSpi(&hedgWs2812);
		//Se espera un proceso de no sensado de carrusel y uno de sensado para
		//saber que el carrusel esta arriba
		while(!EDG_SENSOR_CARRUSEL());
		HAL_Delay(500); //Se coloca delay como antirrebote pero es mejor mirar otra opcion
		while(EDG_SENSOR_CARRUSEL());
		//Cuando el carrusel esta arriba se inactiva el rele de elevar
		EDG_RELE_ELEVAR_INACTIVO();
		HAL_Delay(500); //Se coloca delay como antirrebote pero es mejor mirar otra opcion
		EDG_WS2812_Pixel(&hedgWs2812, 0, 50, 0, 0);
		EDG_WS2812_SendSpi(&hedgWs2812);
		sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"CARRUSEL ARRIBA\"");
		EDG_NEXTION_SendFrame(&hedgNextion);
	}

	sprintf((char *)hedgNextion.TxFrame,"vis bt0,1");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis b0,1");
	EDG_NEXTION_SendFrame(&hedgNextion);

	EDG_NEXTION_EnableTouch(&hedgNextion);
	EDG_STATE_MACHINE_SetNextState(&hedgStateMachine, EDG_STATE_MACHINE_STATE_IDLE);
	return;

}
