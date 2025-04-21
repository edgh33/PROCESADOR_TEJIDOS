/*
 * EDG_NEXTION.h
 *
 *  Created on: 08/04/2023
 *      Author: Elkin Granados
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EDG_NEXTION_H_
#define EDG_NEXTION_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "EDG_CONFIG.h"

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

#include "main.h"
#include "usart.h"
#include "tim.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#endif

#include "EDG_RTC.h"

/* Include the headers files related with uart callbacks  ----------------------*/

#define EDG_NEXTION_USART_PORT			USART1
#define EDG_NEXTION_USART_PORT_HANDLE	huart1

#define EDG_NEXTION_TIM_BRIGHT			TIM7
#define EDG_NEXTION_TIM_BRIGHT_HANDLE	htim7

#define EDG_NEXTION_BRIGHT_LEVEL_NORMAL	(100)
#define EDG_NEXTION_BRIGHT_LEVEL_LOW	(10)

#define EDG_NEXTION_FRAME_SIZE 			(300)
#define EDG_NEXTION_RX_TIME_OUT_MS		(10)
#define EDG_NEXTION_TX_TIME_OUT_MS		(100)
#define EDG_NEXTION_DELAY_MS			(5)
#define	EDG_NEXTION_MAX_DATA_RECEIVED   (40)
#define EDG_NEXTION_FRAME_START_CHAR	('#')

//Common array positions
#define EDG_NEXTION_POS_COMMAND			(0)
#define EDG_NEXTION_POS_PAGE			(1)
#define EDG_NEXTION_POS_WEEK_DAY		(1)
#define EDG_NEXTION_POS_PROGRAM			(1)
#define EDG_NEXTION_POS_SETTING_PROG	(2)
#define EDG_NEXTION_POS_DRIP_SECONDS	(25)
#define EDG_NEXTION_POS_T1_STATE		(26)
#define EDG_NEXTION_POS_T1_VALUE		(27)
#define EDG_NEXTION_POS_T2_STATE		(28)
#define EDG_NEXTION_POS_T2_VALUE		(29)


/**
  * @brief
  */
typedef enum
{

	EDG_NEXTION_STATUS_OK,
	EDG_NEXTION_STATUS_ERROR,

}EDG_NEXTION_StatusTypeDef;

/**
  * @brief
  */
typedef enum
{

	EDG_NEXTION_PAGE_START,
	EDG_NEXTION_PAGE_MENU,
	EDG_NEXTION_PAGE_EXECUTE,
	EDG_NEXTION_PAGE_MANUAL,
	EDG_NEXTION_PAGE_CLOCK,
	EDG_NEXTION_PAGE_SCHEDULE,
	EDG_NEXTION_PAGE_PROGRAM,
	EDG_NEXTION_PAGE_OFFSET,
	EDG_NEXTION_PAGE_ALARM,
	EDG_NEXTION_PAGE_HELP,
	EDG_NEXTION_PAGE_LAST, /*** used for check a valid page ***/

}EDG_NEXTION_PageTypeDef;

/**
  * @brief
  */
typedef enum
{

	EDG_NEXTION_COMMAND_CHANGE_PAGE,
	EDG_NEXTION_COMMAND_LOAD_PROGRAM_VALUES,
	EDG_NEXTION_COMMAND_SET_DATE,
	EDG_NEXTION_COMMAND_PAUSE_PROCESS,
	EDG_NEXTION_COMMAND_STOP_PROCESS,
	EDG_NEXTION_COMMAND_SET_TEMPERATURE,
	EDG_NEXTION_COMMAND_RUN_PROCESS,
	EDG_NEXTION_COMMAND_SAVE_SCHEDULE,
	EDG_NEXTION_COMMAND_RESET,
	EDG_NEXTION_COMMAND_BUZZER,
	EDG_NEXTION_COMMAND_BUZZER_CONFIG,
	EDG_NEXTION_COMMAND_BRIGHT,
	EDG_NEXTION_COMMAND_OFFSET, /* (13/05/23) comando agregado para configurar offset */
	EDG_NEXTION_COMMAND_MANUAL_MODE,
	EDG_NEXTION_COMMAND_SAVE_PROGRAM,
	EDG_NEXTION_COMMAND_RESUME_PROCESS,
	EDG_NEXTION_COMMAND_RESTART_MEMORY,
	EDG_NEXTION_COMMAND_NONE, /*** used as default value command ***/
	EDG_NEXTION_COMMAND_LAST, /*** used for check a valid command ***/

}EDG_NEXTION_CommandTypeDef;

/**
  * @brief
  */
typedef enum
{

	EDG_NEXTION_RTC_POS_MINUTE=1,
	EDG_NEXTION_RTC_POS_HOUR,
	EDG_NEXTION_RTC_POS_WEEKDAY,
	EDG_NEXTION_RTC_POS_DAY,
	EDG_NEXTION_RTC_POS_MONTH,
	EDG_NEXTION_RTC_POS_YEAR,
	EDG_NEXTION_RTC_POS_AMPM,

}EDG_NEXTION_RtcPosTypeDef;

/**
  * @brief
  */
typedef enum
{

	EDG_NEXTION_RX_FRAME_STATUS_NOT_RECEIVED,
	EDG_NEXTION_RX_FRAME_STATUS_INCOMING_FRAME,
	EDG_NEXTION_RX_FRAME_STATUS_RECEIVED,

}EDG_NEXTION_RxFrameStatusTypeDef;

/**
  * @brief
  */
typedef enum
{

	EDG_NEXTION_BRIGHT_STATUS_NORMAL,
	EDG_NEXTION_BRIGHT_STATUS_LOW,
	EDG_NEXTION_BRIGHT_STATUS_CHANGED,

}EDG_NEXTION_BrightStatusTypeDef;


/**
  * @brief Nextion Handle Structure definition
  */
typedef struct __EDG_NEXTION_BrightTypeDef
{
	EDG_NEXTION_BrightStatusTypeDef Status;
	uint8_t Level;

}EDG_NEXTION_BrightTypeDef;

/**
  * @brief Nextion Handle Structure definition
  */
typedef struct __EDG_NEXTION_HandleTypeDef
{

	EDG_NEXTION_RxFrameStatusTypeDef RxFrameStatus;
	EDG_NEXTION_CommandTypeDef CommandReceived;
	EDG_NEXTION_PageTypeDef CurrentPage;
	uint8_t RxFrame[EDG_NEXTION_FRAME_SIZE];
	uint8_t TxFrame[EDG_NEXTION_FRAME_SIZE];
	uint8_t Endflag[4];
	uint32_t DataReceived[EDG_NEXTION_MAX_DATA_RECEIVED];
	uint16_t DataReceivedLen;

	EDG_NEXTION_BrightTypeDef Bright;

}EDG_NEXTION_HandleTypeDef;

extern EDG_NEXTION_HandleTypeDef hedgNextion;
extern const uint8_t EDG_NEXTION_WeekdayNames[][10];
extern const uint8_t EDG_NEXTION_MonthNames[][11];
extern const uint8_t EDG_NEXTION_AmPmNames[][3];

/* Nextion Functions -------------------------------------*/

void EDG_NEXTION_Init(EDG_NEXTION_HandleTypeDef * ptrhedgNextion);

EDG_NEXTION_StatusTypeDef EDG_NEXTION_StartReceiveFrame(EDG_NEXTION_HandleTypeDef * ptrhedgNextion);
EDG_NEXTION_StatusTypeDef EDG_NEXTION_StopReceiveFrame(void);
EDG_NEXTION_StatusTypeDef EDG_NEXTION_ReceiveFrame(EDG_NEXTION_HandleTypeDef * ptrhedgNextion);
EDG_NEXTION_StatusTypeDef EDG_NEXTION_SendFrame(EDG_NEXTION_HandleTypeDef * ptrhedgNextion);
EDG_NEXTION_StatusTypeDef EDG_NEXTION_GetAllDataReceived(EDG_NEXTION_HandleTypeDef * ptrhedgNextion);

EDG_NEXTION_PageTypeDef EDG_NEXTION_GetCurrentePage(EDG_NEXTION_HandleTypeDef * ptrhedgNextion);
void EDG_NEXTION_EnableTouch(EDG_NEXTION_HandleTypeDef * ptrhedgNextion);
void EDG_NEXTION_DisableTouch(EDG_NEXTION_HandleTypeDef * ptrhedgNextion);
void EDG_NEXTION_ChangePage(EDG_NEXTION_HandleTypeDef * ptrhedgNextion, EDG_NEXTION_PageTypeDef Page);

void EDG_NEXTION_RestartBrightTim(EDG_NEXTION_HandleTypeDef * ptrhedgNextion);
void EDG_NEXTION_StopBrightTim(void);
void EDG_NEXTION_SetCurrentBright(EDG_NEXTION_HandleTypeDef * ptrhedgNextion);
void EDG_NEXTION_SetLowBright(EDG_NEXTION_HandleTypeDef * ptrhedgNextion);


uint32_t EDG_NEXTION_Atoi(char* str);

#ifdef __cplusplus
}
#endif

#endif /* EDG_NEXTION_H_ */
