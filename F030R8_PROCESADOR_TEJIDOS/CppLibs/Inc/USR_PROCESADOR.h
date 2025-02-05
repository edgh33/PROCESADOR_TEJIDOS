/*
 * USR_PROCESADOR.h
 *
 *  Created on: 1/09/2018
 *      Author: edgh3
 */

#ifndef USR_PROCESADOR_H_
#define USR_PROCESADOR_H_

#include "main.h"
#include "gpio.h"
//#include <stdio.h>
#include "USR_NEXTION.h"
#include "USR_TIM.h"
#include "USR_FLASH.h"


#define USR_PROCESADOR_DESHABILITAR_MOTOR	0U  //0: control de motor activo, 1: control de motor inactivo
#define USR_PROCESADOR_VERSION_FW			1U  //0: version con 2 reles y sensor, 1: version con 1 rele 1 sensor
#define USR_PROCESADOR_NUMERO_ELEMENTOS 	12U	//Cantidad de canastas
#define USR_PROCESADOR_TIMEOUT_CAMBIOS 		45000U
#define USR_PROCESADOR_TIMEOUT_ROTACION 	60000U

#define USR_PROCESADOR_TIMEOUT_ROTACION_E1 	5000U
#define USR_PROCESADOR_TIMEOUT_ROTACION_E2	50000U
#define USR_PROCESADOR_TIMEOUT_ROTACION_E3 	18000U
//
#define USR_PROCESADOR_RELE_AC_ON_ON() 			HAL_GPIO_WritePin(RELE_ON_GPIO_Port, RELE_ON_Pin, GPIO_PIN_SET);
#define USR_PROCESADOR_RELE_AC_ON_OFF() 		HAL_GPIO_WritePin(RELE_ON_GPIO_Port, RELE_ON_Pin, GPIO_PIN_RESET);
#define USR_PROCESADOR_RELE_AC_OFF_ON() 		HAL_GPIO_WritePin(RELE_OFF_GPIO_Port, RELE_OFF_Pin, GPIO_PIN_SET);
#define USR_PROCESADOR_RELE_AC_OFF_OFF() 		HAL_GPIO_WritePin(RELE_OFF_GPIO_Port, RELE_OFF_Pin, GPIO_PIN_RESET);

#define USR_PROCESADOR_RELE_START_ACTIVO() 		HAL_GPIO_WritePin(RELE_START_GPIO_Port, RELE_START_Pin, GPIO_PIN_SET);
#define USR_PROCESADOR_RELE_START_INACTIVO() 	HAL_GPIO_WritePin(RELE_START_GPIO_Port, RELE_START_Pin, GPIO_PIN_RESET);
#define USR_PROCESADOR_RELE_MANUAL_ACTIVO() 	HAL_GPIO_WritePin(RELE_MANUAL_GPIO_Port, RELE_MANUAL_Pin, GPIO_PIN_SET);
#define USR_PROCESADOR_RELE_MANUAL_INACTIVO() 	HAL_GPIO_WritePin(RELE_MANUAL_GPIO_Port, RELE_MANUAL_Pin, GPIO_PIN_RESET);

#define USR_PROCESADOR_ESTADO_SENSOR_AC() 		HAL_GPIO_ReadPin(SENSOR_AC_GPIO_Port, SENSOR_AC_Pin)

/*09/02/22
 * Se agrega la macro de leer el estado del rele manual que ahora va a activar las resistencias*/
#define USR_PROCESADOR_ESTADO_RELE_MANUAL() 	HAL_GPIO_ReadPin(RELE_MANUAL_GPIO_Port, RELE_MANUAL_Pin)

#define USR_PROCESADOR_BUZZER_OFF()				HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET)

typedef enum xusr_procesador_posicion
{

	USR_PROCESADOR_POSICION_COMANDO,
	USR_PROCESADOR_POSICION_PROGRAMA,
	USR_PROCESADOR_POSICION_RECIPIENTES,
	USR_PROCESADOR_POSICION_R1_HORA,
	USR_PROCESADOR_POSICION_R1_MINUTO,
	USR_PROCESADOR_POSICION_R2_HORA,
	USR_PROCESADOR_POSICION_R2_MINUTO,
	USR_PROCESADOR_POSICION_R3_HORA,
	USR_PROCESADOR_POSICION_R3_MINUTO,
	USR_PROCESADOR_POSICION_R4_HORA,
	USR_PROCESADOR_POSICION_R4_MINUTO,
	USR_PROCESADOR_POSICION_R5_HORA,
	USR_PROCESADOR_POSICION_R5_MINUTO,
	USR_PROCESADOR_POSICION_R6_HORA,
	USR_PROCESADOR_POSICION_R6_MINUTO,
	USR_PROCESADOR_POSICION_R7_HORA,
	USR_PROCESADOR_POSICION_R7_MINUTO,
	USR_PROCESADOR_POSICION_R8_HORA,
	USR_PROCESADOR_POSICION_R8_MINUTO,
	USR_PROCESADOR_POSICION_R9_HORA,
	USR_PROCESADOR_POSICION_R9_MINUTO,
	USR_PROCESADOR_POSICION_R10_HORA,
	USR_PROCESADOR_POSICION_R10_MINUTO,
	USR_PROCESADOR_POSICION_R11_HORA,
	USR_PROCESADOR_POSICION_R11_MINUTO,
	USR_PROCESADOR_POSICION_R12_HORA,
	USR_PROCESADOR_POSICION_R12_MINUTO,
	USR_PROCESADOR_POSICION_RETARDO_HORAS,
	USR_PROCESADOR_POSICION_RETARDO_MINUTOS,
	USR_PROCESADOR_POSICION_T1_ESTADO,
	USR_PROCESADOR_POSICION_T1_VALOR,
	USR_PROCESADOR_POSICION_T2_ESTADO,
	USR_PROCESADOR_POSICION_T2_VALOR,
	USR_PROCESADOR_POSICION_POSICION_ACTUAL,
	USR_PROCESADOR_POSICION_ESTADO_ACTUAL,


}usr_procesador_posicion;

typedef enum xusr_procesador_estado
{

	USR_PROCESADOR_ESTADO_DETENIDO,
	USR_PROCESADOR_ESTADO_EJECUTANDO,
	USR_PROCESADOR_ESTADO_PAUSADO,
	USR_PROCESADOR_ESTADO_ALERTA,
	USR_PROCESADOR_ESTADO_ROTANDO,

}usr_procesador_estado;

typedef enum xusr_procesador_primera_ejecucion
{

	USR_PROCESADOR_BANDERA_PRIMERA_EJECUCION,
	USR_PROCESADOR_BANDERA_NO_PRIMERA_EJECUCION,

}usr_procesador_primera_ejecucion;

typedef enum xusr_procesador_bandera_minuto
{

	USR_PROCESADOR_BANDERA_MINUTO_CORRIENDO,
	USR_PROCESADOR_BANDERA__MINUTO_COMPLETO,

}usr_procesador_bandera_minuto;

typedef enum xusr_procesador_manual
{

	USR_PROCESADOR_MANUAL_BAJAR_CANASTA,
	USR_PROCESADOR_MANUAL_ELEVAR_CANASTA,

}usr_procesador_manual;

typedef enum xusr_procesador_releac
{

	USR_PROCESADOR_RELEAC_INACTIVO,
	USR_PROCESADOR_RELEAC_ACTIVO,

}usr_procesador_releac;


extern uint32_t usr_procesador_recipiente_actual;
extern uint32_t usr_procesador_recipiente_cantidad;

extern uint32_t usr_procesador_recipiente_hora_actual;
extern uint32_t usr_procesador_recipiente_minuto_actual;
extern uint32_t usr_procesador_tiempo_total_recipiente_actual;

extern uint32_t usr_procesador_delay_hora_actual;
extern uint32_t usr_procesador_delay_minuto_actual;
extern uint32_t usr_procesador_tiempo_delay_total_minutos;

extern usr_procesador_estado usr_procesador_estado_actual;
extern usr_procesador_primera_ejecucion usr_procesador_bandera_ejecucion;
extern usr_procesador_bandera_minuto usr_procesador_bandera_estado_minuto;


void USR_PROCESADOR_Ejecutar_Programa(void);
void USR_PROCESADOR_Pausar_Programa(void);
void USR_PROCESADOR_Detener_Programa(void);
void USR_PROCESADOR_Cambiar_Posicion(void);
void USR_PROCESADOR_Elevar_Canasta(void);
void USR_PROCESADOR_Bajar_Canasta(void);
void USR_PROCESADOR_Buzzer(void);
void USR_PROCESADOR_ReleAC(usr_procesador_releac estado);

void USR_PROCESADOR_Retomar_Programa_Actual(void);

#endif /* USR_PROCESADOR_H_ */
