/*
 * EDG_AC_CONTROL.cpp
 *
 *  Created on: 06/04/2023
 *      Author: Elkin Granados
 */

/*** Include header file ***/
#include "EDG_AC_CONTROL.h"

EDG_AC_CONTROL_HandleTypeDef hedgAccontrol;

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_AC_CONTROL_Init(EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol,
						 EDG_MAX6675_HandleTypeDef * ptrhedgMAX6675)
{

	/*** Set the qty of units to be controlled ***/
	ptrhedgAccontrol->UnitsQty = EDG_AC_CONTROL_MAX_UNITS;

	ptrhedgAccontrol->UnitsInAlarm = 0x0000;
	ptrhedgAccontrol->SensorsInAlarm = 0x0000;

	/*** Check the sensor status ***/
	EDG_AC_CONTROL_CheckSensors(ptrhedgAccontrol, ptrhedgMAX6675);

	for(ptrhedgAccontrol->UnitsCounter = 0; ptrhedgAccontrol->UnitsCounter < ptrhedgAccontrol->UnitsQty; ptrhedgAccontrol->UnitsCounter++)
	{
		/*** Check the unit value to set the right control type ***/
		if(ptrhedgAccontrol->UnitsCounter < 0)
		{
			ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].ControlType = EDG_AC_CONTROL_CONTROL_TYPE_ONOFF;
		}
		else
		{
			ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].ControlType = EDG_AC_CONTROL_CONTROL_TYPE_PID;
		}

		/*** All the PÏD values are set no matter the sensor state or control type ***/
		switch(ptrhedgAccontrol->UnitsCounter)
		{
			case EDG_AC_CONTROL_CONTROL_LOAD_0:
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.kp = EDG_AC_CONTROL_PID_P_L0;
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.ki = EDG_AC_CONTROL_PID_I_L0;
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.kd = EDG_AC_CONTROL_PID_D_L0;
				break;

			case EDG_AC_CONTROL_CONTROL_LOAD_1:
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.kp = EDG_AC_CONTROL_PID_P_L1;
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.ki = EDG_AC_CONTROL_PID_I_L1;
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.kd = EDG_AC_CONTROL_PID_D_L1;
				break;

			case EDG_AC_CONTROL_CONTROL_LOAD_2:
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.kp = EDG_AC_CONTROL_PID_P_L2;
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.ki = EDG_AC_CONTROL_PID_I_L2;
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.kd = EDG_AC_CONTROL_PID_D_L2;
				break;

			case EDG_AC_CONTROL_CONTROL_LOAD_3:
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.kp = EDG_AC_CONTROL_PID_P_L3;
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.ki = EDG_AC_CONTROL_PID_I_L3;
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.kd = EDG_AC_CONTROL_PID_D_L3;
				break;

			case EDG_AC_CONTROL_CONTROL_LOAD_4:
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.kp = EDG_AC_CONTROL_PID_P_L4;
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.ki = EDG_AC_CONTROL_PID_I_L4;
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.kd = EDG_AC_CONTROL_PID_D_L4;
				break;

			case EDG_AC_CONTROL_CONTROL_LOAD_5:
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.kp = EDG_AC_CONTROL_PID_P_L5;
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.ki = EDG_AC_CONTROL_PID_I_L5;
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.kd = EDG_AC_CONTROL_PID_D_L5;
				break;

			default:

				break;
		}

		ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.SetPoint = EDG_AC_CONTROL_DEFAULT_VALUE;
		ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.Control = 0;
		ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.Proportional = 0;
		ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.Integral = 0;
		ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.Derivative = 0;
		ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.Error = 0;
		ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.Error_0 = 0;
		ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.Error_Sum = 0;

		ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].PwmStatus = EDG_AC_CONTROL_PWM_STATUS_INACTIVE;

	}

	EDG_AC_CONTROL_CheckOffsetValues(ptrhedgAccontrol);
	EDG_AC_CONTROL_LoadOffsetValues(ptrhedgAccontrol);
	EDG_AC_CONTROL_CheckAlarm(ptrhedgAccontrol);

	if(ptrhedgAccontrol->AlarmStatus == EDG_AC_CONTROL_ALARM_STATUS_ACTIVE)
	{
		EDG_AC_CONTROL_OpenAcRelay();
	}
	else
	{
		EDG_AC_CONTROL_CloseAcRelay();
	}

	return;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_AC_CONTROL_CheckSensors(EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol,
						         EDG_MAX6675_HandleTypeDef * ptrhedgMAX6675)
{

	ptrhedgAccontrol->SensorsInAlarm = 0x0000;
	/*** Read all the max6675 chips to check the thermocouple status ***/
	EDG_MAX6675_ReadAllChips(ptrhedgMAX6675);

	for(ptrhedgAccontrol->UnitsCounter = 0; ptrhedgAccontrol->UnitsCounter < ptrhedgAccontrol->UnitsQty; ptrhedgAccontrol->UnitsCounter++)
	{

		/*** Check the status of each chip to set the values of each unit ***/
		if(ptrhedgMAX6675->Chip[ptrhedgAccontrol->UnitsCounter].ChipStatus != EDG_MAX6675_CHIP_STATUS_OK)
		{
			/*** If the chip is set as TC open then set the sensor status as not present ***/
			if(ptrhedgMAX6675->Chip[ptrhedgAccontrol->UnitsCounter].ChipStatus == EDG_MAX6675_CHIP_STATUS_TC_OPEN)
			{
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].SensorStatus = EDG_AC_CONTROL_SENSOR_STATUS_NOT_PRESENT;
				ptrhedgAccontrol->SensorsInAlarm |= (1<< ptrhedgAccontrol->UnitsCounter);
			}
			else
			{
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].SensorStatus = EDG_AC_CONTROL_SENSOR_STATUS_ERROR;
				ptrhedgAccontrol->SensorsInAlarm |= (1<< ptrhedgAccontrol->UnitsCounter);
			}

			/*** If the chip is not right and was active stop output ***/
			if((ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].ControlStatus == EDG_AC_CONTROL_CONTROL_STATUS_ACTIVE) ||
			   (ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].PwmStatus == EDG_AC_CONTROL_PWM_STATUS_ACTIVE))
			{
				EDG_AC_CONTROL_StopPWMOutput(ptrhedgAccontrol, ptrhedgAccontrol->UnitsCounter);
			}

			/*** If the chip is not right the control flag is inactive ***/
			ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].ControlStatus = EDG_AC_CONTROL_CONTROL_STATUS_INACTIVE;
			ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].PwmStatus = EDG_AC_CONTROL_PWM_STATUS_INACTIVE;
			/*** Set error value of current value ***/
			ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.CurrentValue = 0;

		}
		else
		{
			ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].SensorStatus = EDG_AC_CONTROL_SENSOR_STATUS_OK;
			/*** If the chip is right the control flag is active ***/
			ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].ControlStatus = EDG_AC_CONTROL_CONTROL_STATUS_ACTIVE;
			ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.CurrentValue = ptrhedgMAX6675->Chip[ptrhedgAccontrol->UnitsCounter].Temperature;
		}
	}

	return;
}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_AC_CONTROL_CheckAlarm(EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol)
{

	ptrhedgAccontrol->UnitsInAlarm = 0x0000;

	for(ptrhedgAccontrol->UnitsCounter = 0; ptrhedgAccontrol->UnitsCounter < ptrhedgAccontrol->UnitsQty; ptrhedgAccontrol->UnitsCounter++)
	{
		/*** If sensor is OK check the value ***/
		if(ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].SensorStatus == EDG_AC_CONTROL_SENSOR_STATUS_OK)
		{
			/*** Check if the value is within the range ***/
			if(ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.CurrentValue > EDG_AC_CONTROL_MAX_VALUE)
			{
				/*** If the value is out of the range set the alarm flag and the values status flag ***/
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].ValueStatus = EDG_AC_CONTROL_VALUE_STATUS_OUT_OF_RANGE;
				ptrhedgAccontrol->UnitsInAlarm |= (1U << ptrhedgAccontrol->UnitsCounter);
			}
			else
			{
				/*** If the value is within the range set the value status flag ***/
				ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].ValueStatus = EDG_AC_CONTROL_VALUE_STATUS_OK;
			}
		}
		else
		{
			/*** If sensor is not OK the value is not available ***/
			ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].ValueStatus = EDG_AC_CONTROL_VALUE_STATUS_NOT_AVAILABLE;
		}
	}

	if(ptrhedgAccontrol->UnitsInAlarm == 0)
	{
		ptrhedgAccontrol->AlarmStatus = EDG_AC_CONTROL_ALARM_STATUS_INACTIVE;
	}
	else
	{
		ptrhedgAccontrol->AlarmStatus = EDG_AC_CONTROL_ALARM_STATUS_ACTIVE;
	}

	return;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_AC_CONTROL_PerformAll(EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol,
						 	   EDG_MAX6675_HandleTypeDef * ptrhedgMAX6675)
{

	/*** Chechk the sensors status and get the current values ***/
	EDG_AC_CONTROL_CheckSensors(ptrhedgAccontrol, ptrhedgMAX6675);

	for(ptrhedgAccontrol->UnitsCounter = 0; ptrhedgAccontrol->UnitsCounter < ptrhedgAccontrol->UnitsQty; ptrhedgAccontrol->UnitsCounter++)
	{
		if((ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].ControlStatus == EDG_AC_CONTROL_CONTROL_STATUS_ACTIVE) &&
		   (ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].SensorStatus == EDG_AC_CONTROL_SENSOR_STATUS_OK))
		   {
				if(ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].ControlType == EDG_AC_CONTROL_CONTROL_TYPE_ONOFF)
				{
					EDG_AC_CONTROL_PerformOnOff(ptrhedgAccontrol);
				}
				else if(ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].ControlType == EDG_AC_CONTROL_CONTROL_TYPE_PID)
				{
					EDG_AC_CONTROL_PerformPid(ptrhedgAccontrol);
				}
		   }
	}

	EDG_AC_CONTROL_CheckAlarm(ptrhedgAccontrol);

	return;
}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_AC_CONTROL_PerformPid(EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol)
{

	EDG_AC_CONTROL_Pid(&ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid);
	/***After perform the PID calculation set the PWM output ***/
	EDG_AC_CONTROL_SetPWMOutput(ptrhedgAccontrol, ptrhedgAccontrol->UnitsCounter, EDG_AC_CONTROL_PWMEcuation((ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.Control/EDG_AC_CONTROL_MAX_PWM) * 100));

	return;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_AC_CONTROL_PerformOnOff(EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol)
{

	/* (13/05/23) Al setpoint le quito el valor de offset para el control OnOff */
	if(ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.CurrentValue < (ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.SetPoint - ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.Offset))
	{
		if(ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].PwmStatus == EDG_AC_CONTROL_PWM_STATUS_INACTIVE)
		{
			EDG_AC_CONTROL_SetPWMOutput(ptrhedgAccontrol, ptrhedgAccontrol->UnitsCounter, 1);
		}
	}
	else
	{
		if(ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].PwmStatus == EDG_AC_CONTROL_PWM_STATUS_ACTIVE)
		{
			EDG_AC_CONTROL_StopPWMOutput(ptrhedgAccontrol, ptrhedgAccontrol->UnitsCounter);
		}
	}

	return;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_AC_CONTROL_Pid(EDG_AC_CONTROL_PidStructTypeDef * ptrPidStruct)
{

	ptrPidStruct->Error_0 = ptrPidStruct->Error;
	/* (13/05/23) Al setpoint le quito el valor de offset para el calculo del error */
	ptrPidStruct->Error = (ptrPidStruct->SetPoint - ptrPidStruct->Offset) - ptrPidStruct->CurrentValue;
	ptrPidStruct->Error_Sum += (ptrPidStruct->Error * EDG_AC_CONTROL_SAMPLE_TIME_SECS);

	ptrPidStruct->Proportional = ptrPidStruct->kp * ptrPidStruct->Error;
	ptrPidStruct->Integral = ptrPidStruct->ki * ptrPidStruct->Error_Sum;
	ptrPidStruct->Derivative = ptrPidStruct->kd * ((ptrPidStruct->Error - ptrPidStruct->Error_0)/EDG_AC_CONTROL_SAMPLE_TIME_SECS);

	ptrPidStruct->Control = ptrPidStruct->Proportional + ptrPidStruct->Integral + ptrPidStruct->Derivative;

	if(ptrPidStruct->Control > EDG_AC_CONTROL_MAX_PWM)
	{
		ptrPidStruct->Control = EDG_AC_CONTROL_MAX_PWM;
	}
	if(ptrPidStruct->Control < 0)
	{
		ptrPidStruct->Control = 0;
	}

	return;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_AC_CONTROL_ResetPidValues(EDG_AC_CONTROL_PidStructTypeDef * ptrPidStruct)
{

	ptrPidStruct->Control = 0;
	ptrPidStruct->Proportional = 0;
	ptrPidStruct->Integral = 0;
	ptrPidStruct->Derivative = 0;
	ptrPidStruct->Error = 0;
	ptrPidStruct->Error_0 = 0;
	ptrPidStruct->Error_Sum = 0;

	return;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_AC_CONTROL_SetPWMOutput(EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol, uint8_t output, uint16_t value)
{

	uint32_t tickstart = HAL_GetTick();

	switch(output)
	{
		case EDG_AC_CONTROL_CONTROL_LOAD_0:

			if(ptrhedgAccontrol->Units[output].PwmStatus == EDG_AC_CONTROL_PWM_STATUS_INACTIVE &&
			   ptrhedgAccontrol->Units[output].Pid.SetPoint != 0)
			{
				EDG_AC_CONTROL_ResetPidValues(&ptrhedgAccontrol->Units[output].Pid);
				ptrhedgAccontrol->Units[output].PwmStatus = EDG_AC_CONTROL_PWM_STATUS_ACTIVE;
				__HAL_TIM_SET_COMPARE(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_0, value);
				HAL_TIM_PWM_Start(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_0);
			}
			else if(ptrhedgAccontrol->Units[output].PwmStatus == EDG_AC_CONTROL_PWM_STATUS_ACTIVE)
			{
				while((!(__HAL_TIM_GET_FLAG(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, TIM_IT_UPDATE))) &&
						((HAL_GetTick() - tickstart) < EDG_AC_CONTROL_WAIT_MS));
				if(__HAL_TIM_GET_FLAG(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, TIM_IT_UPDATE))
				{
					__HAL_TIM_CLEAR_FLAG(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, TIM_IT_UPDATE);
					__HAL_TIM_SET_COMPARE(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_0, value);
				}

			}

			break;

		case EDG_AC_CONTROL_CONTROL_LOAD_1:

			if(ptrhedgAccontrol->Units[output].PwmStatus == EDG_AC_CONTROL_PWM_STATUS_INACTIVE &&
			   ptrhedgAccontrol->Units[output].Pid.SetPoint != 0)
			{
				EDG_AC_CONTROL_ResetPidValues(&ptrhedgAccontrol->Units[output].Pid);
				ptrhedgAccontrol->Units[output].PwmStatus = EDG_AC_CONTROL_PWM_STATUS_ACTIVE;
				__HAL_TIM_SET_COMPARE(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_1, value);
				HAL_TIM_PWM_Start(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_1);
			}
			else if(ptrhedgAccontrol->Units[output].PwmStatus == EDG_AC_CONTROL_PWM_STATUS_ACTIVE)
			{
				while((!(__HAL_TIM_GET_FLAG(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, TIM_IT_UPDATE))) &&
					  ((HAL_GetTick() - tickstart) < EDG_AC_CONTROL_WAIT_MS));
				if(__HAL_TIM_GET_FLAG(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, TIM_IT_UPDATE))
				{
					__HAL_TIM_CLEAR_FLAG(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, TIM_IT_UPDATE);
					__HAL_TIM_SET_COMPARE(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_1, value);
				}
			}

			break;

		case EDG_AC_CONTROL_CONTROL_LOAD_2:

			if(ptrhedgAccontrol->Units[output].PwmStatus == EDG_AC_CONTROL_PWM_STATUS_INACTIVE &&
			   ptrhedgAccontrol->Units[output].Pid.SetPoint != 0)
			{
				EDG_AC_CONTROL_ResetPidValues(&ptrhedgAccontrol->Units[output].Pid);
				ptrhedgAccontrol->Units[output].PwmStatus = EDG_AC_CONTROL_PWM_STATUS_ACTIVE;
				__HAL_TIM_SET_COMPARE(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_2, value);
				HAL_TIM_PWM_Start(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_2);
			}
			else if(ptrhedgAccontrol->Units[output].PwmStatus == EDG_AC_CONTROL_PWM_STATUS_ACTIVE)
			{
				while((!(__HAL_TIM_GET_FLAG(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, TIM_IT_UPDATE))) &&
					  ((HAL_GetTick() - tickstart) < EDG_AC_CONTROL_WAIT_MS));
				if(__HAL_TIM_GET_FLAG(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, TIM_IT_UPDATE))
				{
					__HAL_TIM_CLEAR_FLAG(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, TIM_IT_UPDATE);
					__HAL_TIM_SET_COMPARE(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_2, value);
				}
			}

			break;

		case EDG_AC_CONTROL_CONTROL_LOAD_3:

			if(ptrhedgAccontrol->Units[output].PwmStatus == EDG_AC_CONTROL_PWM_STATUS_INACTIVE &&
			   ptrhedgAccontrol->Units[output].Pid.SetPoint != 0)
			{
				EDG_AC_CONTROL_ResetPidValues(&ptrhedgAccontrol->Units[output].Pid);
				ptrhedgAccontrol->Units[output].PwmStatus = EDG_AC_CONTROL_PWM_STATUS_ACTIVE;
				__HAL_TIM_SET_COMPARE(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_3, value);
				HAL_TIM_PWM_Start(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_3);
			}
			else if(ptrhedgAccontrol->Units[output].PwmStatus == EDG_AC_CONTROL_PWM_STATUS_ACTIVE)
			{
				while((!(__HAL_TIM_GET_FLAG(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, TIM_IT_UPDATE))) &&
					  ((HAL_GetTick() - tickstart) < EDG_AC_CONTROL_WAIT_MS));
				if(__HAL_TIM_GET_FLAG(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, TIM_IT_UPDATE))
				{
					__HAL_TIM_CLEAR_FLAG(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, TIM_IT_UPDATE);
					__HAL_TIM_SET_COMPARE(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_3, value);
				}
			}

			break;

		case EDG_AC_CONTROL_CONTROL_LOAD_4:

			if(ptrhedgAccontrol->Units[output].PwmStatus == EDG_AC_CONTROL_PWM_STATUS_INACTIVE &&
			   ptrhedgAccontrol->Units[output].Pid.SetPoint != 0)
			{
				EDG_AC_CONTROL_ResetPidValues(&ptrhedgAccontrol->Units[output].Pid);
				ptrhedgAccontrol->Units[output].PwmStatus = EDG_AC_CONTROL_PWM_STATUS_ACTIVE;
				__HAL_TIM_SET_COMPARE(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_4, value);
				HAL_TIM_PWM_Start(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_4);
			}
			else if(ptrhedgAccontrol->Units[output].PwmStatus == EDG_AC_CONTROL_PWM_STATUS_ACTIVE)
			{
				while((!(__HAL_TIM_GET_FLAG(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, TIM_IT_UPDATE))) &&
					  ((HAL_GetTick() - tickstart) < EDG_AC_CONTROL_WAIT_MS));
				if(__HAL_TIM_GET_FLAG(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, TIM_IT_UPDATE))
				{
					__HAL_TIM_CLEAR_FLAG(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, TIM_IT_UPDATE);
					__HAL_TIM_SET_COMPARE(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_4, value);
				}
			}

			break;

		case EDG_AC_CONTROL_CONTROL_LOAD_5:

			if(ptrhedgAccontrol->Units[output].PwmStatus == EDG_AC_CONTROL_PWM_STATUS_INACTIVE &&
			   ptrhedgAccontrol->Units[output].Pid.SetPoint != 0)
			{
				EDG_AC_CONTROL_ResetPidValues(&ptrhedgAccontrol->Units[output].Pid);
				ptrhedgAccontrol->Units[output].PwmStatus = EDG_AC_CONTROL_PWM_STATUS_ACTIVE;
				__HAL_TIM_SET_COMPARE(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_5, value);
				HAL_TIM_PWM_Start(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_5);
			}
			else if(ptrhedgAccontrol->Units[output].PwmStatus == EDG_AC_CONTROL_PWM_STATUS_ACTIVE)
			{
				while((!(__HAL_TIM_GET_FLAG(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, TIM_IT_UPDATE))) &&
					  ((HAL_GetTick() - tickstart) < EDG_AC_CONTROL_WAIT_MS));
				if(__HAL_TIM_GET_FLAG(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, TIM_IT_UPDATE))
				{
					__HAL_TIM_CLEAR_FLAG(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, TIM_IT_UPDATE);
					__HAL_TIM_SET_COMPARE(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_5, value);
				}
			}

			break;

		default:

			break;

	}

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_AC_CONTROL_StopPWMOutput(EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol, uint8_t output)
{

	switch(output)
	{
		case EDG_AC_CONTROL_CONTROL_LOAD_0:

			HAL_TIM_PWM_Stop(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_0);

			break;

		case EDG_AC_CONTROL_CONTROL_LOAD_1:

			HAL_TIM_PWM_Stop(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_1);

			break;

		case EDG_AC_CONTROL_CONTROL_LOAD_2:

			HAL_TIM_PWM_Stop(&EDG_AC_CONTROL_TIM_LOAD_0_2_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_2);

			break;

		case EDG_AC_CONTROL_CONTROL_LOAD_3:

			HAL_TIM_PWM_Stop(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_3);

			break;

		case EDG_AC_CONTROL_CONTROL_LOAD_4:

			HAL_TIM_PWM_Stop(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_4);

			break;

		case EDG_AC_CONTROL_CONTROL_LOAD_5:

			HAL_TIM_PWM_Stop(&EDG_AC_CONTROL_TIM_LOAD_3_5_HANDLE, EDG_AC_CONTROL_TIM_CHANNEL_LOAD_5);

			break;

		default:

			break;

	}

	ptrhedgAccontrol->Units[output].PwmStatus = EDG_AC_CONTROL_PWM_STATUS_INACTIVE;

	return;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
uint16_t EDG_AC_CONTROL_PWMEcuation(float val)
{
	double res = 0.0;

	/*** Ecuation that relates a needed power value with an alpha angle of trigger ***/
	res = (double)EDG_AC_CONTROL_MAX_PWM * ((-2e-06*(EDG_AC_CONTROL_PowerCalc(val,3))) + (0.0002*(EDG_AC_CONTROL_PowerCalc(val,2))) - (0.0147*val)  + 1.0052);

	if(res > EDG_AC_CONTROL_MAX_PWM)
	{
		res = EDG_AC_CONTROL_MAX_PWM;
	}
	else if(res < 0)
	{
		res = 0;
	}
	return (uint16_t)res;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_AC_CONTROL_CheckOffsetValues(EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol)
{

	uint8_t Temp[EDG_MEM_ADDR_OFFSET_QTY] = {0};
	uint8_t FlagWrite = 0;

	if(EDG_MEMORY_ReadMemory(EDG_MEMORY_ADDRESS_MEM1,
						     EDG_MEM_ADDR_BASE_OFFSET,
							 Temp,
							 EDG_MEM_ADDR_OFFSET_QTY) == EDG_MEMORY_STATE_OK)
	{
		for(ptrhedgAccontrol->UnitsCounter = 0; ptrhedgAccontrol->UnitsCounter < ptrhedgAccontrol->UnitsQty; ptrhedgAccontrol->UnitsCounter++)
		{
			if(Temp[ptrhedgAccontrol->UnitsCounter] > EDG_AC_CONTROL_PID_MAX_OFFSET)
			{
				Temp[ptrhedgAccontrol->UnitsCounter] = EDG_AC_CONTROL_PID_DEFAULT_OFFSET;
				FlagWrite = 1;
			}
		}

		if(FlagWrite == 1)
		{
			EDG_MEMORY_WriteMemory(EDG_MEMORY_ADDRESS_MEM1,
								   EDG_MEM_ADDR_BASE_OFFSET,
								   Temp,
								   EDG_MEM_ADDR_OFFSET_QTY);
		}

	}

	return;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_AC_CONTROL_LoadOffsetValues(EDG_AC_CONTROL_HandleTypeDef * ptrhedgAccontrol)
{

	uint8_t Temp[EDG_MEM_ADDR_VALUES_X_OFFSET] = {0};

	if(EDG_MEMORY_ReadMemory(EDG_MEMORY_ADDRESS_MEM1,
						     EDG_MEM_ADDR_BASE_OFFSET,
							 Temp,
							 EDG_MEM_ADDR_VALUES_X_OFFSET) == EDG_MEMORY_STATE_OK)
	{
		for(ptrhedgAccontrol->UnitsCounter = 0; ptrhedgAccontrol->UnitsCounter < ptrhedgAccontrol->UnitsQty; ptrhedgAccontrol->UnitsCounter++)
		{
			ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.Offset = Temp[ptrhedgAccontrol->UnitsCounter];
		}
		return;
	}
	else
	{
		for(ptrhedgAccontrol->UnitsCounter = 0; ptrhedgAccontrol->UnitsCounter < ptrhedgAccontrol->UnitsQty; ptrhedgAccontrol->UnitsCounter++)
		{
			ptrhedgAccontrol->Units[ptrhedgAccontrol->UnitsCounter].Pid.Offset = EDG_AC_CONTROL_PID_DEFAULT_OFFSET;
		}
	}

	return;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
double EDG_AC_CONTROL_PowerCalc(float val, uint8_t pot)
{
	uint8_t counter = 0;
	double res = 0;

	res = (double)val;
	for(counter = 0; counter < (pot-1); counter++)
	{
		res *= (double)val;
	}

	return res;
}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_AC_CONTROL_CloseAcRelay(void)
{
	HAL_GPIO_WritePin(RELE_AC_GPIO_Port, RELE_AC_Pin, GPIO_PIN_SET);
	return;
}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_AC_CONTROL_OpenAcRelay(void)
{
	HAL_GPIO_WritePin(RELE_AC_GPIO_Port, RELE_AC_Pin, GPIO_PIN_RESET);
	return;
}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
EDG_AC_CONTROL_RelayStatusTypeDef EDG_AC_CONTROL_GetRelayStatus(void)
{
	return (EDG_AC_CONTROL_RelayStatusTypeDef)HAL_GPIO_ReadPin(RELE_AC_GPIO_Port, RELE_AC_Pin);
}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_AC_CONTROL_ToogleAcRelay(void)
{
	HAL_GPIO_TogglePin(RELE_AC_GPIO_Port, RELE_AC_Pin);
	return;
}
