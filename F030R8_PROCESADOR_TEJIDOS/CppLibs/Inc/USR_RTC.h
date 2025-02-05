/*
 * USR_RTC.h
 *
 *  Created on: 1/09/2018
 *      Author: edgh3
 */

#ifndef USR_RTC_H_
#define USR_RTC_H_


//Se incluyen los archivos cabeceras necesarios

#include "i2c.h"

#define USR_RTC_DEV_ADDRESS						0x68U
#define USR_RTC_STOP_FLAG						0x20U
#define USR_RTC_12H_MASK						0x40U
#define USR_RTC_AMPM_MASK						0x20U
#define USR_RTC_AMPM_POS						5U
#define USR_RTC_REGISTRO_TAM					8U
#define USR_RTC_ANHO_ACTUAL						18U


typedef enum xusr_rtc_posicion
{

	USR_RTC_POSICION_SEGUNDO,
	USR_RTC_POSICION_MINUTO,
	USR_RTC_POSICION_HORA,
	USR_RTC_POSICION_DIA_SEMANA,
	USR_RTC_POSICION_DIA,
	USR_RTC_POSICION_MES,
	USR_RTC_POSICION_ANHO,
	USR_RTC_POSICION_CONFIGURACION,

}usr_rtc_posicion;

typedef enum xusr_rtc_estado
{

	USR_RTC_ESTADO_ERROR,
	USR_RTC_ESTADO_OK,

}usr_rtc_estado;

typedef enum xusr_rtc_dia_semana
{

	USR_RTC_DIA_LUNES = 1,
	USR_RTC_DIA_MARTES,
	USR_RTC_DIA_MIERCOLES,
	USR_RTC_DIA_JUEVES,
	USR_RTC_DIA_VIERNES,
	USR_RTC_DIA_SABADO,
	USR_RTC_DIA_DOMINGO,

}usr_rtc_dia_semana;

typedef enum xusr_rtc_mes
{

	USR_RTC_MES_ENERO = 1,
	USR_RTC_MES_FEBRERO,
	USR_RTC_MES_MARZO,
	USR_RTC_MES_ABRIL,
	USR_RTC_MES_MAYO,
	USR_RTC_MES_JUNIO,
	USR_RTC_MES_JULIO,
	USR_RTC_MES_AGOSTO,
	USR_RTC_MES_SEPTIEMBRE,
	USR_RTC_MES_OCTUBRE,
	USR_RTC_MES_NOVIEMBRE,
	USR_RTC_MES_DICIEMBRE,

}usr_rtc_mes;


extern uint8_t usr_rtc_config[8];
extern uint8_t usr_rtc_set_reg[1];
extern uint8_t usr_rtc_read[8];

extern usr_rtc_estado usr_rtc_estado_actual;

void USR_RTC_Leer_Fecha_Hora(void);
void USR_RTC_Escribir_Fecha_Hora(void);
usr_rtc_estado USR_RTC_Comparar(void);

#endif /* USR_RTC_H_ */
