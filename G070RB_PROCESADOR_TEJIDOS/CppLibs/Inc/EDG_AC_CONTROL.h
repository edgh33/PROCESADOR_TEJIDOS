/*
 * EDG_AC_CONTROL.h
 *
 *  Created on: 06/04/2023
 *      Author: Elkin Granados
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EDG_AC_CONTROL_H_
#define EDG_AC_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "EDG_CONFIG.h"

/**** Include the main.h file if the workspace is CubeIDE ****/
#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

#include <string.h>
#include "main.h"
#include "tim.h"

#endif

#include "EDG_MAX6675.h"
#include "EDG_MEMORY.h"
#include "EDG_MEM_ADDR.h"

#define EDG_AC_CONTROL_TIM_LOAD_0_2			TIM1
#define EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE 	htim1
#define EDG_AC_CONTROL_TIM_LOAD_3_5 		TIM3
#define EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE 	htim3

#define EDG_AC_CONTROL_TIM_CHANNEL_LOAD_0	TIM_CHANNEL_1
#define EDG_AC_CONTROL_TIM_CHANNEL_LOAD_1	TIM_CHANNEL_3
#define EDG_AC_CONTROL_TIM_CHANNEL_LOAD_2	TIM_CHANNEL_4
#define EDG_AC_CONTROL_TIM_CHANNEL_LOAD_3	TIM_CHANNEL_1
#define EDG_AC_CONTROL_TIM_CHANNEL_LOAD_4	TIM_CHANNEL_3
#define EDG_AC_CONTROL_TIM_CHANNEL_LOAD_5	TIM_CHANNEL_4

#define EDG_AC_CONTROL_MAX_PWM		(8099)
#define EDG_AC_CONTROL_MAX_UNITS	(2)

#define EDG_AC_CONTROL_MAX_VALUE		(95.0)
#define EDG_AC_CONTROL_DEFAULT_VALUE	(60.0)
#define EDG_AC_CONTROL_WAIT_MS			(10)
#define EDG_AC_CONTROL_SAMPLE_TIME_SECS	(5)  /*** For be used in Timer.h ***/

#define EDG_AC_CONTROL_PID_MAX		(EDG_AC_CONTROL_MAX_PWM)
#define EDG_AC_CONTROL_PID_MIN		(1U)
#define EDG_AC_CONTROL_PID_DEFAULT_OFFSET	(5U)
#define EDG_AC_CONTROL_PID_MAX_OFFSET		(20U)

#define EDG_AC_CONTROL_PID_P		(100.0)
#define EDG_AC_CONTROL_PID_I		(0.0)
#define EDG_AC_CONTROL_PID_D		(0.0)

#define EDG_AC_CONTROL_PID_P_L0		(5000.0) 	//Test Value: 5000.0
#define EDG_AC_CONTROL_PID_I_L0		(0.04)		//Test Value: 0.5
#define EDG_AC_CONTROL_PID_D_L0		(3.0)

#define EDG_AC_CONTROL_PID_P_L1		(4500.0)	//Test Value: 4500.0
#define EDG_AC_CONTROL_PID_I_L1		(0.025)		//Test Value: 0.5
#define EDG_AC_CONTROL_PID_D_L1		(3.0)

#define EDG_AC_CONTROL_PID_P_L2		(4500.0)	//Test Value: 4500.0
#define EDG_AC_CONTROL_PID_I_L2		(0.025)		//Test Value: 0.5
#define EDG_AC_CONTROL_PID_D_L2		(3.0)

#define EDG_AC_CONTROL_PID_P_L3		(2500.0)	//Test Value: 3000.0
#define EDG_AC_CONTROL_PID_I_L3		(0.2)		//Test Value: 0.2
#define EDG_AC_CONTROL_PID_D_L3		(0.0)

#define EDG_AC_CONTROL_PID_P_L4		(3000.0)	//Test Value: 3500.0
#define EDG_AC_CONTROL_PID_I_L4		(0.25)		//Test Value: 0.25
#define EDG_AC_CONTROL_PID_D_L4		(0.0)

#define EDG_AC_CONTROL_PID_P_L5		(1200.0)	//Test Value: 2000.0
#define EDG_AC_CONTROL_PID_I_L5		(0.15)		//Test Value: 0.15
#define EDG_AC_CONTROL_PID_D_L5		(0.001)

typedef enum
{

	EDG_AC_CONTROL_CONTROL_LOAD_0,
	EDG_AC_CONTROL_CONTROL_LOAD_1,
	EDG_AC_CONTROL_CONTROL_LOAD_2,
	EDG_AC_CONTROL_CONTROL_LOAD_3,
	EDG_AC_CONTROL_CONTROL_LOAD_4,
	EDG_AC_CONTROL_CONTROL_LOAD_5,

}EDG_AC_CONTROL_LoadTypeDef;

typedef enum
{

	EDG_AC_CONTROL_CONTROL_STATUS_INACTIVE,
	EDG_AC_CONTROL_CONTROL_STATUS_ACTIVE,

}EDG_AC_CONTROL_ControlStatusTypeDef;

typedef enum
{

	EDG_AC_CONTROL_CONTROL_TYPE_ONOFF,
	EDG_AC_CONTROL_CONTROL_TYPE_PID,

}EDG_AC_CONTROL_ControlTypeTypeDef;

typedef enum
{

	EDG_AC_CONTROL_PWM_STATUS_INACTIVE,
	EDG_AC_CONTROL_PWM_STATUS_ACTIVE,

}EDG_AC_CONTROL_PwmStatusTypeDef;

typedef enum
{

	EDG_AC_CONTROL_ALARM_STATUS_INACTIVE,
	EDG_AC_CONTROL_ALARM_STATUS_ACTIVE,

}EDG_AC_CONTROL_AlarmStatusTypeDef;

typedef enum
{

	EDG_AC_CONTROL_VALUE_STATUS_OK,
	EDG_AC_CONTROL_VALUE_STATUS_OUT_OF_RANGE,
	EDG_AC_CONTROL_VALUE_STATUS_NOT_AVAILABLE,

}EDG_AC_CONTROL_ValueStatusTypeDef;

typedef enum
{

	EDG_AC_CONTROL_SENSOR_STATUS_OK,
	EDG_AC_CONTROL_SENSOR_STATUS_NOT_PRESENT,
	EDG_AC_CONTROL_SENSOR_STATUS_ERROR,

}EDG_AC_CONTROL_SensorStatusTypeDef;

typedef enum
{

	EDG_AC_CONTROL_RELAY_STATUS_OPEN,
	EDG_AC_CONTROL_RELAY_STATUS_CLOSE,

}EDG_AC_CONTROL_RelayStatusTypeDef;


/**
  * @brief AC Control PID Structure definition
  */
typedef struct __EDG_AC_CONTROL_PidStructTypeDef
{

	float kp;
	float ki;
	float kd;
	float SetPoint;
	float CurrentValue;
	float Error; //Error = Reference - Sensor
	float Proportional;
	float Integral;
	float Derivative;
	float Error_0; //Error_0 = Error
	float Error_Sum; //(Error += Error)*SampleTime
	float Control;
	float Offset;

}EDG_AC_CONTROL_PidStructTypeDef;

/**
  * @brief AC Control Unit Structure definition
  */
typedef struct __EDG_AC_CONTROL_UnitStructTypeDef
{

	EDG_AC_CONTROL_ControlStatusTypeDef ControlStatus;
	EDG_AC_CONTROL_PwmStatusTypeDef PwmStatus;
	EDG_AC_CONTROL_ControlTypeTypeDef ControlType;
	EDG_AC_CONTROL_SensorStatusTypeDef SensorStatus;
	EDG_AC_CONTROL_ValueStatusTypeDef ValueStatus;
	EDG_AC_CONTROL_PidStructTypeDef Pid;

}EDG_AC_CONTROL_UnitStructTypeDef;

/**
  * @brief AC Control Handle Structure definition
  */
typedef struct __EDG_AC_CONTROL_HandleTypeDef
{

	EDG_AC_CONTROL_UnitStructTypeDef Units[EDG_AC_CONTROL_MAX_UNITS];
	EDG_AC_CONTROL_AlarmStatusTypeDef AlarmStatus;
	uint16_t UnitsQty;
	uint16_t UnitsCounter;
	uint16_t UnitsInAlarm; /*** Works in bit level***/
	uint16_t SensorsInAlarm; /*** Works in bit level***/

}EDG_AC_CONTROL_HandleTypeDef;

extern EDG_AC_CONTROL_HandleTypeDef hedgAccontrol;

void EDG_AC_CONTROL_Init(EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol,
						 EDG_MAX6675_HandleTypeDef * ptrhedgMAX6675);
void EDG_AC_CONTROL_CheckSensors(EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol,
						         EDG_MAX6675_HandleTypeDef * ptrhedgMAX6675);
void EDG_AC_CONTROL_CheckAlarm(EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol);
void EDG_AC_CONTROL_PerformAll(EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol,
						 	   EDG_MAX6675_HandleTypeDef * ptrhedgMAX6675);
void EDG_AC_CONTROL_PerformPid(EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol);
void EDG_AC_CONTROL_PerformOnOff(EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol);
void EDG_AC_CONTROL_Pid(EDG_AC_CONTROL_PidStructTypeDef * ptrPidStruct);
void EDG_AC_CONTROL_ResetPidValues(EDG_AC_CONTROL_PidStructTypeDef * ptrPidStruct);
void EDG_AC_CONTROL_SetPWMOutput(EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol, uint8_t output, uint16_t value);
void EDG_AC_CONTROL_StopPWMOutput(EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol, uint8_t output);
uint16_t EDG_AC_CONTROL_PWMEcuation(float val);
void EDG_AC_CONTROL_CheckOffsetValues(EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol);
void EDG_AC_CONTROL_LoadOffsetValues(EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol);
double EDG_AC_CONTROL_PowerCalc(float val, uint8_t pot);
void EDG_AC_CONTROL_CloseAcRelay(void);
void EDG_AC_CONTROL_OpenAcRelay(void);
void EDG_AC_CONTROL_ToogleAcRelay(void);
EDG_AC_CONTROL_RelayStatusTypeDef EDG_AC_CONTROL_GetRelayStatus(void);

#ifdef __cplusplus
}
#endif

#endif /* EDG_AC_CONTROL_H_ */
