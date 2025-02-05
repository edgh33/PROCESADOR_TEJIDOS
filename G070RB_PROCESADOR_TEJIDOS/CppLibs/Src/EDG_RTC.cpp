/*
 * EDG_RTC.cpp
 *
 *  Created on: 20/01/2023
 *      Author: Elkin Granados
 */

#include "EDG_RTC.h"


EDG_RTC_HandleTypeDef hedgRTC;

/**
  * @brief Funcion que realiza la lectura de multiples registros del RTC
  * @note
  * @param Address direccion I2C del RTC
  * @param MemAddress Direccion del RTC inicial a ser leida
  * @param ptrData Apuntador a donde se va a almacenar el valor o los valores leidos
  * @param Lenght Cantidad de datos a ser leidos
  * @retval EDG_RTC_StateTypeDef Estado del RTC
  */
EDG_RTC_StateTypeDef EDG_RTC_Read(uint8_t Address, uint16_t MemAddress, uint8_t * ptrData, uint16_t Lenght)
{

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

	/* Proteccion para no leer espacios incorrectos de la RAM del RTC */
	if(MemAddress > EDG_RTC_REGISTER_CONTROL)
	{
		return EDG_RTC_STATE_ERROR;
	}

	if(HAL_I2C_Mem_Read(&EDG_RTC_I2C_HANDLER, (Address<<1), MemAddress, I2C_MEMADD_SIZE_8BIT, ptrData, Lenght, EDG_RTC_TIMEOUT_MS) == HAL_OK)
	{
		HAL_Delay(EDG_RTC_WAIT_TIME_MS);
		return EDG_RTC_STATE_OK;
	}
	else
	{
		HAL_Delay(EDG_RTC_WAIT_TIME_MS);
		return EDG_RTC_STATE_ERROR;
	}

#endif

}

/**
  * @brief Funcion que realiza la escritura de multiples registros del RTC
  * @note
  * @param Address direccion I2C del RTC
  * @param MemAddress Direccion del RTC inicial a ser escrita
  * @param ptrData Apuntador donde estan los valor a ser escritos
  * @param Lenght Cantidad de datos a ser escritos
  * @retval EDG_RTC_StateTypeDef Estado del RTC
  */
EDG_RTC_StateTypeDef EDG_RTC_Write(uint8_t Address, uint16_t MemAddress, uint8_t * ptrData, uint16_t Lenght)
{

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

	/* Proteccion para no escribir espacios incorrectos de la RAM del RTC */
	if(MemAddress > EDG_RTC_REGISTER_CONTROL)
	{
		return EDG_RTC_STATE_ERROR;
	}

	if(HAL_I2C_Mem_Write(&EDG_RTC_I2C_HANDLER, (Address<<1), MemAddress, I2C_MEMADD_SIZE_8BIT, ptrData, Lenght, EDG_RTC_TIMEOUT_MS) == HAL_OK)
	{
		HAL_Delay(EDG_RTC_WAIT_TIME_MS);
		return EDG_RTC_STATE_OK;
	}
	else
	{
		HAL_Delay(EDG_RTC_WAIT_TIME_MS);
		return EDG_RTC_STATE_ERROR;
	}

#endif

}

/**
  * @brief Funcion que inicializa el RTC revisando la comunicacion I2C, que este corriendo los segundos y
  * 	   que tenga configurada una fecha actualizada
  * @note
  * @param  Address direccion I2C del RTC
  * @param  ptrhedgRTC Apuntador a la estructura Handler del RTC
  * @retval EDG_RTC_StateTypeDef Estado del RTC
  */
EDG_RTC_StateTypeDef EDG_RTC_Init(EDG_RTC_HandleTypeDef * ptrhedgRTC, uint8_t Address)
{

#if (EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE) || (EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_PLATFORMIO_PLUS_HAL)

	if(EDG_RTC_CheckCommI2C(ptrhedgRTC, Address) == EDG_RTC_STATE_OK)
	{
		if(EDG_RTC_CheckRun(ptrhedgRTC, Address)== EDG_RTC_STATE_OK)
		{
			EDG_RTC_CheckDate(ptrhedgRTC, Address);
			return ptrhedgRTC->CurrentState;
		}
	}
	else
	{
		ptrhedgRTC->CurrentState = EDG_RTC_STATE_ERROR;
		return EDG_RTC_STATE_ERROR;
	}

#endif

	ptrhedgRTC->CurrentState = EDG_RTC_STATE_OK;
	return EDG_RTC_STATE_OK;

}



/**
  * @brief Funcion para obtener la fecha y hora actual
  * @note  La fecha actual queda en la estructura interna del Handler CurrentDate
  * @param Address direccion I2C del RTC
  * @param ptrhedgRTC Apuntador a la estructura Handler del RTC
  * @retval EDG_RTC_StateTypeDef Estado del RTC
  */
EDG_RTC_StateTypeDef EDG_RTC_GetDate(EDG_RTC_HandleTypeDef * ptrhedgRTC, uint8_t Address)
{
	uint8_t temp[EDG_RTC_REGISTER_CONTROL] = {0,0,0,0,0,0,0};

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

	/* Se leen los registros del RTC en un arreglo temporal y despues se ajustan los valores en la estructura de
	 * fecha actual del handler */
	if(EDG_RTC_Read(Address, EDG_RTC_REGISTER_SECONDS, temp, EDG_RTC_REGISTER_CONTROL) == EDG_RTC_STATE_OK)
	{

		ptrhedgRTC->CurrentDate.Second = (((temp[EDG_RTC_REGISTER_SECONDS] >> 4) & 0x07) * 10) + (temp[EDG_RTC_REGISTER_SECONDS] & 0x0F);
		ptrhedgRTC->CurrentDate.Minute = (((temp[EDG_RTC_REGISTER_MINUTES] >> 4) & 0x0F) * 10) + (temp[EDG_RTC_REGISTER_MINUTES] & 0x0F);
		/* Para una libreria mas general se debe comprobar el formato de la hora leyendo el valor actual del registro */

		if(EDG_RTC_GetHourFormat(ptrhedgRTC, Address) == EDG_RTC_STATE_OK)
		{

			if(ptrhedgRTC->CurrentDate.Format == EDG_RTC_HOUR_FORMAT_24H)
			{
				ptrhedgRTC->CurrentDate.Hour = (((temp[EDG_RTC_REGISTER_HOURS] >> 4) & 0x03) * 10) + (temp[EDG_RTC_REGISTER_HOURS] & 0x0F);
				ptrhedgRTC->CurrentDate.AmPm = EDG_RTC_AMPM_AM;
			}
			else
			{
				ptrhedgRTC->CurrentDate.Hour = (((temp[EDG_RTC_REGISTER_HOURS] >> 4) & 0x01) * 10) + (temp[EDG_RTC_REGISTER_HOURS] & 0x0F);
				if(temp[EDG_RTC_REGISTER_HOURS] & EDG_RTC_AMPM_MASK)
				{
					ptrhedgRTC->CurrentDate.AmPm = EDG_RTC_AMPM_PM;
				}
				else
				{
					ptrhedgRTC->CurrentDate.AmPm = EDG_RTC_AMPM_AM;
				}

			}
		}

		ptrhedgRTC->CurrentDate.WeekDay = temp[EDG_RTC_REGISTER_WEEKDAY];
		ptrhedgRTC->CurrentDate.Day = (((temp[EDG_RTC_REGISTER_DAY] >> 4) & 0x03) * 10) + (temp[EDG_RTC_REGISTER_DAY] & 0x0F);
		ptrhedgRTC->CurrentDate.Month = (((temp[EDG_RTC_REGISTER_MONTH] >> 4) & 0x01) * 10) + (temp[EDG_RTC_REGISTER_MONTH] & 0x0F);
		ptrhedgRTC->CurrentDate.Year = (((temp[EDG_RTC_REGISTER_YEAR] >> 4) & 0x0F) * 10) + (temp[EDG_RTC_REGISTER_YEAR] & 0x0F);

	}
	else
	{
		ptrhedgRTC->CurrentState = EDG_RTC_STATE_ERROR;
		return EDG_RTC_STATE_ERROR;
	}

#endif

	ptrhedgRTC->CurrentState = EDG_RTC_STATE_OK;
	return EDG_RTC_STATE_OK;

}

/**
  * @brief Funcion para configurar la fecha y hora actual
  * @note  La fecha a configurar se debe establecer en la estructura interna del Handler DatetoChange
  * @param Address direccion I2C del RTC
  * @param ptrhedgRTC Apuntador a la estructura Handler del RTC
  * @retval EDG_RTC_StateTypeDef Estado del RTC
  */
EDG_RTC_StateTypeDef EDG_RTC_SetDate(EDG_RTC_HandleTypeDef * ptrhedgRTC, uint8_t Address)
{

	uint8_t temp[EDG_RTC_REGISTER_CONTROL] = {0,0,0,0,0,0,0};
	uint8_t flag = 0;

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

	if(EDG_RTC_GetHourFormat(ptrhedgRTC, Address) == EDG_RTC_STATE_OK)
	{
		if(ptrhedgRTC->CurrentDate.Format == EDG_RTC_HOUR_FORMAT_24H)
		{
			if(ptrhedgRTC->DatetoChange.Hour < 24)
			{
				flag = 1;
			}
			else
			{
				flag = 0;
			}
		}
		else
		{
			if((ptrhedgRTC->DatetoChange.Hour <= 12) && (ptrhedgRTC->DatetoChange.Hour > 0))
			{
				flag = 1;
			}
			else
			{
				flag = 0;
			}
		}
	}
	/* Se comprueba que todos los valores de fecha sean correctos */
	if( (ptrhedgRTC->DatetoChange.Second < 60) &&
		(ptrhedgRTC->DatetoChange.Minute < 60) &&
		(ptrhedgRTC->DatetoChange.WeekDay <= EDG_RTC_WEEKDAY_SUNDAY) &&
		(ptrhedgRTC->DatetoChange.Month <= EDG_RTC_MONTH_DECEMBER) &&
		(ptrhedgRTC->DatetoChange.Year >= EDG_RTC_TEMP_YEAR) &&
		(flag == 1))
	{

		flag = 0;

		switch(ptrhedgRTC->DatetoChange.Month)
		{
			case EDG_RTC_MONTH_JANUARY:
			case EDG_RTC_MONTH_MARCH:
			case EDG_RTC_MONTH_MAY:
			case EDG_RTC_MONTH_JULY:
			case EDG_RTC_MONTH_AUGUST:
			case EDG_RTC_MONTH_OCTOBER:
			case EDG_RTC_MONTH_DECEMBER:

				if(ptrhedgRTC->DatetoChange.Day <= 31)
				{
					flag = 1;
				}

				break;

			case EDG_RTC_MONTH_APRIL:
			case EDG_RTC_MONTH_JUNE:
			case EDG_RTC_MONTH_SEPTEMBER:
			case EDG_RTC_MONTH_NOVEMBER:

				if(ptrhedgRTC->DatetoChange.Day <= 30)
				{
					flag = 1;
				}

				break;

			case EDG_RTC_MONTH_FEBRUARY:

				if(ptrhedgRTC->DatetoChange.Day <= 28)
				{
					flag = 1;
				}

				break;

			default:

				break;

		}
	}
	/* Si algun valor de la fecha diferente al dia esta por fuera de los parametros se finaliza por error */
	else
	{

		ptrhedgRTC->DateState = EDG_RTC_DATE_STATE_UNCONFIGURED;
		return EDG_RTC_STATE_OK;

	}
	/* Si todos los valores de la fecha son correctos se ajustan los valores al formato del RTC y se escriben */
	if(flag == 1)
	{

		temp[EDG_RTC_REGISTER_SECONDS] = (((ptrhedgRTC->DatetoChange.Second / 10) & 0x07) << 4) | (((ptrhedgRTC->DatetoChange.Second % 10) & 0x0F));
		temp[EDG_RTC_REGISTER_MINUTES] = (((ptrhedgRTC->DatetoChange.Minute / 10) & 0x07) << 4) | (((ptrhedgRTC->DatetoChange.Minute % 10) & 0x0F));
		/* Para una libreria mas general se debe comprobar el formato de la hora leyendo el valor actual del registro */

		if(ptrhedgRTC->CurrentDate.Format == EDG_RTC_HOUR_FORMAT_24H)
		{
			temp[EDG_RTC_REGISTER_HOURS] = (((ptrhedgRTC->DatetoChange.Hour / 10) & 0x03) << 4) | (((ptrhedgRTC->DatetoChange.Hour % 10) & 0x0F));
			temp[EDG_RTC_REGISTER_HOURS] &= ~(EDG_RTC_HOUR_FORMAT_MASK);
		}
		else
		{
			temp[EDG_RTC_REGISTER_HOURS] = ((((ptrhedgRTC->DatetoChange.Hour / 10) & 0x01) << 4) | (((ptrhedgRTC->DatetoChange.Hour % 10) & 0x0F))) | (ptrhedgRTC->DatetoChange.AmPm << EDG_RTC_AMPM_POS);
			temp[EDG_RTC_REGISTER_HOURS] |= EDG_RTC_HOUR_FORMAT_MASK;
		}

		temp[EDG_RTC_REGISTER_WEEKDAY] = (ptrhedgRTC->DatetoChange.WeekDay & 0x07);
		temp[EDG_RTC_REGISTER_DAY] = (((ptrhedgRTC->DatetoChange.Day / 10) & 0x03) << 4) | (((ptrhedgRTC->DatetoChange.Day % 10) & 0x0F));
		temp[EDG_RTC_REGISTER_MONTH] = (((ptrhedgRTC->DatetoChange.Month / 10) & 0x01) << 4) | (((ptrhedgRTC->DatetoChange.Month % 10) & 0x0F));
		temp[EDG_RTC_REGISTER_YEAR] = (((ptrhedgRTC->DatetoChange.Year / 10) & 0x0F) << 4) | (((ptrhedgRTC->DatetoChange.Year % 10) & 0x0F));

		if(EDG_RTC_Write(Address, EDG_RTC_REGISTER_SECONDS, temp, EDG_RTC_REGISTER_CONTROL) == EDG_RTC_STATE_OK)
		{
			ptrhedgRTC->CurrentState = EDG_RTC_STATE_OK;
			ptrhedgRTC->DateState = EDG_RTC_DATE_STATE_OK;
			return EDG_RTC_STATE_OK;
		}

		else
		{
			ptrhedgRTC->CurrentState = EDG_RTC_STATE_ERROR;
			ptrhedgRTC->DateState = EDG_RTC_DATE_STATE_UNCONFIGURED;
			return EDG_RTC_STATE_ERROR;
		}
	}
	else
	{
		ptrhedgRTC->CurrentState = EDG_RTC_STATE_OK;
		ptrhedgRTC->DateState = EDG_RTC_DATE_STATE_UNCONFIGURED;
		return EDG_RTC_STATE_OK;
	}

#endif

}

/**
  * @brief Funcion para establecer el formato de la hora 12/24H
  * @note
  * @param  Address direccion I2C del RTC
  * @param  format Formato de la hora a configurar
  * @retval EDG_RTC_StateTypeDef Estado del RTC
  */
EDG_RTC_StateTypeDef EDG_RTC_SetHourFormat(EDG_RTC_HandleTypeDef * ptrhedgRTC, uint8_t Address)
{
	uint8_t temp = 0;
	uint8_t hour_adj = 0;

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

	if((ptrhedgRTC->DatetoChange.Format == EDG_RTC_HOUR_FORMAT_24H) || (ptrhedgRTC->DatetoChange.Format == EDG_RTC_HOUR_FORMAT_12H))
	{
		/* Se lee la hora actual para no modificar el dato */
		if(EDG_RTC_Read(Address, (uint16_t)EDG_RTC_REGISTER_HOURS, &temp, 1) == EDG_RTC_STATE_OK)
		{

			/* Para dejar formato de 12H se deja el bit 6 en 1 */
			if(ptrhedgRTC->DatetoChange.Format == EDG_RTC_HOUR_FORMAT_12H)
			{
				/* Se verifica si el formato actual es el mismo y se finaliza la funcion */
				if(temp & EDG_RTC_HOUR_FORMAT_MASK)
				{
					ptrhedgRTC->CurrentDate.Format = EDG_RTC_HOUR_FORMAT_12H;
					return EDG_RTC_STATE_OK;
				}

				/* se convierte el valor al formato correspondiente */
				hour_adj = (((temp >> 4) & 0x03) * 10) + (temp & 0x0F);

				/* Si la hora es mayor que 12 se verifica que no sean las 12 y se le resta 12 horas */
				if(hour_adj >= 12)
				{
					if(hour_adj != 12)
					{
						hour_adj -= 12;
					}
					/* Se cambia  a formato BCD y se pone el bit de AMPM en 1 */
					temp = ((((hour_adj / 10) & 0x01)<<4) | ((hour_adj % 10) & 0x0F)) | EDG_RTC_AMPM_MASK;
					ptrhedgRTC->CurrentDate.AmPm = EDG_RTC_AMPM_PM;

				}
				else
				{
					if(hour_adj == 0)
					{
						hour_adj = 12;
					}
					/* Si la hora es menor de 12 se pone el bit de AMPM en 0 */
					temp = ((((hour_adj / 10) & 0x01)<<4) | ((hour_adj % 10) & 0x0F)) & ~(EDG_RTC_AMPM_MASK);
					ptrhedgRTC->CurrentDate.AmPm = EDG_RTC_AMPM_PM;

				}
				/* Se pone el bit de 24 horas en 0 */
				temp |= EDG_RTC_HOUR_FORMAT_MASK;
				ptrhedgRTC->CurrentDate.Format = EDG_RTC_HOUR_FORMAT_12H;

			}
			/* Para dejar formato de 24H se deja el bit 6 en 0 */
			else if(ptrhedgRTC->DatetoChange.Format == EDG_RTC_HOUR_FORMAT_24H)
			{
				/* Se verifica si el formato actual es el mismo y se finaliza la funcion */
				if(!(temp & EDG_RTC_HOUR_FORMAT_MASK))
				{
					ptrhedgRTC->CurrentDate.Format = EDG_RTC_HOUR_FORMAT_24H;
					return EDG_RTC_STATE_OK;
				}

				/* se convierte el valor al formato correspondiente */
				hour_adj = (((temp >> 4) & 0x01) * 10) + (temp & 0x0F);

				/* Si la hora actual es PM se ajusta a formato de 24 horas */

				if(!(temp & EDG_RTC_AMPM_MASK) && (hour_adj == 12))
				{
					hour_adj = 0;
				}
				else if((temp & EDG_RTC_AMPM_MASK) && (hour_adj != 12))
				{
					hour_adj += 12;
				}
				/* Se ajusta el valor a BCD y se pone el bit de 24 horas en 0 */
				temp = (((hour_adj / 10) & 0x03)<<4) | ((hour_adj % 10) & 0x0F);
				temp &= ~(EDG_RTC_HOUR_FORMAT_MASK);
				ptrhedgRTC->CurrentDate.Format = EDG_RTC_HOUR_FORMAT_24H;

			}

			/* Se escribe el nuevo valor en el registo de horas del RTC */
			if(EDG_RTC_Write(Address, (uint16_t)EDG_RTC_REGISTER_HOURS, &temp, 1) == EDG_RTC_STATE_OK)
			{
				ptrhedgRTC->CurrentState = EDG_RTC_STATE_OK;
				return EDG_RTC_STATE_OK;
			}
			else
			{
				ptrhedgRTC->CurrentState = EDG_RTC_STATE_ERROR;
				return EDG_RTC_STATE_ERROR;
			}

		}
		else //Si la lectura del RTC no es correcta es porque hay error
		{
			ptrhedgRTC->CurrentState = EDG_RTC_STATE_ERROR;
			return EDG_RTC_STATE_ERROR;
		}

	}

#endif
	ptrhedgRTC->CurrentState = EDG_RTC_STATE_OK;
	return EDG_RTC_STATE_OK;

}

/**
  * @brief Funcion para obtener el formato de la hora 12/24H
  * @note
  * @param  Address direccion I2C del RTC
  * @param  format Puntero a variable donde se almacena el formato actual
  * @retval EDG_RTC_StateTypeDef Estado del RTC
  */
EDG_RTC_StateTypeDef EDG_RTC_GetHourFormat(EDG_RTC_HandleTypeDef * ptrhedgRTC, uint8_t Address)
{

	uint8_t temp = 0;

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

	/* Se lee la hora actual para no modificar el dato */
	if(EDG_RTC_Read(Address, (uint16_t)EDG_RTC_REGISTER_HOURS, &temp, 1) == EDG_RTC_STATE_OK)
	{

		/* Se verifica si el formato actual es el mismo */
		if(temp & EDG_RTC_HOUR_FORMAT_MASK)
		{
			ptrhedgRTC->CurrentDate.Format = EDG_RTC_HOUR_FORMAT_12H;
		}
		else
		{
			ptrhedgRTC->CurrentDate.Format = EDG_RTC_HOUR_FORMAT_24H;
		}
		ptrhedgRTC->CurrentState = EDG_RTC_STATE_OK;
		return EDG_RTC_STATE_OK;
	}
	else
	{
		ptrhedgRTC->CurrentState = EDG_RTC_STATE_ERROR;
		return EDG_RTC_STATE_ERROR;
	}

#endif

}

/**
  * @brief Funcion para revisar la comunicacion I2C con el RTC
  * @note
  * @param  Address direccion I2C del RTC
  * @retval EDG_RTC_StateTypeDef Estado del RTC
  */
EDG_RTC_StateTypeDef EDG_RTC_CheckCommI2C(EDG_RTC_HandleTypeDef * ptrhedgRTC, uint8_t Address)
{
	uint8_t temp = 0;

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

	/* Se comprueba que se pueda hacer una lectura del RTC */
	if(EDG_RTC_Read(Address, (uint16_t)EDG_RTC_REGISTER_SECONDS, &temp, 1) == EDG_RTC_STATE_OK)
	{
		ptrhedgRTC->CurrentState = EDG_RTC_STATE_OK;
		return EDG_RTC_STATE_OK;
	}
	else
	{
		ptrhedgRTC->CurrentState = EDG_RTC_STATE_ERROR;
		return EDG_RTC_STATE_ERROR;
	}

#endif

}

/**
  * @brief Funcion para revisar si el RTC esta funcionando correctamente
  * @note
  * @param  Address direccion I2C del RTC
  * @retval EDG_RTC_StateTypeDef Estado del RTC
  */
EDG_RTC_StateTypeDef EDG_RTC_CheckRun(EDG_RTC_HandleTypeDef * ptrhedgRTC, uint8_t Address)
{

	uint8_t temp = 0;
	uint8_t second , nextsecond = 0;

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

	/* VERIFICAR SI RTC ESTA CORRIENDO POR REGISTRO DE SEGUNDO & EDG_RTC_STOP_MASK */
	if(EDG_RTC_Read(Address, (uint16_t)EDG_RTC_REGISTER_SECONDS, &temp, 1) == EDG_RTC_STATE_OK)
	{
		/* Si el RTC esta detenido se escribe un 0x00 en el registro de segundos para activarlo */
		if(temp & EDG_RTC_STOP_MASK)
		{
			temp = 0x00;

			if(EDG_RTC_Write(Address, (uint16_t)EDG_RTC_REGISTER_SECONDS, &temp, 1) != EDG_RTC_STATE_OK)

			{
				ptrhedgRTC->CurrentState = EDG_RTC_STATE_ERROR;
				return EDG_RTC_STATE_ERROR;
			}

		}
		/* Despues de comprobar que el bit de STOP del RTC esta en el valor correcto (0 corriendo)
		 * se comprueba si si esta funcionando correctamente */

		if(EDG_RTC_Read(Address, (uint16_t)EDG_RTC_REGISTER_SECONDS, &temp, 1) == EDG_RTC_STATE_OK)
		{
			/* Se hacen dos lecturas de segundos con diferencias de 995ms y se comprueba si los valores son diferente*/
			second = (((temp >> 4) & 0x07) * 10) + (temp & 0x0F);
			HAL_Delay(995);

			if(EDG_RTC_Read(Address, (uint16_t)EDG_RTC_REGISTER_SECONDS, &temp, 1) == EDG_RTC_STATE_OK)
			{
				nextsecond = (((temp >> 4) & 0x07) * 10) + (temp & 0x0F);

				if(second < 59)
				{
					if(nextsecond > second)
					{
						ptrhedgRTC->CurrentState = EDG_RTC_STATE_OK;
						return EDG_RTC_STATE_OK;
					}
					else
					{
						ptrhedgRTC->CurrentState = EDG_RTC_STATE_ERROR;
						return EDG_RTC_STATE_ERROR;
					}
				}
				else
				{
					if(nextsecond < second)
					{
						ptrhedgRTC->CurrentState = EDG_RTC_STATE_OK;
						return EDG_RTC_STATE_OK;
					}
					else
					{
						ptrhedgRTC->CurrentState = EDG_RTC_STATE_ERROR;
						return EDG_RTC_STATE_ERROR;
					}
				}

			}
			else /* Si la lectura del RTC no es correcta es porque hay error */
			{
				ptrhedgRTC->CurrentState = EDG_RTC_STATE_ERROR;
				return EDG_RTC_STATE_ERROR;
			}


		}
		else /* Si la lectura del RTC no es correcta es porque hay error */
		{
			ptrhedgRTC->CurrentState = EDG_RTC_STATE_ERROR;
			return EDG_RTC_STATE_ERROR;
		}


	}
	else /* Si la lectura del RTC no es correcta es porque hay error */
	{
		ptrhedgRTC->CurrentState = EDG_RTC_STATE_ERROR;
		return EDG_RTC_STATE_ERROR;
	}

#endif

}

/**
  * @brief Funcion para revisar si hay una fecha actualizada, y si no, se establece una de base
  * @note
  * @param  Address direccion I2C del RTC
  * @retval EDG_RTC_StateTypeDef Estado del RTC
  */
EDG_RTC_StateTypeDef EDG_RTC_CheckDate(EDG_RTC_HandleTypeDef * ptrhedgRTC, uint8_t Address)
{

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

	/* Se verifica que el RTC este configurado con una fecha mayor o igual a la minima que deberia tener revisando
	 * el año que tiene configurado*/
	if(EDG_RTC_GetDate(ptrhedgRTC, Address) == EDG_RTC_STATE_OK)
	{

		/* Si el valor del año es menor que el actual se establece una fecha por defecto */
		if(ptrhedgRTC->CurrentDate.Year < EDG_RTC_TEMP_YEAR)
		{

			ptrhedgRTC->DatetoChange.Second = EDG_RTC_TEMP_SECONDS;
			ptrhedgRTC->DatetoChange.Minute = EDG_RTC_TEMP_MINUTES;
			ptrhedgRTC->DatetoChange.Hour =   EDG_RTC_TEMP_HOURS;
			ptrhedgRTC->DatetoChange.WeekDay = EDG_RTC_TEMP_WEEKDAY;
			ptrhedgRTC->DatetoChange.Day = EDG_RTC_TEMP_DAY;
			ptrhedgRTC->DatetoChange.Month = EDG_RTC_TEMP_MONTH;
			ptrhedgRTC->DatetoChange.Year = EDG_RTC_TEMP_YEAR;

			if(EDG_RTC_SetDate(ptrhedgRTC, Address) == EDG_RTC_STATE_OK)
			{
				ptrhedgRTC->CurrentState = EDG_RTC_STATE_OK;
				ptrhedgRTC->DateState = EDG_RTC_DATE_STATE_UNCONFIGURED;

				return EDG_RTC_STATE_OK;
			}
			else
			{
				ptrhedgRTC->CurrentState = EDG_RTC_STATE_ERROR;
				ptrhedgRTC->DateState = EDG_RTC_DATE_STATE_UNCONFIGURED;
				return EDG_RTC_STATE_ERROR;
			}


		}
		else /* Si el año es correcto no se debe ajustar */
		{
			ptrhedgRTC->CurrentState = EDG_RTC_STATE_OK;
			ptrhedgRTC->DateState = EDG_RTC_DATE_STATE_OK;
			return EDG_RTC_STATE_OK;
		}

	}
	else /* Si la lectura del RTC no es correcta es porque hay error */
	{
		ptrhedgRTC->CurrentState = EDG_RTC_STATE_ERROR;
		ptrhedgRTC->DateState = EDG_RTC_DATE_STATE_UNCONFIGURED;
		return EDG_RTC_STATE_ERROR;
	}

#endif

}
