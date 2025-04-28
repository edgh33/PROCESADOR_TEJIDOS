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
	ptrhedgStateMachine->TempValuint8 = 0;
	return;
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

			EDG_STATE_MACHINE_Process();
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

#if ENG_STATE_MACH_WATCHDOG == 1
	HAL_IWDG_Refresh(&hiwdg);
#endif

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
	//EDG_MAX6675_Init(&hedgMAX6675);
	EDG_DS18B20_Init(&hedgDS18B20);

	sprintf((char *)hedgNextion.TxFrame, "j0.val=75");
	EDG_NEXTION_SendFrame(&hedgNextion);
	HAL_Delay(1000);

	EDG_PROCESSOR_Init(&hedgProcessor);
	EDG_AC_CONTROL_Init(&hedgAccontrol, &hedgDS18B20);
	/*** Set the leds in the right color ***/
	EDG_STATE_MACHINE_SetLedColors();

	if(hedgAccontrol.AlarmStatus == EDG_AC_CONTROL_ALARM_STATUS_INACTIVE)
	{

		EDG_SCHEDULE_Init(&hedgSchedule, &hedgRTC);
		hedgTimer.FlagsStatus.Flag1m = true; //for make a fast check of schedule in idle

		EDG_BUZZER_Sound(10, 15, 0, 1, 2);

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

		EDG_TIMER_SyncRTC(&hedgTimer, &hedgRTC);
		EDG_TIMER_StarBaseTime(&hedgTimer, &hedgRTC);
		/*** If all is right change to idle state ***/
		if(EDG_STATE_MACHINE_TEST_STATE == EDG_STATE_MACHINE_TEST_INACTIVE)
		{
			if(EDG_STATE_MACHINE_CheckActiveProcess())
			{
				EDG_STATE_MACHINE_ResumeActiveProcess();
			}
			else
			{
				EDG_STATE_MACHINE_SetNextState(&hedgStateMachine, EDG_STATE_MACHINE_STATE_IDLE);
			}

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

#if ENG_STATE_MACH_WATCHDOG == 1
	if(hedgTimer.FlagsStatus.FlagIwdg == true)
	{
		hedgTimer.FlagsStatus.FlagIwdg = false;
		HAL_IWDG_Refresh(&hiwdg);

	}
#endif

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
			EDG_NEXTION_EnableTouch(&hedgNextion);
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

#if EDG_STATE_MACHINE_DEBUG_STATE == EDG_STATE_MACHINE_DEBUG_ACTIVE
		/*** Function that shows the current temperature value ***/
		EDG_STATE_MACHINE_TestTempetatureChips();
#endif
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
		EDG_SCHEDULE_CheckActive(&hedgSchedule, &hedgRTC);
		if(hedgSchedule.ActiveStatus == EDG_SCHEDULE_STATUS_TO_APPLY)
		{
			if((hedgProcessor.FlagsStatus.FlagSetRunning == 0) && (hedgProcessor.FlagsStatus.FlagRunning == 0))
			{
				hedgProcessor.FlagsStatus.FlagSetRunning = 1;
				hedgProcessor.FlagsStatus.FlagTimComplete = 1; //Trick for jump next state
				EDG_STATE_MACHINE_SetNextState(&hedgStateMachine, EDG_STATE_MACHINE_STATE_PROCESS);
				hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_PROGRAM] = hedgSchedule.Program;
				EDG_NEXTION_ChangePage(&hedgNextion, EDG_NEXTION_PAGE_EXECUTE);
				EDG_STATE_MACHINE_ShowDate();
				sprintf((char *)hedgNextion.TxFrame,"n24.val=%d", hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_PROGRAM] + 1);
				EDG_NEXTION_SendFrame(&hedgNextion);
				sprintf((char *)hedgNextion.TxFrame,"bt14.val=1");
				EDG_NEXTION_SendFrame(&hedgNextion);
				EDG_STATE_MACHINE_LoadProgramValues(hedgSchedule.Program, 0);
			}
			else
			{
				hedgSchedule.ActiveStatus = EDG_SCHEDULE_STATUS_APPLIDED;
				EDG_SCHEDULE_ClearScheduleToday(&hedgSchedule);
			}
		}
	}

	if(hedgProcessor.FlagsStatus.FlagTimComplete && hedgProcessor.FlagsStatus.FlagRunning)
	{
		EDG_STATE_MACHINE_SetNextState(&hedgStateMachine, EDG_STATE_MACHINE_STATE_PROCESS);
		return;
	}

	if(hedgProcessor.FlagsStatus.FlagTimComplete && hedgProcessor.FlagsStatus.FlagManual)
	{
		EDG_STATE_MACHINE_SetNextState(&hedgStateMachine, EDG_STATE_MACHINE_STATE_MANUAL);
		return;
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
		EDG_AC_CONTROL_PerformAll(&hedgAccontrol, &hedgDS18B20);

		/*** Change to the respective state  ***/
		EDG_AC_CONTROL_CheckAlarm(&hedgAccontrol);
		if(hedgAccontrol.AlarmStatus == EDG_AC_CONTROL_ALARM_STATUS_INACTIVE)
		{
			if(EDG_AC_CONTROL_GetRelayStatus() == EDG_AC_CONTROL_RELAY_STATUS_OPEN)
			{
				/*** Come back from alarm ***/
				EDG_AC_CONTROL_CloseAcRelay();
				EDG_STATE_MACHINE_ResumeActiveProcess();
				EDG_BUZZER_Stop();
			}
			else
			{
				if(hedgNextion.CurrentPage == EDG_NEXTION_PAGE_EXECUTE)
				{
					EDG_STATE_MACHINE_RefreshExecutePage();
				}
				EDG_STATE_MACHINE_SetNextState(&hedgStateMachine, EDG_STATE_MACHINE_STATE_IDLE);
			}
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
void EDG_STATE_MACHINE_Process(void)
{
	static uint8_t Temp[EDG_MEM_ADDR_VALUES_X_PROGRAM] = {0};
	static uint8_t Counter;

	//Check if the SetRunning flag is set for load the program values
	if(hedgProcessor.FlagsStatus.FlagSetRunning)
	{
		hedgProcessor.FlagsStatus.FlagSetRunning = 0;
		//if procces don't come from resume must load all values and star the process
		if(!hedgProcessor.FlagsStatus.FlagResume)
		{
			//Read the values of the program
			if(EDG_MEMORY_ReadMemory(EDG_MEMORY_ADDRESS_MEM1,
									 (EDG_MEM_ADDR_BASE_PROGRAM + (EDG_MEM_ADDR_PROGRAM_OFFSET * hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_PROGRAM])),
									 Temp,
									 EDG_MEM_ADDR_VALUES_X_PROGRAM) != EDG_MEMORY_STATE_OK)
			{
				sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"   ERROR      CARGANDO\"");
				EDG_NEXTION_SendFrame(&hedgNextion);
				hedgProcessor.FlagsStatus.FlagError = 1;
				return;
			}
			else
			{
				//Load the values of hours and minutes into the CurrentProcess array
				for(Counter = 0; Counter < 24; Counter++)
				{
					hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_HOUR_CONTAIN_1 + Counter] = Temp[1 + Counter];
				}
				//Load the value of active containers
				hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_ACTIVE_CONTAINERS] = Temp[0];
				hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CONTAINER] = 0;
				hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CAROU_POS] = EDG_PROCESSOR_CAROUSEL_POS_UNKNOW;
				hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_IS_ACTIVE] = 1; //Flag that check is there is a process running!!!

				//Save the current process in memory
				if(!EDG_STATE_MACHINE_SaveCurrentProcess())
				{
					sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"   ERROR      GUARDANDO\"");
					EDG_NEXTION_SendFrame(&hedgNextion);
					hedgProcessor.FlagsStatus.FlagError = 1;
					return;
				}
				else
				{

					//TODO!!! Add routine for temperature control!!!
					hedgProcessor.TotalTimeCurrenteContainer = (hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_HOUR_CONTAIN_1] * EDG_PROCESSOR_SECONDS_X_MIN) + hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_HOUR_CONTAIN_1 + 1];
					hedgProcessor.Index = EDG_PROCESSOR_ARR_POS_CURR_HOUR_CONTAIN_1 + (hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CONTAINER] * 2);
					hedgProcessor.TotalTimeDelayMinutes = (hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DELAY_HOUR] * EDG_PROCESSOR_SECONDS_X_MIN) + hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DELAY_MIN];
					if(hedgProcessor.TotalTimeDelayMinutes > 0)
					{
						hedgProcessor.CurrentState = EDG_PROCESSOR_STATE_DELAY;
						sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"  PROGRAMA   EN RETARDO\"");
						EDG_NEXTION_SendFrame(&hedgNextion);
						//Start Tim for 60 seconds
						EDG_PROCESSOR_StartTim(&hedgProcessor, EDG_PROCESSOR_SECONDS_X_MIN);
					}
					else
					{
						hedgProcessor.CurrentState = EDG_PROCESSOR_STATE_CHECK_CAROUSEL;
						hedgProcessor.FlagsStatus.FlagTimComplete = 1; //Trick for jump next state
						sprintf((char *)hedgNextion.TxFrame,"vis q%d,1", hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CONTAINER] + 1);
						EDG_NEXTION_SendFrame(&hedgNextion);
						sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"PROGRAMA EN  EJECUCION\"");
						EDG_NEXTION_SendFrame(&hedgNextion);
					}

					if(hedgSchedule.ActiveStatus == EDG_SCHEDULE_STATUS_TO_APPLY)
					{
						hedgSchedule.ActiveStatus = EDG_SCHEDULE_STATUS_APPLIDED;
						EDG_SCHEDULE_ClearScheduleToday(&hedgSchedule);
						hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DRIP_SECONDS] = Temp[25];
						hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_T1_STATE] = Temp[26];
						hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_T1_VALUE] = Temp[27];
						hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_T2_STATE] = Temp[28];
						hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_T2_VALUE] = Temp[29];

					}

					EDG_STATE_MACHINE_HideButtonsExecute();

					//Set the value and the state of the temperature process
					EDG_STATE_MACHINE_SetTemperatureProcess();
					//Set the flag for start de process
					hedgProcessor.FlagsStatus.FlagRunning = 1;
				}
			}
		}
		//if procces comes from resume all values are load now
		else
		{
			//Clear the flag for resume de process
			hedgProcessor.FlagsStatus.FlagResume = 0;

			if(hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CONTAINER] != hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_ACTIVE_CONTAINERS])
			{

				hedgProcessor.Index = EDG_PROCESSOR_ARR_POS_CURR_HOUR_CONTAIN_1 + (hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CONTAINER] * 2);
				hedgProcessor.TotalTimeCurrenteContainer = (hedgProcessor.CurrentProcess[hedgProcessor.Index] * EDG_PROCESSOR_SECONDS_X_MIN) + hedgProcessor.CurrentProcess[hedgProcessor.Index + 1];
				hedgProcessor.TotalTimeDelayMinutes = (hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DELAY_HOUR] * EDG_PROCESSOR_SECONDS_X_MIN) + hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DELAY_MIN];
				if(hedgProcessor.TotalTimeDelayMinutes > 0)
				{
					hedgProcessor.CurrentState = EDG_PROCESSOR_STATE_DELAY;
					sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"  PROGRAMA   EN RETARDO\"");
					EDG_NEXTION_SendFrame(&hedgNextion);
					//Start Tim for 60 seconds
					EDG_PROCESSOR_StartTim(&hedgProcessor, EDG_PROCESSOR_SECONDS_X_MIN);
				}
				else
				{
					//Make all the decisions based in the read data
					//Select the right process based on the carrusel position:
					//If carousel is downs it means that program is in execution
					switch(hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CAROU_POS])
					{
						case EDG_PROCESSOR_CAROUSEL_POS_UNKNOW:

							hedgProcessor.CurrentState = EDG_PROCESSOR_STATE_CHECK_CAROUSEL;
							hedgProcessor.FlagsStatus.FlagCheckCarousel = 0;
							hedgProcessor.FlagsStatus.FlagTimComplete = 1; //Trick for jump next state
							sprintf((char *)hedgNextion.TxFrame,"vis q%d,1", hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CONTAINER] + 1);
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"PROGRAMA EN  EJECUCION\"");
							EDG_NEXTION_SendFrame(&hedgNextion);

							break;

						case EDG_PROCESSOR_CAROUSEL_POS_DOWN:

							hedgProcessor.CurrentState = EDG_PROCESSOR_STATE_RUNNING;
							sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"PROGRAMA EN  EJECUCION\"");
							EDG_NEXTION_SendFrame(&hedgNextion);
							EDG_PROCESSOR_RAISE_RELAY_INACTIVE();
							EDG_PROCESSOR_SHAKE_RELAY_ACTIVE();
							HAL_Delay(EDG_PROCESSOR_DELAY_TIME_MS);
							EDG_PROCESSOR_StopTim(&hedgProcessor);
							EDG_PROCESSOR_StartTim(&hedgProcessor, EDG_PROCESSOR_SECONDS_X_MIN); //Start Tim for 60 seconds for start counting

							break;

						case EDG_PROCESSOR_CAROUSEL_POS_RAISING_1:

							hedgProcessor.CurrentState = EDG_PROCESSOR_STATE_ROTATING;
							hedgProcessor.CurrentCarouselState = EDG_PROCESSOR_CAROUSEL_STATE_RAISING;
							hedgProcessor.FlagsStatus.FlagCheckCarousel = 0; // for checking the carousel raising
							hedgProcessor.CounterCheckCarousel = 0;
							sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"ROTANDO...\"");
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"vis bt0,0");
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"vis bt14,0");
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"vis b4,0");
							EDG_NEXTION_SendFrame(&hedgNextion);
							EDG_PROCESSOR_RAISE_RELAY_ACTIVE();
							EDG_PROCESSOR_SHAKE_RELAY_INACTIVE();
							HAL_Delay(EDG_PROCESSOR_DELAY_TIME_MS);
							hedgProcessor.CounterCheckCarousel = 0;
							EDG_PROCESSOR_StopTim(&hedgProcessor);
							EDG_PROCESSOR_StartTimMs(&hedgProcessor, EDG_PROCESSOR_CHECK_TIME_MS); //The tim starts for 200ms checking of carousel changing position

							break;

						case EDG_PROCESSOR_CAROUSEL_POS_RAISING_2:

							hedgProcessor.CurrentState = EDG_PROCESSOR_STATE_ROTATING;
							hedgProcessor.CurrentCarouselState = EDG_PROCESSOR_CAROUSEL_STATE_RAISING;
							hedgProcessor.FlagsStatus.FlagCheckCarousel = 1; // for checking the carousel raising
							hedgProcessor.CounterCheckCarousel = 0;
							sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"ROTANDO...\"");
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"vis bt0,0");
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"vis bt14,0");
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"vis b4,0");
							EDG_NEXTION_SendFrame(&hedgNextion);
							EDG_PROCESSOR_RAISE_RELAY_ACTIVE();
							EDG_PROCESSOR_SHAKE_RELAY_INACTIVE();
							HAL_Delay(EDG_PROCESSOR_DELAY_TIME_MS);
							EDG_PROCESSOR_StopTim(&hedgProcessor);
							EDG_PROCESSOR_StartTimMs(&hedgProcessor, EDG_PROCESSOR_CHECK_TIME_MS); //The tim starts for 200ms checking of carousel changing position

							break;

						case EDG_PROCESSOR_CAROUSEL_POS_WAITING:

							hedgProcessor.CurrentCarouselState = EDG_PROCESSOR_CAROUSEL_STATE_WAITING;
							hedgProcessor.FlagsStatus.FlagCheckCarousel = 0;
							hedgProcessor.CounterCheckCarousel = 0;
							sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"ESCURRIENDO\"");
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"vis bt0,0");
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"vis bt14,0");
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"vis b4,0");
							EDG_NEXTION_SendFrame(&hedgNextion);
							EDG_PROCESSOR_RAISE_RELAY_INACTIVE();
							EDG_PROCESSOR_SHAKE_RELAY_INACTIVE();
							HAL_Delay(EDG_PROCESSOR_DELAY_TIME_MS);
							EDG_PROCESSOR_StopTim(&hedgProcessor);
							EDG_PROCESSOR_StartTimMs(&hedgProcessor, EDG_PROCESSOR_WAIT_BASE_MS); //The tim starts for 1000ms waiting time

							break;

						case EDG_PROCESSOR_CAROUSEL_POS_DESCENDING:

							hedgProcessor.CurrentCarouselState = EDG_PROCESSOR_CAROUSEL_STATE_DESCENDING;
							hedgProcessor.CounterCheckCarousel = 0;
							sprintf((char *)hedgNextion.TxFrame,"n31.val=%03d", hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DRIP_SECONDS]);
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"ROTANDO...\"");
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"vis bt0,0");
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"vis bt14,0");
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"vis b4,0");
							EDG_NEXTION_SendFrame(&hedgNextion);
							EDG_PROCESSOR_RAISE_RELAY_ACTIVE();
							EDG_PROCESSOR_SHAKE_RELAY_INACTIVE();
							HAL_Delay(EDG_PROCESSOR_DELAY_TIME_MS);
							EDG_PROCESSOR_StopTim(&hedgProcessor);
							EDG_PROCESSOR_StartTimMs(&hedgProcessor, EDG_PROCESSOR_CHECK_TIME_MS); //The tim starts for 200ms checking of carousel changing position

							break;
					}
				}
			}
			else
			{
				hedgProcessor.CurrentState = EDG_PROCESSOR_STATE_FINISH;
				EDG_PROCESSOR_SHAKE_RELAY_ACTIVE();
				HAL_Delay(EDG_PROCESSOR_DELAY_TIME_MS);
				hedgProcessor.FlagsStatus.FlagShaking = 1;
				hedgProcessor.FlagsStatus.FlagSetFinish = 1;
				hedgProcessor.FlagsStatus.FlagTimComplete = 1; //Trick for jump next state
			}

			EDG_STATE_MACHINE_SetTemperatureProcess();
			//Set the flag for start de process
			hedgProcessor.FlagsStatus.FlagRunning = 1;
		}
	}
	//If process is running check the current state
	if(hedgProcessor.FlagsStatus.FlagRunning)
	{
		switch (hedgProcessor.CurrentState)
		{

			case EDG_PROCESSOR_STATE_STOP:

				sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"  PROGRAMA    DETENIDO\"");
				EDG_NEXTION_SendFrame(&hedgNextion);
				EDG_PROCESSOR_SHAKE_RELAY_INACTIVE();
				hedgProcessor.FlagsStatus.FlagRunning = 0;
				hedgProcessor.FlagsStatus.FlagShaking = 0;

				EDG_STATE_MACHINE_LoadProgramValues(hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_PROGRAM], 0);
				sprintf((char *)hedgNextion.TxFrame,"prog.val=%d", hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_PROGRAM] + 1);
				EDG_NEXTION_SendFrame(&hedgNextion);
				sprintf((char *)hedgNextion.TxFrame,"n24.val=%d", hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_PROGRAM] + 1);
				EDG_NEXTION_SendFrame(&hedgNextion);
				EDG_STATE_MACHINE_ShowButtonsExecute();

				//Turn off the temperature control
				hedgAccontrol.Units[0].ControlStatus = EDG_AC_CONTROL_CONTROL_STATUS_INACTIVE;
				EDG_AC_CONTROL_StopPWMOutput(&hedgAccontrol, 0);
				hedgAccontrol.Units[0].PwmStatus = EDG_AC_CONTROL_PWM_STATUS_INACTIVE;
				hedgAccontrol.Units[0].Pid.CurrentValue = 0;

				hedgAccontrol.Units[1].ControlStatus = EDG_AC_CONTROL_CONTROL_STATUS_INACTIVE;
				EDG_AC_CONTROL_StopPWMOutput(&hedgAccontrol, 1);
				hedgAccontrol.Units[1].PwmStatus = EDG_AC_CONTROL_PWM_STATUS_INACTIVE;
				hedgAccontrol.Units[1].Pid.CurrentValue = 0;

				hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_IS_ACTIVE] = 0; //Flag that check is there is a process running!!!
				EDG_PROCESSOR_StopTim(&hedgProcessor);
				EDG_STATE_MACHINE_SaveCurrentProcess(); //Check if the save process is right!!

				break;

			case EDG_PROCESSOR_STATE_DELAY:

				if(hedgProcessor.FlagsStatus.FlagTimComplete)
				{
					hedgProcessor.FlagsStatus.FlagTimComplete = 0;

					hedgProcessor.TotalTimeDelayMinutes--;
					if(hedgProcessor.TotalTimeDelayMinutes > 0)
					{
						if(hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DELAY_MIN] == 0)
						{
							hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DELAY_MIN] = 59;
							if(hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DELAY_HOUR] > 0)
							{
								hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DELAY_HOUR]--;
							}
						}
						else
						{
							hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DELAY_MIN]--;
						}
						sprintf((char *)hedgNextion.TxFrame,"n27.val=%d", hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DELAY_HOUR]);
						EDG_NEXTION_SendFrame(&hedgNextion);
						sprintf((char *)hedgNextion.TxFrame,"n28.val=%d", hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DELAY_MIN]);
						EDG_NEXTION_SendFrame(&hedgNextion);
					}
					else
					{
						hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DELAY_MIN] = 0;
						EDG_STATE_MACHINE_SaveCurrentProcess(); //Check if the save process is right!!
						hedgProcessor.CurrentState = EDG_PROCESSOR_STATE_CHECK_CAROUSEL;
						hedgProcessor.FlagsStatus.FlagTimComplete = 1; //Trick for jump next state
						sprintf((char *)hedgNextion.TxFrame,"n28.val=0");
						EDG_NEXTION_SendFrame(&hedgNextion);
					}
					EDG_STATE_MACHINE_SaveCurrentProcess(); //Check if the save process is right!!
				}

				break;

			case EDG_PROCESSOR_STATE_CHECK_CAROUSEL:

				if(hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CAROU_POS] == EDG_PROCESSOR_CAROUSEL_POS_UNKNOW)
				{
					//Check if carousel is down
					if(!EDG_PROCESSOR_SENSOR_POSITION() && !hedgProcessor.FlagsStatus.FlagCheckCarousel)
					{
						hedgProcessor.FlagsStatus.FlagCheckCarousel = 1;
						hedgProcessor.CounterCheckCarousel = 0;
						EDG_PROCESSOR_RAISE_RELAY_ACTIVE();
						HAL_Delay(EDG_PROCESSOR_DELAY_TIME_MS);
						EDG_PROCESSOR_StopTim(&hedgProcessor);
						EDG_PROCESSOR_StartTimMs(&hedgProcessor, EDG_PROCESSOR_CHECK_TIME_MS); //The tim starts for 200ms checking of carousel position
						sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"  UBICANDO    CARRUSEL\"");
						EDG_NEXTION_SendFrame(&hedgNextion);
					}
					else if(EDG_PROCESSOR_SENSOR_POSITION() && !hedgProcessor.FlagsStatus.FlagCheckCarousel)
					{
						EDG_PROCESSOR_RAISE_RELAY_INACTIVE();
						EDG_PROCESSOR_SHAKE_RELAY_ACTIVE();
						hedgProcessor.FlagsStatus.FlagCheckCarousel = 0;
						hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CAROU_POS] = EDG_PROCESSOR_CAROUSEL_POS_DOWN;
						hedgProcessor.CurrentState = EDG_PROCESSOR_STATE_RUNNING;
						EDG_STATE_MACHINE_SaveCurrentProcess(); //Check if the save process is right!!
						EDG_PROCESSOR_StopTim(&hedgProcessor);
						//Start Tim for 60 seconds
						EDG_PROCESSOR_StartTim(&hedgProcessor, EDG_PROCESSOR_SECONDS_X_MIN);
						sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"PROGRAMA EN  EJECUCION\"");
						EDG_NEXTION_SendFrame(&hedgNextion);
					}
					else if(hedgProcessor.FlagsStatus.FlagCheckCarousel
							&& hedgProcessor.FlagsStatus.FlagTimComplete
							&& (hedgProcessor.CounterCheckCarousel < (EDG_PROCESSOR_MAX_CHECKS_ROTATING * 2)) //30 seconds = 0.2*30
							)
					{
						hedgProcessor.FlagsStatus.FlagTimComplete = 0;
						hedgProcessor.CounterCheckCarousel++;
						if(EDG_PROCESSOR_SENSOR_POSITION())
						{
							EDG_PROCESSOR_RAISE_RELAY_INACTIVE();
							EDG_PROCESSOR_SHAKE_RELAY_ACTIVE();
							hedgProcessor.FlagsStatus.FlagCheckCarousel = 0;
							hedgProcessor.CounterCheckCarousel = 0;
							hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CAROU_POS] = EDG_PROCESSOR_CAROUSEL_POS_DOWN;
							EDG_STATE_MACHINE_SaveCurrentProcess(); //Check if the save process is right!!
							hedgProcessor.CurrentState = EDG_PROCESSOR_STATE_RUNNING;
							sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"PROGRAMA EN  EJECUCION\"");
							EDG_NEXTION_SendFrame(&hedgNextion);
							EDG_PROCESSOR_StopTim(&hedgProcessor);
							EDG_PROCESSOR_StartTim(&hedgProcessor, EDG_PROCESSOR_SECONDS_X_MIN); //Start Tim for 60 seconds for start counting

						}
					}
					if(hedgProcessor.CounterCheckCarousel >= EDG_PROCESSOR_MAX_CHECKS_ROTATING)
					{
						hedgProcessor.CurrentState = EDG_PROCESSOR_STATE_ALERT;
						sprintf((char *)hedgNextion.TxFrame,"t0.txt=\" ERROR CON    CARRUSEL\"");
						EDG_NEXTION_SendFrame(&hedgNextion);
					}
				}

				break;

			case EDG_PROCESSOR_STATE_RUNNING:

				if(hedgProcessor.FlagsStatus.FlagTimComplete)
				{
					hedgProcessor.FlagsStatus.FlagTimComplete = 0;

					hedgProcessor.TotalTimeCurrenteContainer--;

					if(hedgProcessor.TotalTimeCurrenteContainer > 0)
					{

						if(hedgProcessor.CurrentProcess[hedgProcessor.Index + 1] == 0) //Container Minute is +1
						{
							hedgProcessor.CurrentProcess[hedgProcessor.Index + 1] = 59; //Container Minute is +1
							if(hedgProcessor.CurrentProcess[hedgProcessor.Index] > 0) //Container Hour
							{
								hedgProcessor.CurrentProcess[hedgProcessor.Index]--;
							}
						}
						else
						{
							hedgProcessor.CurrentProcess[hedgProcessor.Index + 1]--;
						}
						sprintf((char *)hedgNextion.TxFrame,"n%d.val=%d", (hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CONTAINER] * 2),
																		  hedgProcessor.CurrentProcess[hedgProcessor.Index]);
						EDG_NEXTION_SendFrame(&hedgNextion);
						sprintf((char *)hedgNextion.TxFrame,"n%d.val=%d", ((hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CONTAINER] * 2) + 1),
																		  hedgProcessor.CurrentProcess[hedgProcessor.Index + 1]);
						EDG_NEXTION_SendFrame(&hedgNextion);
						EDG_STATE_MACHINE_SaveCurrentProcess(); //Check if the save process is right!!
					}
					else
					{
						hedgProcessor.CurrentProcess[hedgProcessor.Index + 1] = 0; // minute equals to zero
						sprintf((char *)hedgNextion.TxFrame,"n%d.val=%d", ((hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CONTAINER] * 2) + 1),
																			hedgProcessor.CurrentProcess[hedgProcessor.Index + 1]);
						EDG_NEXTION_SendFrame(&hedgNextion);
						//SET THE NEXT CONTAINER!!!!
						//hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CONTAINER]++;
						//Compare current container with total container
						if((hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CONTAINER] + 1) == hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_ACTIVE_CONTAINERS])
						{
							hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CONTAINER]++;
							hedgProcessor.CurrentState = EDG_PROCESSOR_STATE_FINISH;
							hedgProcessor.FlagsStatus.FlagSetFinish = 1;
							hedgProcessor.FlagsStatus.FlagTimComplete = 1; //Trick for jump next state
						}
						else
						{
							//Set the index for show for the hour and minute of current container
							//hedgProcessor.Index = EDG_PROCESSOR_ARR_POS_CURR_HOUR_CONTAIN_1 + (hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CONTAINER] * 2);
							//hedgProcessor.TotalTimeCurrenteContainer = (hedgProcessor.CurrentProcess[hedgProcessor.Index] * EDG_PROCESSOR_SECONDS_X_MIN) + hedgProcessor.CurrentProcess[hedgProcessor.Index + 1];
							hedgProcessor.CurrentState = EDG_PROCESSOR_STATE_ROTATING;
							hedgProcessor.CurrentCarouselState = EDG_PROCESSOR_CAROUSEL_STATE_CHECKING;
							hedgProcessor.FlagsStatus.FlagTimComplete = 1; //Trick for jump next state
						}
						EDG_STATE_MACHINE_SaveCurrentProcess(); //Check if the save process is right!!
					}
				}

				break;

			case EDG_PROCESSOR_STATE_ROTATING:

				if(hedgProcessor.FlagsStatus.FlagTimComplete)
				{
					hedgProcessor.FlagsStatus.FlagTimComplete = 0;

					switch(hedgProcessor.CurrentCarouselState)
					{
						case EDG_PROCESSOR_CAROUSEL_STATE_CHECKING:

							sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"ROTANDO...\"");
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"vis bt0,0");
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"vis bt14,0");
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"vis b4,0");
							EDG_NEXTION_SendFrame(&hedgNextion);
							EDG_PROCESSOR_SHAKE_RELAY_INACTIVE();
							HAL_Delay(EDG_PROCESSOR_DELAY_TIME_MS);
							hedgProcessor.FlagsStatus.FlagShaking = 0;
							hedgProcessor.CounterCheckCarousel = 0;
							//Check if Carousel is in the right position
							if(EDG_PROCESSOR_SENSOR_POSITION() && !EDG_PROCESSOR_SENSOR_CAROUSEL())
							{

								hedgProcessor.FlagsStatus.FlagCheckCarousel = 0; // for checking the carousel raising
								hedgProcessor.CurrentCarouselState = EDG_PROCESSOR_CAROUSEL_STATE_RAISING;
								hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CAROU_POS] = EDG_PROCESSOR_CAROUSEL_POS_RAISING_1;
								EDG_STATE_MACHINE_SaveCurrentProcess(); //Check if the save process is right!!
								EDG_PROCESSOR_RAISE_RELAY_ACTIVE();
								HAL_Delay(EDG_PROCESSOR_DELAY_TIME_MS);
								EDG_PROCESSOR_StopTim(&hedgProcessor);
								EDG_PROCESSOR_StartTimMs(&hedgProcessor, EDG_PROCESSOR_CHECK_TIME_MS); //The tim starts for 200ms checking of carousel changing position
							}
							else
							{
								sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"ERROR POS 1\"");
								EDG_NEXTION_SendFrame(&hedgNextion);
								//what to do if carousel is not in the right position???
							}

							break;

						case EDG_PROCESSOR_CAROUSEL_STATE_RAISING:

							if(!hedgProcessor.FlagsStatus.FlagCheckCarousel)
							{
								hedgProcessor.CounterCheckCarousel++;
								if(EDG_PROCESSOR_SENSOR_CAROUSEL())
								{
									hedgProcessor.CounterCheckCarousel = 0;
									hedgProcessor.FlagsStatus.FlagCheckCarousel = 1;
									hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CAROU_POS] = EDG_PROCESSOR_CAROUSEL_POS_RAISING_2;
									EDG_STATE_MACHINE_SaveCurrentProcess(); //Check if the save process is right!!
								}
								else if(hedgProcessor.CounterCheckCarousel >= EDG_PROCESSOR_MAX_CHECKS_ROTATING)
								{
									sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"ERROR POS 2\"");
									EDG_NEXTION_SendFrame(&hedgNextion);
									//what to do if carousel is not in the right position???
									hedgProcessor.CounterCheckCarousel = 0;
								}
							}
							else
							{
								hedgProcessor.CounterCheckCarousel++;
								if(!EDG_PROCESSOR_SENSOR_CAROUSEL())
								{
									hedgProcessor.FlagsStatus.FlagCheckCarousel = 0;
									hedgProcessor.CounterCheckCarousel = 0;
									hedgProcessor.CurrentCarouselState = EDG_PROCESSOR_CAROUSEL_STATE_WAITING;
									hedgProcessor.FlagsStatus.FlagCarouselPos = 1;
									sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"ESCURRIENDO\"");
									EDG_NEXTION_SendFrame(&hedgNextion);
									hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CAROU_POS] = EDG_PROCESSOR_CAROUSEL_POS_WAITING;
									EDG_STATE_MACHINE_SaveCurrentProcess(); //Check if the save process is right!!
									EDG_PROCESSOR_RAISE_RELAY_INACTIVE();
									HAL_Delay(EDG_PROCESSOR_DELAY_TIME_MS);
									EDG_PROCESSOR_StopTim(&hedgProcessor);
									EDG_PROCESSOR_StartTimMs(&hedgProcessor, EDG_PROCESSOR_WAIT_BASE_MS); //The tim starts for 1000ms waiting time
								}
								else if(hedgProcessor.CounterCheckCarousel >= EDG_PROCESSOR_MAX_CHECKS_ROTATING)
								{
									sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"ERROR POS 3\"");
									EDG_NEXTION_SendFrame(&hedgNextion);
									//what to do if carousel is not in the right position???
									hedgProcessor.CounterCheckCarousel = 0;
								}

							}

						break;

					case EDG_PROCESSOR_CAROUSEL_STATE_WAITING:

						hedgProcessor.CounterCheckCarousel++;
						sprintf((char *)hedgNextion.TxFrame,"n31.val=%03d", (hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DRIP_SECONDS] - hedgProcessor.CounterCheckCarousel));
						EDG_NEXTION_SendFrame(&hedgNextion);
						if(hedgProcessor.CounterCheckCarousel >= hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DRIP_SECONDS])
						{
							sprintf((char *)hedgNextion.TxFrame,"n31.val=%03d", hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DRIP_SECONDS]);
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"ROTANDO...\"");
							EDG_NEXTION_SendFrame(&hedgNextion);
							hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CAROU_POS] = EDG_PROCESSOR_CAROUSEL_POS_DESCENDING;
							EDG_STATE_MACHINE_SaveCurrentProcess(); //Check if the save process is right!!
							EDG_PROCESSOR_RAISE_RELAY_ACTIVE();
							HAL_Delay(EDG_PROCESSOR_DELAY_TIME_END_MS);
							EDG_PROCESSOR_StopTim(&hedgProcessor);
							EDG_PROCESSOR_StartTimMs(&hedgProcessor, EDG_PROCESSOR_CHECK_TIME_MS); //The tim starts for 200ms checking of carousel changing position
							hedgProcessor.CounterCheckCarousel = 0;
							hedgProcessor.CurrentCarouselState = EDG_PROCESSOR_CAROUSEL_STATE_DESCENDING;
						}

						break;

					case EDG_PROCESSOR_CAROUSEL_STATE_DESCENDING:

						hedgProcessor.CounterCheckCarousel++;
						if(EDG_PROCESSOR_SENSOR_POSITION() && !EDG_SENSOR_CARRUSEL())
						{
							EDG_PROCESSOR_RAISE_RELAY_INACTIVE();
							HAL_Delay(EDG_PROCESSOR_DELAY_TIME_END_MS);
							EDG_PROCESSOR_SHAKE_RELAY_ACTIVE();
							HAL_Delay(EDG_PROCESSOR_DELAY_TIME_END_MS);

							//SET THE NEXT CONTAINER!!!!
							hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CONTAINER]++;
							hedgProcessor.Index = EDG_PROCESSOR_ARR_POS_CURR_HOUR_CONTAIN_1 + (hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CONTAINER] * 2);
							hedgProcessor.TotalTimeCurrenteContainer = (hedgProcessor.CurrentProcess[hedgProcessor.Index] * EDG_PROCESSOR_SECONDS_X_MIN) + hedgProcessor.CurrentProcess[hedgProcessor.Index + 1];
							sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"PROGRAMA EN  EJECUCION\"");
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"vis bt0,1");
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"vis bt14,1");
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"vis q%d,0", hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CONTAINER]); //Past container in zero
							EDG_NEXTION_SendFrame(&hedgNextion);
							sprintf((char *)hedgNextion.TxFrame,"vis q%d,1", hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CONTAINER] + 1); //Past current in one
							EDG_NEXTION_SendFrame(&hedgNextion);
							hedgProcessor.CurrentCarouselState = EDG_PROCESSOR_CAROUSEL_STATE_CHECKING;
							hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CAROU_POS] = EDG_PROCESSOR_CAROUSEL_POS_DOWN;
							hedgProcessor.FlagsStatus.FlagCarouselPos = 0;
							hedgProcessor.FlagsStatus.FlagShaking = 1;
							hedgProcessor.CurrentState = EDG_PROCESSOR_STATE_RUNNING;

							EDG_STATE_MACHINE_SaveCurrentProcess(); //Check if the save process is right!!
							EDG_PROCESSOR_StopTim(&hedgProcessor);
							EDG_PROCESSOR_StartTim(&hedgProcessor, EDG_PROCESSOR_SECONDS_X_MIN); //Start Tim for 60 seconds for start counting
						}
						else if(hedgProcessor.CounterCheckCarousel >= EDG_PROCESSOR_MAX_CHECKS_ROTATING)
						{
							sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"ERROR POS 4\"");
							EDG_NEXTION_SendFrame(&hedgNextion);
							//what to do if carousel is not in the right position???
							hedgProcessor.CounterCheckCarousel = 0;
						}

						break;
					}
				}

				break;

			case EDG_PROCESSOR_STATE_PAUSE:

				if(hedgProcessor.FlagsStatus.FlagSetPause)
				{
					hedgProcessor.FlagsStatus.FlagSetPause = 0;
					sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"  PROGRAMA    EN PAUSA\"");
					EDG_NEXTION_SendFrame(&hedgNextion);
					EDG_PROCESSOR_SHAKE_RELAY_INACTIVE();
					HAL_Delay(EDG_PROCESSOR_DELAY_TIME_MS);
				}
				break;

			case EDG_PROCESSOR_STATE_RESUME:

				hedgProcessor.CurrentState = hedgProcessor.LastState;
				if(hedgProcessor.CurrentState == EDG_PROCESSOR_STATE_DELAY)
				{
					sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"  PROGRAMA   EN RETARDO\"");
				}
				else if(hedgProcessor.CurrentState == EDG_PROCESSOR_STATE_RUNNING)
				{
					sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"PROGRAMA EN  EJECUCION\"");
					EDG_PROCESSOR_SHAKE_RELAY_ACTIVE();
					HAL_Delay(EDG_PROCESSOR_DELAY_TIME_MS);
				}
				EDG_NEXTION_SendFrame(&hedgNextion);

				break;

			case EDG_PROCESSOR_STATE_FINISH:

				if(hedgProcessor.FlagsStatus.FlagSetFinish)
				{
					hedgProcessor.FlagsStatus.FlagTimComplete = 0;
					hedgProcessor.FlagsStatus.FlagSetFinish = 0;

					EDG_BUZZER_Sound(50, 50, 100, 2, 10);
					sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"  PROGRAMA   FINALIZADO\"");
					EDG_NEXTION_SendFrame(&hedgNextion);
					sprintf((char *)hedgNextion.TxFrame,"vis bt0,0");
					EDG_NEXTION_SendFrame(&hedgNextion);
					sprintf((char *)hedgNextion.TxFrame,"vis b4,0");
					EDG_NEXTION_SendFrame(&hedgNextion);
					sprintf((char *)hedgNextion.TxFrame,"vis bt14,1");
					EDG_NEXTION_SendFrame(&hedgNextion);
					sprintf((char *)hedgNextion.TxFrame,"bt14.val=1");
					EDG_NEXTION_SendFrame(&hedgNextion);
					EDG_PROCESSOR_StopTim(&hedgProcessor);
					EDG_PROCESSOR_StartTimMs(&hedgProcessor, EDG_PROCESSOR_SECONDS_X_MIN); //The tim is set for checking each minute
				}

			case EDG_PROCESSOR_STATE_ALERT:

				break;

			default:
				break;
		}
	}

	EDG_STATE_MACHINE_SetNextState(&hedgStateMachine, EDG_STATE_MACHINE_STATE_IDLE);

	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_Manual(void)
{
	if(hedgProcessor.FlagsStatus.FlagSetManual)
	{
		hedgProcessor.FlagsStatus.FlagSetManual = 0;
		hedgProcessor.FlagsStatus.FlagManual = 1;
		hedgProcessor.ManualState = (EDG_PROCESSOR_ManualState)hedgStateMachine.TempValuint8;

		switch(hedgProcessor.ManualState)
		{

			case EDG_PROCESSOR_MANUAL_STATE_DOWN:

				sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"BAJANDO CARRUSEL\"");
				EDG_NEXTION_SendFrame(&hedgNextion);
				EDG_WS2812_Pixel(&hedgWs2812, 5, 50, 0, 50);
				EDG_WS2812_SendSpi(&hedgWs2812);
				EDG_PROCESSOR_RAISE_RELAY_ACTIVE();
				HAL_Delay(EDG_PROCESSOR_DELAY_TIME_MS);
				EDG_PROCESSOR_StopTim(&hedgProcessor);
				EDG_PROCESSOR_StartTimMs(&hedgProcessor, EDG_PROCESSOR_CHECK_TIME_MS); //The tim starts for 200ms checking of carousel position
				hedgProcessor.FlagsStatus.FlagTimComplete = 1;//Trick for jump next state

				break;

			case EDG_PROCESSOR_MANUAL_STATE_RAISE:

				sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"SUBIENDO CARRUSEL\"");
				EDG_NEXTION_SendFrame(&hedgNextion);
				EDG_WS2812_Pixel(&hedgWs2812, 5, 50, 50, 0);
				EDG_WS2812_SendSpi(&hedgWs2812);
				EDG_PROCESSOR_RAISE_RELAY_ACTIVE();
				HAL_Delay(EDG_PROCESSOR_DELAY_TIME_MS);
				hedgProcessor.FlagsStatus.FlagCheckCarousel = 0;
				EDG_PROCESSOR_StopTim(&hedgProcessor);
				EDG_PROCESSOR_StartTimMs(&hedgProcessor, EDG_PROCESSOR_CHECK_TIME_MS); //The tim starts for 200ms checking of carousel position
				hedgProcessor.FlagsStatus.FlagTimComplete = 1;//Trick for jump next state

				break;

			case EDG_PROCESSOR_MANUAL_STATE_SHAKE:
			case EDG_PROCESSOR_MANUAL_STATE_NO_SHAKE:

				hedgProcessor.FlagsStatus.FlagTimComplete = 1;//Trick for jump next state
				break;

			default:
				break;
		}

	}

	if(hedgProcessor.FlagsStatus.FlagTimComplete)
	{
		hedgProcessor.FlagsStatus.FlagTimComplete = 0;

		switch(hedgProcessor.ManualState)
		{
			//Down Carousel
			case EDG_PROCESSOR_MANUAL_STATE_DOWN:

				hedgProcessor.CounterCheckCarousel++;
				if(EDG_PROCESSOR_SENSOR_POSITION() && !EDG_SENSOR_CARRUSEL())
				{
					EDG_PROCESSOR_RAISE_RELAY_INACTIVE();
					HAL_Delay(EDG_PROCESSOR_DELAY_TIME_MS);
					sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"CARRUSEL ABAJO\"");
					EDG_NEXTION_SendFrame(&hedgNextion);
					hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CAROU_POS] = EDG_PROCESSOR_CAROUSEL_POS_DOWN;
					hedgProcessor.FlagsStatus.FlagTimComplete = 1;//Trick for jump next state
					EDG_STATE_MACHINE_SaveCurrentProcess(); //Check if the save process is right!!
					hedgProcessor.ManualState = EDG_PROCESSOR_MANUAL_STATE_FINISH;
					hedgProcessor.FlagsStatus.FlagCarouselPos = 0;
					hedgProcessor.FlagsStatus.FlagTimComplete = 1;//Trick for jump next state
				}
				else if(hedgProcessor.CounterCheckCarousel >= EDG_PROCESSOR_MAX_CHECKS_ROTATING)
				{
					sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"ERROR POS 4\"");
					EDG_NEXTION_SendFrame(&hedgNextion);
					//what to do if carousel is not in the right position???
					hedgProcessor.CounterCheckCarousel = 0;
				}

				break;

			//Up carousel
			case EDG_PROCESSOR_MANUAL_STATE_RAISE:

				if(!hedgProcessor.FlagsStatus.FlagCheckCarousel)
				{
					hedgProcessor.CounterCheckCarousel++;
					if(EDG_PROCESSOR_SENSOR_CAROUSEL())
					{
						hedgProcessor.CounterCheckCarousel = 0;
						hedgProcessor.FlagsStatus.FlagCheckCarousel = 1;

					}
					else if(hedgProcessor.CounterCheckCarousel >= EDG_PROCESSOR_MAX_CHECKS_ROTATING)
					{
						sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"ERROR POS 2\"");
						EDG_NEXTION_SendFrame(&hedgNextion);
						//what to do if carousel is not in the right position???
						hedgProcessor.CounterCheckCarousel = 0;
					}
				}
				else
				{
					hedgProcessor.CounterCheckCarousel++;
					if(!EDG_PROCESSOR_SENSOR_CAROUSEL())
					{
						EDG_PROCESSOR_RAISE_RELAY_INACTIVE();
						HAL_Delay(EDG_PROCESSOR_DELAY_TIME_MS);
						hedgProcessor.FlagsStatus.FlagCheckCarousel = 0;
						hedgProcessor.CounterCheckCarousel = 0;
						sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"CARRUSEL ARRIBA\"");
						EDG_NEXTION_SendFrame(&hedgNextion);
						hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CAROU_POS] = EDG_PROCESSOR_CAROUSEL_POS_UP;
						EDG_STATE_MACHINE_SaveCurrentProcess(); //Check if the save process is right!!
						hedgProcessor.ManualState = EDG_PROCESSOR_MANUAL_STATE_FINISH;
						hedgProcessor.FlagsStatus.FlagCarouselPos = 1;
						hedgProcessor.FlagsStatus.FlagTimComplete = 1;//Trick for jump next state


					}
					else if(hedgProcessor.CounterCheckCarousel >= EDG_PROCESSOR_MAX_CHECKS_ROTATING)
					{
						sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"ERROR POS 3\"");
						EDG_NEXTION_SendFrame(&hedgNextion);
						//what to do if carousel is not in the right position???
						hedgProcessor.CounterCheckCarousel = 0;
					}

				}

				break;

			case EDG_PROCESSOR_MANUAL_STATE_SHAKE:

				sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"AGITANDO\"");
				EDG_NEXTION_SendFrame(&hedgNextion);
				EDG_PROCESSOR_SHAKE_RELAY_ACTIVE();
				HAL_Delay(EDG_PROCESSOR_DELAY_TIME_MS);
				hedgProcessor.FlagsStatus.FlagShaking = 1;
				hedgProcessor.ManualState = EDG_PROCESSOR_MANUAL_STATE_FINISH;
				hedgProcessor.FlagsStatus.FlagTimComplete = 1;//Trick for jump next state

				break;

			case EDG_PROCESSOR_MANUAL_STATE_NO_SHAKE:

				sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"AGITADO DETENIDO\"");
				EDG_NEXTION_SendFrame(&hedgNextion);
				EDG_PROCESSOR_SHAKE_RELAY_INACTIVE();
				HAL_Delay(EDG_PROCESSOR_DELAY_TIME_MS);
				hedgProcessor.FlagsStatus.FlagShaking = 0;
				hedgProcessor.ManualState = EDG_PROCESSOR_MANUAL_STATE_FINISH;
				hedgProcessor.FlagsStatus.FlagTimComplete = 1;//Trick for jump next state

				break;

			case EDG_PROCESSOR_MANUAL_STATE_FINISH:

				sprintf((char *)hedgNextion.TxFrame,"vis bt0,1");
				EDG_NEXTION_SendFrame(&hedgNextion);
				sprintf((char *)hedgNextion.TxFrame,"vis b0,1");
				EDG_NEXTION_SendFrame(&hedgNextion);
				sprintf((char *)hedgNextion.TxFrame,"vis bt1,1");
				EDG_NEXTION_SendFrame(&hedgNextion);
				hedgProcessor.FlagsStatus.FlagManual = 0;
				EDG_PROCESSOR_StopTim(&hedgProcessor);
				EDG_NEXTION_EnableTouch(&hedgNextion);

				break;
		}
	}
	EDG_STATE_MACHINE_SetNextState(&hedgStateMachine, EDG_STATE_MACHINE_STATE_IDLE);
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
	if(hedgBuzzer.FlagsStatus.FlagSoundOn)
	{
		EDG_BUZZER_Sound(2, 2, 0, 1, 1);
	}
	EDG_NEXTION_GetAllDataReceived(&hedgNextion);
	HAL_Delay(50);

	switch(hedgNextion.CommandReceived)
	{
		case EDG_NEXTION_COMMAND_CHANGE_PAGE:

			EDG_STATE_MACHINE_ChangePage((EDG_NEXTION_PageTypeDef)hedgNextion.DataReceived[EDG_NEXTION_POS_PAGE]);

			break;

		case EDG_NEXTION_COMMAND_LOAD_PROGRAM_VALUES:

			EDG_STATE_MACHINE_LoadProgramValues(hedgNextion.DataReceived[EDG_NEXTION_POS_PROGRAM] - 1, hedgNextion.DataReceived[EDG_NEXTION_POS_SETTING_PROG]);//Minus 1 because data received is from 1 to 10

			break;

		case EDG_NEXTION_COMMAND_SET_DATE:

			EDG_STATE_MACHINE_SetDate();
			EDG_STATE_MACHINE_ShowDate();
			EDG_TIMER_SyncRTC(&hedgTimer, &hedgRTC);

			break;

		case EDG_NEXTION_COMMAND_PAUSE_PROCESS:

			hedgProcessor.LastState = hedgProcessor.CurrentState;
			hedgProcessor.CurrentState = EDG_PROCESSOR_STATE_PAUSE;
			hedgProcessor.FlagsStatus.FlagSetPause = 1;
			hedgProcessor.FlagsStatus.FlagTimComplete = 1; //Trick for jump next state
			NextState = EDG_STATE_MACHINE_STATE_PROCESS;

			break;

		case EDG_NEXTION_COMMAND_STOP_PROCESS:

			hedgProcessor.CurrentState = EDG_PROCESSOR_STATE_STOP;
			hedgProcessor.FlagsStatus.FlagTimComplete = 1; //Trick for jump next state
			NextState = EDG_STATE_MACHINE_STATE_PROCESS;

			break;

		case EDG_NEXTION_COMMAND_SET_TEMPERATURE:

			EDG_STATE_MACHINE_ChangeTemperature();

			break;

		case EDG_NEXTION_COMMAND_RUN_PROCESS:

			//all the values of the process are load into the array
			hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_PROGRAM] = hedgNextion.DataReceived[1] - 1; //Minus 1 because program comes from 1 to 10
			hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DELAY_HOUR] = hedgNextion.DataReceived[2];
			hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DELAY_MIN] = hedgNextion.DataReceived[3];
			hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DRIP_SECONDS] = hedgNextion.DataReceived[4];
			hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_T1_STATE] = hedgNextion.DataReceived[5];
			hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_T1_VALUE] = hedgNextion.DataReceived[6];
			hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_T2_STATE] = hedgNextion.DataReceived[7];
			hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_T2_VALUE] = hedgNextion.DataReceived[8];
			//The flag of set the process is set and the next state is load
			hedgProcessor.FlagsStatus.FlagSetRunning = 1;
			NextState = EDG_STATE_MACHINE_STATE_PROCESS;

			break;

		case EDG_NEXTION_COMMAND_SAVE_SCHEDULE:

			EDG_STATE_MACHINE_SaveScheduleValues(hedgNextion.DataReceived[EDG_NEXTION_POS_WEEK_DAY]);
			EDG_STATE_MACHINE_LoadScheduleValues();
			EDG_SCHEDULE_GetScheduleToday(&hedgSchedule, &hedgRTC);
			EDG_SCHEDULE_CheckActive(&hedgSchedule, &hedgRTC);

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

			hedgStateMachine.TempValuint8 = hedgNextion.DataReceived[1];
			hedgProcessor.FlagsStatus.FlagSetManual = 1;
			NextState = EDG_STATE_MACHINE_STATE_MANUAL;
			break;

		case EDG_NEXTION_COMMAND_SAVE_PROGRAM:

			EDG_STATE_MACHINE_SaveProgramValues(hedgNextion.DataReceived[EDG_NEXTION_POS_PROGRAM] - 1); //Minus 1 because data received is from 1 to 10

			break;

		case EDG_NEXTION_COMMAND_RESUME_PROCESS:

			hedgProcessor.CurrentState = EDG_PROCESSOR_STATE_RESUME;
			hedgProcessor.FlagsStatus.FlagTimComplete = 1; //Trick for jump next state
			NextState = EDG_STATE_MACHINE_STATE_PROCESS;

			break;

		case EDG_NEXTION_COMMAND_RESTART_MEMORY:

			EDG_STATE_MACHINE_CommandResetMemory();

			break;

		default:

			break;

	}

	EDG_NEXTION_SetCurrentBright(&hedgNextion);
	EDG_NEXTION_RestartBrightTim(&hedgNextion);
	EDG_NEXTION_StartReceiveFrame(&hedgNextion);
	EDG_NEXTION_EnableTouch(&hedgNextion);
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
		EDG_STATE_MACHINE_ChangePage(EDG_NEXTION_PAGE_ALARM);
	}

	EDG_BUZZER_Sound(10, 20, 100, 3, 0);
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

//	uint8_t Presence = 0;
//	uint16_t Temp_byte1 = 0;
//	uint16_t Temp_byte2 = 0;
	uint16_t TEMP = 0;
//	float Temperature = 0.0;
//	uint8_t Counter = 0;

//	while(1)
//	{
//		EDG_STATE_MACHINE_PrintDataLog();
//		sprintf((char *)stringDebug, ";");
//		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
//		for(Counter = 0; Counter < hedgDS18B20.ChipsToRead; Counter++)
//		{
//			EDG_DS18B20_ReadChipScratchpad(&hedgDS18B20, Counter);
//		}
//		for(Counter = 0; Counter < hedgDS18B20.ChipsToRead; Counter++)
//		{
//			sprintf((char *)stringDebug, "%3.02f;", hedgDS18B20.Chip[Counter].Temperature);
//			HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
//		}
//		for(Counter = 0; Counter < hedgDS18B20.ChipsToRead; Counter++)
//		{
//			sprintf((char *)stringDebug, "%01d;", (int)hedgDS18B20.Chip[Counter].ReadStatus);
//			HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
//		}
//		sprintf((char *)stringDebug, "\n");
//		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
//		HAL_Delay(1000);
//	}


//	while(1)
//	{
//			EDG_DS18B20_ReadAllChipsTemperature(&hedgDS18B20);
//			EDG_STATE_MACHINE_PrintDataLog();
//			sprintf((char *)stringDebug, ";%.02f;%.02f;%02d;%02d\n", hedgDS18B20.Chip[0].Temperature,
//																	 hedgDS18B20.Chip[1].Temperature,
//																	 hedgDS18B20.Chip[0].ChipStatus,
//																	 hedgDS18B20.Chip[1].ChipStatus);
//			HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
//			HAL_Delay(2000);
//	}

	while(1)
	{

		__HAL_TIM_SET_COUNTER(&EDG_DS18B20_TIM_BASE_HANDLER, 0);
		EDG_DS18B20_ChangePinOutput(&hedgDS18B20, 0);
		TEMP = __HAL_TIM_GET_COUNTER(&EDG_DS18B20_TIM_BASE_HANDLER);
		sprintf((char *)stringDebug, "Tiempo a output = %d\n", TEMP);
		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);

		__HAL_TIM_SET_COUNTER(&EDG_DS18B20_TIM_BASE_HANDLER, 0);
		EDG_DS18B20_ChangePinInput(&hedgDS18B20, 0);
		TEMP = __HAL_TIM_GET_COUNTER(&EDG_DS18B20_TIM_BASE_HANDLER);
		sprintf((char *)stringDebug, "Tiempo a input = %d\n", TEMP);
		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);

		HAL_Delay(1000);
	}

//	while(1)
//	{
//		for(counter = 0; counter < 2; counter++)
//		{
//
//
//		Presence = EDG_DS18B20_Start(&hedgDS18B20, counter);
//		EDG_DS18B20_WriteByte(&hedgDS18B20, counter, 0xCC);  // skip ROM
//		EDG_DS18B20_WriteByte(&hedgDS18B20, counter, 0x44);  // convert t
//
//		Presence = EDG_DS18B20_Start(&hedgDS18B20, counter);
//		EDG_DS18B20_WriteByte(&hedgDS18B20, counter, 0xCC);  // skip ROM
//		EDG_DS18B20_WriteByte(&hedgDS18B20, counter, 0xBE);  // Read Scratch-pad
//
//		Temp_byte1 = EDG_DS18B20_ReadByte(&hedgDS18B20, counter);
//		Temp_byte2 = EDG_DS18B20_ReadByte(&hedgDS18B20, counter);
//		TEMP = ((Temp_byte2<<8))|Temp_byte1;
//
//		Temperature = (float)((TEMP & 0x7FF) * 0.0625);
//		if(((TEMP >> 11) & 0x001F) == 0x001F)
//		{
//			Temperature *= -1;
//		}
//		//Temperature = (float)TEMP/16.0;  // resolution is 0.0625
//
//		EDG_RTC_GetDate(&hedgRTC, EDG_RTC_ADDRESS);
//		sprintf((char *)stringDebug, "[%02d:%02d:%02d-%02d/%02d/%02d]Temperatura Chip %d = %.02f, Presence = %d, %02X %02X\n", hedgRTC.CurrentDate.Hour,
//																							  hedgRTC.CurrentDate.Minute,
//																							  hedgRTC.CurrentDate.Second,
//																							  hedgRTC.CurrentDate.Day,
//																							  hedgRTC.CurrentDate.Month,
//																							  hedgRTC.CurrentDate.Year,
//																							  counter + 1,
//																							  Temperature,
//																							  Presence,
//																							  Temp_byte1,
//																							  Temp_byte2);
//		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
//
//		}
//		HAL_Delay(1000);
//	}


	/*
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


	}*/

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

			case EDG_AC_CONTROL_SENSOR_STATUS_ERROR:

				/*** If sensor is in error led is BLUE ***/
				EDG_WS2812_Pixel(&hedgWs2812, (hedgAccontrol.UnitsQty - hedgAccontrol.UnitsCounter - 1) , 0, 0, 50);

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

	//EDG_DS18B20_ReadAllChipsTemperature(&hedgDS18B20);

#if EDG_STATE_MACHINE_DEBUG_STATE == EDG_STATE_MACHINE_DEBUG_ACTIVE

	EDG_STATE_MACHINE_PrintDataLog();
	sprintf((char *)stringDebug, ";");
	HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
	for(Counter = 0; Counter < hedgDS18B20.ChipsToRead; Counter++)
	{
		sprintf((char *)stringDebug, "%3.02f;", hedgDS18B20.Chip[Counter].Temperature);
		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
	}
	for(Counter = 0; Counter < hedgAccontrol.UnitsQty; Counter++)
	{
		sprintf((char *)stringDebug, "%3.02f;", hedgAccontrol.Units[Counter].Pid.SetPoint);
		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
	}
	for(Counter = 0; Counter < hedgDS18B20.ChipsToRead; Counter++)
	{
		sprintf((char *)stringDebug, "%01d;", (int)hedgDS18B20.Chip[Counter].ReadStatus);
		HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
	}
	sprintf((char *)stringDebug, "\n");
	HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);
#endif
	return;

}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_ChangePage(EDG_NEXTION_PageTypeDef page)
{
	uint8_t counter = 0;

	EDG_NEXTION_ChangePage(&hedgNextion, page);
	switch(hedgNextion.CurrentPage)
	{

		case EDG_NEXTION_PAGE_EXECUTE:

			EDG_STATE_MACHINE_LoadProgramValues(0, 0);
			EDG_STATE_MACHINE_RefreshExecutePage();

			break;

		case EDG_NEXTION_PAGE_PROGRAM:

			EDG_STATE_MACHINE_LoadProgramValues(0, 1);

			break;

		case EDG_NEXTION_PAGE_SCHEDULE:

			EDG_STATE_MACHINE_LoadScheduleValues();
			EDG_STATE_MACHINE_CheckButtonSchedule();

			break;

		case EDG_NEXTION_PAGE_MANUAL:

			sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"PRESIONE UN BOTON\"");
			EDG_NEXTION_SendFrame(&hedgNextion);
			if(hedgProcessor.FlagsStatus.FlagCarouselPos)
			{
				sprintf((char *)hedgNextion.TxFrame,"bt0.val=1");
				EDG_NEXTION_SendFrame(&hedgNextion);
			}
			if(hedgProcessor.FlagsStatus.FlagShaking)
			{
				sprintf((char *)hedgNextion.TxFrame,"bt1.val=1");
				EDG_NEXTION_SendFrame(&hedgNextion);
			}

			break;

		case EDG_NEXTION_PAGE_MENU:

			if((hedgRTC.CurrentState != EDG_RTC_STATE_OK) && (hedgAccontrol.SensorsInAlarm != 0))
			{
				sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"        ERROR EN EL RELOJ Y EN                 UNO O VARIOS SENSORES...\"");
			}
			else if(hedgRTC.CurrentState != EDG_RTC_STATE_OK)
			{
				sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"ERROR EN EL RELOJ...\"");

			}
			else if((hedgRTC.DateState == EDG_RTC_DATE_STATE_UNCONFIGURED) && (hedgAccontrol.SensorsInAlarm != 0))
			{
				sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"     RELOJ DESCONFIGURADO Y ERROR EN            UNO O VARIOS SENSORES...\"");
			}
			else if(hedgRTC.DateState == EDG_RTC_DATE_STATE_UNCONFIGURED)
			{
				sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"RELOJ DESCONFIGURADO...\"");
			}
			else if(hedgAccontrol.SensorsInAlarm != 0)
			{
				sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"ERROR EN UNO O VARIOS SENSORES...\"");
			}
			else
			{
				sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"PROCESADOR EN FUNCIONAMIENTO NORMAL\"");
			}

			EDG_NEXTION_SendFrame(&hedgNextion);
			EDG_STATE_MACHINE_CheckButtonBuzzer();

			break;

		case EDG_NEXTION_PAGE_OFFSET:

			EDG_STATE_MACHINE_LoadOffsetValues();

			break;

		case EDG_NEXTION_PAGE_ALARM:

			for(counter = 0; counter < hedgAccontrol.UnitsQty; counter++)
			{
				if(hedgAccontrol.UnitsInAlarm & (1U << counter))
				{
					sprintf((char *)hedgNextion.TxFrame,"tx%d.txt=\"VASO %d\"", counter, counter + 1);
					EDG_NEXTION_SendFrame(&hedgNextion);
				}
			}
			sprintf((char *)hedgNextion.TxFrame,"t50.txt=\" EN TEMPERATURA ALTA, ESPERE UN MOMENTO O APAGUE   EL EQUIPO\"");
			EDG_NEXTION_SendFrame(&hedgNextion);

			break;

		default:

			break;

	}
	EDG_STATE_MACHINE_ShowDate();

	return;
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
		sprintf((char *)hedgNextion.TxFrame,"t5.txt=\"HORA GUARDADA CORRECTAMENTE!!!\"");
	}
	else
	{
		sprintf((char *)hedgNextion.TxFrame,"t5.txt=\"ERROR AL GUARDAR HORA...\"");
	}
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"timReloj.en=1");
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
void EDG_STATE_MACHINE_ChangeTemperature(void)
{

	if(hedgNextion.DataReceived[EDG_STATE_MACHINE_CHANGE_TEMP_POS_ACTIVE] == 1)
	{
		hedgAccontrol.Units[hedgNextion.DataReceived[EDG_STATE_MACHINE_CHANGE_TEMP_POS_UNIT]].ControlStatus = EDG_AC_CONTROL_CONTROL_STATUS_ACTIVE;
		hedgAccontrol.Units[hedgNextion.DataReceived[EDG_STATE_MACHINE_CHANGE_TEMP_POS_UNIT]].Pid.SetPoint = (float)hedgNextion.DataReceived[EDG_STATE_MACHINE_CHANGE_TEMP_POS_VALUE];
	}
	else
	{
		hedgAccontrol.Units[hedgNextion.DataReceived[EDG_STATE_MACHINE_CHANGE_TEMP_POS_UNIT]].ControlStatus = EDG_AC_CONTROL_CONTROL_STATUS_INACTIVE;
		EDG_AC_CONTROL_StopPWMOutput(&hedgAccontrol, hedgNextion.DataReceived[EDG_STATE_MACHINE_CHANGE_TEMP_POS_UNIT]);
		hedgAccontrol.Units[hedgNextion.DataReceived[EDG_STATE_MACHINE_CHANGE_TEMP_POS_UNIT]].PwmStatus = EDG_AC_CONTROL_PWM_STATUS_INACTIVE;
		hedgAccontrol.Units[hedgNextion.DataReceived[EDG_STATE_MACHINE_CHANGE_TEMP_POS_UNIT]].Pid.CurrentValue = 0;
	}

	if(hedgProcessor.FlagsStatus.FlagRunning)
	{
		switch(hedgNextion.DataReceived[EDG_STATE_MACHINE_CHANGE_TEMP_POS_UNIT])
		{
			case 0:
			hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_T1_STATE] = hedgNextion.DataReceived[EDG_STATE_MACHINE_CHANGE_TEMP_POS_ACTIVE];
			hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_T1_VALUE] = hedgNextion.DataReceived[EDG_STATE_MACHINE_CHANGE_TEMP_POS_VALUE];
			break;

			case 1:
			hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_T2_STATE] = hedgNextion.DataReceived[EDG_STATE_MACHINE_CHANGE_TEMP_POS_ACTIVE];
			hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_T2_VALUE] = hedgNextion.DataReceived[EDG_STATE_MACHINE_CHANGE_TEMP_POS_VALUE];
			break;

		}
		EDG_STATE_MACHINE_SaveCurrentProcess();
	}

	return;

}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_CheckMemory(void)
{
	uint8_t Temp = 0;


	if(EDG_MEMORY_ReadMemory(EDG_MEMORY_ADDRESS_MEM1,
						  	 EDG_MEM_ADDR_BASE_PROGRAM,
							 &Temp,
							 1) == EDG_MEMORY_STATE_OK)
	{
		if(Temp == 0xFF)
		{
			EDG_STATE_MACHINE_ResetProgramMemory();
		}

	}
	if(EDG_MEMORY_ReadMemory(EDG_MEMORY_ADDRESS_MEM1,
						  	 EDG_MEM_ADDR_BASE_SCHEDULE,
							 &Temp,
							 1) == EDG_MEMORY_STATE_OK)
	{
		if(Temp == 0xFF)
		{
			EDG_STATE_MACHINE_ResetScheduleMemory();
		}
	}
	if(EDG_MEMORY_ReadMemory(EDG_MEMORY_ADDRESS_MEM1,
							 EDG_MEM_ADDR_BASE_OFFSET,
							 &Temp,
							 1) == EDG_MEMORY_STATE_OK)
	{
		if(Temp == 0xFF)
		{
			EDG_STATE_MACHINE_ResetOffsetMemory();
		}
	}
	if(EDG_MEMORY_ReadMemory(EDG_MEMORY_ADDRESS_MEM1,
							 EDG_MEM_ADDR_BASE_CURR_PROC,
							 &Temp,
							 1) == EDG_MEMORY_STATE_OK)
	{
		if(Temp == 0xFF)
		{
			EDG_STATE_MACHINE_ResetCurrentProcessMemory();
		}
	}
	return;

}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_ResetProgramMemory(void)
{
	//Array with default program values, 12 containers active, 0 hours 15 minutes for each containers, 60 seconds for drip, T1 inactive, 60° t1, T2 inactive, 60° t2
	uint8_t Temp[EDG_MEM_ADDR_VALUES_X_PROGRAM] = {12, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 60, 0, 60, 0, 60};
	uint8_t Counter;

	for(Counter = 0; Counter < EDG_MEM_ADDR_PROGRAM_QTY; Counter++)
	{
		EDG_MEMORY_WriteMemory(EDG_MEMORY_ADDRESS_MEM1,
							   (EDG_MEM_ADDR_BASE_PROGRAM + (EDG_MEM_ADDR_PROGRAM_OFFSET * Counter)),
							   Temp,
							   EDG_MEM_ADDR_VALUES_X_PROGRAM);
	}
	return;

}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_ResetScheduleMemory(void)
{
	//Array with default schedule values, inactive 0, hour 8, minutes 0, am 0, program 1
	uint8_t Temp[EDG_MEM_ADDR_VALUES_X_SCHEDULE] = {0, 8, 0, 0, 1};
	uint8_t Counter;

	for(Counter = 0; Counter < EDG_MEM_ADDR_SCHEDULE_QTY; Counter++)
	{
		EDG_MEMORY_WriteMemory(EDG_MEMORY_ADDRESS_MEM1,
							   (EDG_MEM_ADDR_BASE_SCHEDULE + (EDG_MEM_ADDR_SCHEDULE_OFFSET * Counter)),
							   Temp,
							   EDG_MEM_ADDR_VALUES_X_SCHEDULE);
	}
	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_ResetOffsetMemory(void)
{
	//Array with default offset values 5
	uint8_t Temp[EDG_MEM_ADDR_VALUES_X_OFFSET] = {0, 0, 0, 0, 0, 0};
	uint8_t Counter;

	for(Counter = 0; Counter < EDG_MEM_ADDR_OFFSET_QTY; Counter++)
	{
		EDG_MEMORY_WriteMemory(EDG_MEMORY_ADDRESS_MEM1,
							   (EDG_MEM_ADDR_BASE_OFFSET + (EDG_MEM_ADDR_OFFSET_OFFSET * Counter)),
							   Temp,
							   EDG_MEM_ADDR_VALUES_X_OFFSET);
	}
	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_ResetCurrentProcessMemory(void)
{

	uint8_t Temp[EDG_MEM_ADDR_VALUES_X_CURRENT_PROC] = {0};
	uint8_t Counter;

	for(Counter = 0; Counter < EDG_MEM_ADDR_CURR_PROC_QTY; Counter++)
	{
		EDG_MEMORY_WriteMemory(EDG_MEMORY_ADDRESS_MEM1,
							   (EDG_MEM_ADDR_BASE_CURR_PROC + (EDG_MEM_ADDR_CURR_PROC_OFFSET * Counter)),
							   Temp,
							   EDG_MEM_ADDR_VALUES_X_CURRENT_PROC);
	}
	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_CommandResetMemory(void)
{
	EDG_STATE_MACHINE_ResetProgramMemory();
	EDG_STATE_MACHINE_ResetScheduleMemory();
	EDG_STATE_MACHINE_ResetCurrentProcessMemory();
	EDG_STATE_MACHINE_ResetOffsetMemory();
	EDG_STATE_MACHINE_LoadOffsetValues();
	sprintf((char *)hedgNextion.TxFrame,"t5.txt=\"MEMORIA REINICIADA!!!\"");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"timReloj.en=1");
	EDG_NEXTION_SendFrame(&hedgNextion);
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
		sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"   PROGRAMA       GUARDADO!!!\"");
	}
	else
	{
		sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"ERROR AL GUARDAR EL PROGRAMA...\"");
	}

	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"tm0.en=1");
	EDG_NEXTION_SendFrame(&hedgNextion);

	return;
}

/**
  * @brief
  * @param    values = 0 if is from execute page
  * 				   1 if is from set program page
  * @retval
  */
void EDG_STATE_MACHINE_LoadProgramValues(uint32_t Program, uint8_t values)
{

	uint8_t Temp[EDG_MEM_ADDR_VALUES_X_PROGRAM] = {0};
	uint8_t Counter;

	if(EDG_MEMORY_ReadMemory(EDG_MEMORY_ADDRESS_MEM1,
						  	 (EDG_MEM_ADDR_BASE_PROGRAM + (EDG_MEM_ADDR_PROGRAM_OFFSET * Program)),
							 Temp,
							 EDG_MEM_ADDR_VALUES_X_PROGRAM) != EDG_MEMORY_STATE_OK)
	{
		sprintf((char *)hedgNextion.TxFrame,"t0.txt=\"   ERROR      CARGANDO\"");
		EDG_NEXTION_SendFrame(&hedgNextion);
		return;
	}

	//Load minutes and seconds
	for(Counter = 0; Counter < 24; Counter++)
	{
		sprintf((char *)hedgNextion.TxFrame,"n%d.val=%d", Counter, Temp[Counter+1]);
		EDG_NEXTION_SendFrame(&hedgNextion);
	}
	if(values == 1)
	{
		for(Counter = 0; Counter < 12; Counter++)
		{
			sprintf((char *)hedgNextion.TxFrame,"h%d.val=%d", (Counter + 1), Temp[(Counter * 2)+1]);
			EDG_NEXTION_SendFrame(&hedgNextion);
			sprintf((char *)hedgNextion.TxFrame,"m%d.val=%d", (Counter + 1), Temp[(Counter * 2)+2]);
			EDG_NEXTION_SendFrame(&hedgNextion);
		}
	}
	sprintf((char *)hedgNextion.TxFrame,"n31.val=%d", Temp[EDG_NEXTION_POS_DRIP_SECONDS]);
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"escu.val=%d", Temp[EDG_NEXTION_POS_DRIP_SECONDS]);
	EDG_NEXTION_SendFrame(&hedgNextion);

	sprintf((char *)hedgNextion.TxFrame,"bt20.val=%d", Temp[EDG_NEXTION_POS_T1_STATE]);
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"n25.val=%d", Temp[EDG_NEXTION_POS_T1_VALUE]);
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"temp1.val=%d", Temp[EDG_NEXTION_POS_T1_VALUE]);
	EDG_NEXTION_SendFrame(&hedgNextion);

	sprintf((char *)hedgNextion.TxFrame,"bt21.val=%d", Temp[EDG_NEXTION_POS_T2_STATE]);
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"n26.val=%d", Temp[EDG_NEXTION_POS_T2_VALUE]);
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"temp2.val=%d", Temp[EDG_NEXTION_POS_T2_VALUE]);
	EDG_NEXTION_SendFrame(&hedgNextion);


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
			sprintf((char *)hedgNextion.TxFrame,"t39.txt=\"   DIA CONFIGURADO        COMO ACTIVO!!!\"");
		}
		else
		{
			sprintf((char *)hedgNextion.TxFrame,"t39.txt=\"   ERROR AL ACTIVAR         EL DIA...\"");
		}


	}
	else
	{

		if(EDG_MEMORY_WriteMemory(EDG_MEMORY_ADDRESS_MEM1,
								  (EDG_MEM_ADDR_BASE_SCHEDULE+ (EDG_MEM_ADDR_SCHEDULE_OFFSET * Day)),
								  &Temp[0],
								  1) == EDG_MEMORY_STATE_OK)
		{
			sprintf((char *)hedgNextion.TxFrame,"t39.txt=\"   DIA CONFIGURADO       COMO INACTIVO!!!\"");
		}
		else
		{
			sprintf((char *)hedgNextion.TxFrame,"t39.txt=\"  ERROR AL INACTIVAR         EL DIA...\"");
		}
	}

	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"timCalen.en=1");
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
			sprintf((char *)hedgNextion.TxFrame,"t39.txt=\"    ERROR AL LEER        EL CALENDARIO...\"");
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

		sprintf((char *)hedgNextion.TxFrame,"pg%d.txt=\"%d\"", Counter, Temp[4]);
		EDG_NEXTION_SendFrame(&hedgNextion);

	}

	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_ClearScheduleValues(void)
{

	uint8_t Temp[EDG_MEM_ADDR_VALUES_X_SCHEDULE] = {0};

	if(EDG_MEMORY_ReadMemory(EDG_MEMORY_ADDRESS_MEM1,
							 (EDG_MEM_ADDR_BASE_SCHEDULE+ (EDG_MEM_ADDR_SCHEDULE_OFFSET * hedgSchedule.CurrentWeekDay)),
							 Temp,
							 EDG_MEM_ADDR_VALUES_X_SCHEDULE) != EDG_MEMORY_STATE_OK)
	{

		return;
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

	uint8_t Temp[EDG_MEM_ADDR_VALUES_X_OFFSET] = {0};
	uint8_t Counter;

	for(Counter = 0; Counter < EDG_MEM_ADDR_VALUES_X_OFFSET; Counter++)
	{
		Temp[Counter] = (uint8_t)hedgNextion.DataReceived[Counter+1];
	}

	if(EDG_MEMORY_WriteMemory(EDG_MEMORY_ADDRESS_MEM1,
							  EDG_MEM_ADDR_BASE_OFFSET,
							  Temp,
							  EDG_MEM_ADDR_VALUES_X_OFFSET) == EDG_MEMORY_STATE_OK)
	{
		sprintf((char *)hedgNextion.TxFrame,"t5.txt=\"VALORES DE OFFSET GUARDADOS!!!\"");
	}
	else
	{
		sprintf((char *)hedgNextion.TxFrame,"t5.txt=\"     ERROR AL GUARDAR LOS           VALORES DE OFFSET...\"");
	}
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"timReloj.en=1");
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

	uint8_t Temp[EDG_MEM_ADDR_VALUES_X_OFFSET] = {0};
	uint8_t Counter;

	if(EDG_MEMORY_ReadMemory(EDG_MEMORY_ADDRESS_MEM1,
						     EDG_MEM_ADDR_BASE_OFFSET,
							 Temp,
							 EDG_MEM_ADDR_VALUES_X_OFFSET) != EDG_MEMORY_STATE_OK)
	{
		sprintf((char *)hedgNextion.TxFrame,"t5.txt=\"     ERROR AL CARGAR LOS            VALORES DE OFFSET...\"");
		EDG_NEXTION_SendFrame(&hedgNextion);
		return;
	}
	else
	{
		for(Counter = 0; Counter < EDG_MEM_ADDR_VALUES_X_OFFSET; Counter++)
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
		hedgBuzzer.FlagsStatus.FlagSoundOn = 0;
	}
	else
	{
		hedgBuzzer.FlagsStatus.FlagSoundOn = 1;
		EDG_BUZZER_Sound(2, 2, 0, 1, 1);
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
		sprintf((char *)hedgNextion.TxFrame,"t39.txt=\"    ERROR AL LEER        EL CALENDARIO...\"");
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

	if(!hedgBuzzer.FlagsStatus.FlagSoundOn)
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
uint8_t EDG_STATE_MACHINE_SaveCurrentProcess(void)
{
	if(EDG_MEMORY_WriteMemory(EDG_MEMORY_ADDRESS_MEM1,
						      (EDG_MEM_ADDR_BASE_CURR_PROC),
							  hedgProcessor.CurrentProcess,
							  EDG_MEM_ADDR_VALUES_X_CURRENT_PROC) != EDG_MEMORY_STATE_OK)
	{
		return 0;
	}else
	{
		return 1;
	}
}

/**
  * @brief
  * @param
  * @retval
  */
uint8_t EDG_STATE_MACHINE_CheckActiveProcess(void)
{
	if(EDG_MEMORY_ReadMemory(EDG_MEMORY_ADDRESS_MEM1,
						     (EDG_MEM_ADDR_BASE_CURR_PROC),
							 hedgProcessor.CurrentProcess,
							 EDG_MEM_ADDR_VALUES_X_CURRENT_PROC) != EDG_MEMORY_STATE_OK)
	{
		return 0;
	}else
	{
		return hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_IS_ACTIVE];
	}
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_ResumeActiveProcess(void)
{

	uint8_t Counter;

	EDG_NEXTION_ChangePage(&hedgNextion, EDG_NEXTION_PAGE_EXECUTE);
	EDG_STATE_MACHINE_ShowDate();

	sprintf((char *)hedgNextion.TxFrame,"n24.val=%d", hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_PROGRAM] + 1);
	EDG_NEXTION_SendFrame(&hedgNextion);

	//Load minutes and seconds
	for(Counter = 0; Counter < 24; Counter++)
	{
		sprintf((char *)hedgNextion.TxFrame,"n%d.val=%d", Counter, hedgProcessor.CurrentProcess[Counter+1]);
		EDG_NEXTION_SendFrame(&hedgNextion);
	}

	sprintf((char *)hedgNextion.TxFrame,"n31.val=%d", hedgProcessor.CurrentProcess[EDG_NEXTION_POS_DRIP_SECONDS]);
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"escu.val=%d", hedgProcessor.CurrentProcess[EDG_NEXTION_POS_DRIP_SECONDS]);
	EDG_NEXTION_SendFrame(&hedgNextion);

	printf((char *)hedgNextion.TxFrame,"n27.val=%d", hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DELAY_HOUR]);
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"n28.val=%d", hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_DELAY_MIN]);
	EDG_NEXTION_SendFrame(&hedgNextion);

	sprintf((char *)hedgNextion.TxFrame,"bt20.val=%d", hedgProcessor.CurrentProcess[EDG_NEXTION_POS_T1_STATE]);
	EDG_NEXTION_SendFrame(&hedgNextion);
	if(hedgProcessor.CurrentProcess[EDG_NEXTION_POS_T1_STATE])
	{
		sprintf((char *)hedgNextion.TxFrame,"vis b2,0");
		EDG_NEXTION_SendFrame(&hedgNextion);
		sprintf((char *)hedgNextion.TxFrame,"vis b3,0");
		EDG_NEXTION_SendFrame(&hedgNextion);
	}
	sprintf((char *)hedgNextion.TxFrame,"n25.val=%d", hedgProcessor.CurrentProcess[EDG_NEXTION_POS_T1_VALUE]);
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"temp1.val=%d", hedgProcessor.CurrentProcess[EDG_NEXTION_POS_T1_VALUE]);
	EDG_NEXTION_SendFrame(&hedgNextion);

	sprintf((char *)hedgNextion.TxFrame,"bt21.val=%d", hedgProcessor.CurrentProcess[EDG_NEXTION_POS_T2_STATE]);
	EDG_NEXTION_SendFrame(&hedgNextion);
	if(hedgProcessor.CurrentProcess[EDG_NEXTION_POS_T2_STATE])
	{
		sprintf((char *)hedgNextion.TxFrame,"vis b5,0");
		EDG_NEXTION_SendFrame(&hedgNextion);
		sprintf((char *)hedgNextion.TxFrame,"vis b7,0");
		EDG_NEXTION_SendFrame(&hedgNextion);
	}
	sprintf((char *)hedgNextion.TxFrame,"n26.val=%d", hedgProcessor.CurrentProcess[EDG_NEXTION_POS_T2_VALUE]);
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"temp2.val=%d", hedgProcessor.CurrentProcess[EDG_NEXTION_POS_T2_VALUE]);
	EDG_NEXTION_SendFrame(&hedgNextion);

	sprintf((char *)hedgNextion.TxFrame,"vis q1,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	if(hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CONTAINER] == hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_ACTIVE_CONTAINERS])
	{
		sprintf((char *)hedgNextion.TxFrame,"vis q%d,1", hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CONTAINER]);
	}
	else
	{
		sprintf((char *)hedgNextion.TxFrame,"vis q%d,1", hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_CONTAINER] + 1);
	}

	EDG_NEXTION_SendFrame(&hedgNextion);

	sprintf((char *)hedgNextion.TxFrame,"vis b4,0");
	EDG_NEXTION_SendFrame(&hedgNextion);

	sprintf((char *)hedgNextion.TxFrame,"vis bt0,1");
	EDG_NEXTION_SendFrame(&hedgNextion);

	sprintf((char *)hedgNextion.TxFrame,"bt14.val=1");
	EDG_NEXTION_SendFrame(&hedgNextion);

	sprintf((char *)hedgNextion.TxFrame,"vis b0,0");
	EDG_NEXTION_SendFrame(&hedgNextion);

	sprintf((char *)hedgNextion.TxFrame,"vis b8,0");
	EDG_NEXTION_SendFrame(&hedgNextion);

	sprintf((char *)hedgNextion.TxFrame,"vis b9,0");
	EDG_NEXTION_SendFrame(&hedgNextion);

	sprintf((char *)hedgNextion.TxFrame,"vis b10,0");
	EDG_NEXTION_SendFrame(&hedgNextion);

	sprintf((char *)hedgNextion.TxFrame,"vis b11,0");
	EDG_NEXTION_SendFrame(&hedgNextion);

	sprintf((char *)hedgNextion.TxFrame,"vis b12,0");
	EDG_NEXTION_SendFrame(&hedgNextion);

	sprintf((char *)hedgNextion.TxFrame,"vis b6,0");
	EDG_NEXTION_SendFrame(&hedgNextion);

	sprintf((char *)hedgNextion.TxFrame,"vis p0,1");
	EDG_NEXTION_SendFrame(&hedgNextion);

	hedgProcessor.FlagsStatus.FlagSetRunning = 1;
	hedgProcessor.FlagsStatus.FlagResume = 1;

	EDG_STATE_MACHINE_SetNextState(&hedgStateMachine, EDG_STATE_MACHINE_STATE_PROCESS);

	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_RefreshExecutePage(void)
{
	sprintf((char *)hedgNextion.TxFrame,"n29.val=%.00f", hedgDS18B20.Chip[0].Temperature);
	EDG_NEXTION_SendFrame(&hedgNextion);

	sprintf((char *)hedgNextion.TxFrame,"n30.val=%.00f", hedgDS18B20.Chip[1].Temperature);
	EDG_NEXTION_SendFrame(&hedgNextion);
	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_SetTemperatureProcess(void)
{
	//Set the value and the state of the temperature process
	if(hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_T1_STATE] == 1)
	{
		hedgAccontrol.Units[0].ControlStatus = EDG_AC_CONTROL_CONTROL_STATUS_ACTIVE;
		hedgAccontrol.Units[0].Pid.SetPoint = (float)hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_T1_VALUE];
		EDG_AC_CONTROL_ResetPidValues(&hedgAccontrol.Units[0].Pid);
		sprintf((char *)hedgNextion.TxFrame,"vis b2,0");
		EDG_NEXTION_SendFrame(&hedgNextion);
		sprintf((char *)hedgNextion.TxFrame,"vis b3,0");
		EDG_NEXTION_SendFrame(&hedgNextion);
		sprintf((char *)hedgNextion.TxFrame,"vis bt20.val=1");
		EDG_NEXTION_SendFrame(&hedgNextion);
	}
	else
	{
		hedgAccontrol.Units[0].ControlStatus = EDG_AC_CONTROL_CONTROL_STATUS_INACTIVE;
		EDG_AC_CONTROL_StopPWMOutput(&hedgAccontrol, 0);
		hedgAccontrol.Units[0].PwmStatus = EDG_AC_CONTROL_PWM_STATUS_INACTIVE;
		hedgAccontrol.Units[0].Pid.CurrentValue = 0;
		sprintf((char *)hedgNextion.TxFrame,"vis b2,1");
		EDG_NEXTION_SendFrame(&hedgNextion);
		sprintf((char *)hedgNextion.TxFrame,"vis b3,1");
		EDG_NEXTION_SendFrame(&hedgNextion);
		sprintf((char *)hedgNextion.TxFrame,"vis bt20.val=0");
		EDG_NEXTION_SendFrame(&hedgNextion);
	}

	if(hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_T2_STATE] == 1)
	{
		hedgAccontrol.Units[1].ControlStatus = EDG_AC_CONTROL_CONTROL_STATUS_ACTIVE;
		hedgAccontrol.Units[1].Pid.SetPoint = (float)hedgProcessor.CurrentProcess[EDG_PROCESSOR_ARR_POS_CURR_T2_VALUE];
		sprintf((char *)hedgNextion.TxFrame,"vis b5,0");
		EDG_NEXTION_SendFrame(&hedgNextion);
		sprintf((char *)hedgNextion.TxFrame,"vis b7,0");
		EDG_NEXTION_SendFrame(&hedgNextion);
		sprintf((char *)hedgNextion.TxFrame,"vis bt21.val=1");
		EDG_NEXTION_SendFrame(&hedgNextion);
		EDG_AC_CONTROL_ResetPidValues(&hedgAccontrol.Units[1].Pid);
	}
	else
	{
		hedgAccontrol.Units[1].ControlStatus = EDG_AC_CONTROL_CONTROL_STATUS_INACTIVE;
		EDG_AC_CONTROL_StopPWMOutput(&hedgAccontrol, 1);
		hedgAccontrol.Units[1].PwmStatus = EDG_AC_CONTROL_PWM_STATUS_INACTIVE;
		hedgAccontrol.Units[1].Pid.CurrentValue = 0;
		sprintf((char *)hedgNextion.TxFrame,"vis b5,1");
		EDG_NEXTION_SendFrame(&hedgNextion);
		sprintf((char *)hedgNextion.TxFrame,"vis b7,1");
		EDG_NEXTION_SendFrame(&hedgNextion);
		sprintf((char *)hedgNextion.TxFrame,"vis bt21.val=0");
		EDG_NEXTION_SendFrame(&hedgNextion);
	}
	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_HideButtonsExecute(void)
{
	sprintf((char *)hedgNextion.TxFrame,"vis b4,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis b0,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis b8,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis b9,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis b10,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis b11,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis b12,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis b6,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis p0,1");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis bt0,1");
	EDG_NEXTION_SendFrame(&hedgNextion);
	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_ShowButtonsExecute(void)
{
	sprintf((char *)hedgNextion.TxFrame,"vis b0,1");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis b8,1");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis b9,1");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis b10,1");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis b11,1");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis b12,1");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis b4,1");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis q1,1");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis q2,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis q3,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis q4,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis q5,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis q6,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis q7,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis q8,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis q9,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis q10,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis q11,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis q12,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis bt0,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis p0,0");
	EDG_NEXTION_SendFrame(&hedgNextion);
	sprintf((char *)hedgNextion.TxFrame,"vis b6,1");
	EDG_NEXTION_SendFrame(&hedgNextion);
	return;
}

/**
  * @brief
  * @param
  * @retval
  */
void EDG_STATE_MACHINE_PrintDataLog(void)
{

#if EDG_STATE_MACHINE_DEBUG_STATE == EDG_STATE_MACHINE_DEBUG_ACTIVE

	EDG_RTC_GetDate(&hedgRTC, EDG_RTC_ADDRESS);
	sprintf((char *)stringDebug, "[%02d:%02d:%02d-%02d/%02d/%02d]",  hedgRTC.CurrentDate.Hour,
																	 hedgRTC.CurrentDate.Minute,
																	 hedgRTC.CurrentDate.Second,
																	 hedgRTC.CurrentDate.Day,
																	 hedgRTC.CurrentDate.Month,
																	 hedgRTC.CurrentDate.Year);
	HAL_UART_Transmit(&EDG_STATE_MACHINE_DEBUG_PORT_HANDLE, stringDebug, strlen((const char*)stringDebug), 100);

#endif
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

