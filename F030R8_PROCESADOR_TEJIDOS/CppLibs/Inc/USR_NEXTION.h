/*
 * USR_NEXTION.h
 *
 *  Created on: 1/09/2018
 *      Author: edgh3
 */

#ifndef USR_NEXTION_H_
#define USR_NEXTION_H_

//Se incluyen los archivos cabeceras necesarios

#include "string.h"
#include "stdlib.h"
#include "usart.h"
//#include <stdio.h>
#include "USR_RTC.h"
#include "USR_FLASH.h"
#include "USR_PROCESADOR.h"
#include "USR_TEMPERATURA.h"
#include "USR_TIM.h"

#define USR_NEXTION_BUFFER_RX_TAM			96

#define USR_NEXTION_Habilitar_Touch_Pantalla()		usr_nextion_status = USR_NEXTION_Enviar_Trama(usr_nextion_com_todo_habilitado, (uint16_t)strlen((const char *)usr_nextion_com_todo_habilitado))
#define USR_NEXTION_Inhabilitar_Touch_Pantalla()	usr_nextion_status = USR_NEXTION_Enviar_Trama(usr_nextion_com_todo_inhabilitado, (uint16_t)strlen((const char *)usr_nextion_com_todo_inhabilitado))
typedef enum xusr_nextion_pantallas
{

	USR_NEXTION_PANTALLA_INICIO=0,
	USR_NEXTION_PANTALLA_MENU,
	USR_NEXTION_PANTALLA_EJECUTAR,
	USR_NEXTION_PANTALLA_MANUAL,
	USR_NEXTION_PANTALLA_CONFIGURAR,
	USR_NEXTION_PANTALLA_CALENDARIO,
	USR_NEXTION_PANTALLA_RELOJ,
	USR_NEXTION_PANTALLA_ALERTA,
	USR_NEXTION_PANTALLA_TERMOCUPLAS,


}usr_nextion_pantallas;

typedef enum xusr_nextion_comandos
{

	USR_NEXTION_COMANDO_PANTALLA=0,
	USR_NEXTION_COMANDO_PROGRAMA,
	USR_NEXTION_COMANDO_EJECUTAR,
	USR_NEXTION_COMANDO_PAUSAR,
	USR_NEXTION_COMANDO_DETENER,
	USR_NEXTION_COMANDO_TEMPERATURA,
	USR_NEXTION_COMANDO_MANUAL,
	USR_NEXTION_COMANDO_GUARDAR_PROGRAMA,
	USR_NEXTION_COMANDO_GUARDAR_RTC,
	USR_NEXTION_COMANDO_BUZZER,
	USR_NEXTION_COMANDO_REINICIAR_MEMORIA,
	USR_NEXTION_COMANDO_GUARDAR_TERMOCUPLAS,

}usr_nextion_comandos;

typedef enum xusr_nextion_trama
{

	USR_NEXTION_TRAMA_NO_RECIBIDA=0,
	USR_NEXTION_TRAMA_RECIBIDA,

}usr_nextion_trama;

typedef enum xusr_nextion_posicion
{

	USR_NEXTION_POSICION_COMANDO,
	USR_NEXTION_POSICION_PANTALLA,
	USR_NEXTION_POSICION_DELAY_HORAS,
	USR_NEXTION_POSICION_DELAY_MINUTOS,

}usr_nextion_posicion;

typedef enum xusr_nextion_posicion_rtc
{

	USR_NEXTION_POSICION_RTC_MINUTO=1,
	USR_NEXTION_POSICION_RTC_HORA,
	USR_NEXTION_POSICION_RTC_DIA_SEMANA,
	USR_NEXTION_POSICION_RTC_DIA,
	USR_NEXTION_POSICION_RTC_MES,
	USR_NEXTION_POSICION_RTC_ANHO,
	USR_NEXTION_POSICION_RTC_AMPM,

}usr_nextion_posicion_rtc;

typedef enum xusr_nextion_temperatura_posicion
{

	USR_NEXTION_TEMPERATURA_POSICION_TERMOCUPLA=1,
	USR_NEXTION_TEMPERATURA_POSICION_CONTROL,
	USR_NEXTION_TEMPERATURA_POSICION_VALOR,

}usr_nextion_temperatura_posicion;

typedef enum xusr_nextion_temperatura_accion
{

	USR_NEXTION_TEMPERATURA_ACCION_APAGAR,
	USR_NEXTION_TEMPERATURA_ACCION_ENCENDER,

}xusr_nextion_temperatura_accion;


extern uint8_t usr_nextion_buffer_rx[USR_NEXTION_BUFFER_RX_TAM];
extern uint8_t usr_nextion_pantalla_actual;
extern uint8_t usr_nextion_texto_programa_finalizado[];
extern uint8_t usr_nextion_com_vison_bt14[];
extern uint8_t usr_nextion_com_visoff_bt14[];
extern uint8_t usr_nextion_com_habilita_b6[];
extern uint8_t usr_nextion_com_deshabilita_b6[];
extern uint8_t usr_nextion_com_visoff_bt14[];
extern uint8_t usr_nextion_com_todo_habilitado[];
extern uint8_t usr_nextion_com_todo_inhabilitado[];
extern HAL_StatusTypeDef usr_nextion_status;

extern uint8_t usr_nextion_texto_programa_finalizado[];
extern uint8_t usr_nextion_texto_programa_ejecutando[];
extern uint8_t usr_nextion_texto_programa_pausado[];
extern uint8_t usr_nextion_texto_programa_rotando[];
extern uint8_t usr_nextion_texto_programa_retardo[];

extern uint8_t usr_nextion_texto_canasta_arriba[];
extern uint8_t usr_nextion_texto_canasta_abajo[];

extern uint8_t usr_nextion_com_vison_b0[];
extern uint8_t usr_nextion_com_visoff_b0[];

extern uint8_t usr_nextion_com_vison_bt0[];
extern uint8_t usr_nextion_com_visoff_bt0[];

extern uint8_t usr_nextion_com_habilita_bt20[];
extern uint8_t usr_nextion_com_deshabilita_bt20[];

extern uint8_t usr_nextion_com_habilita_bt21[];
extern uint8_t usr_nextion_com_deshabilita_bt21[];

extern uint8_t usr_nextion_texto_menu_ok[];
extern uint8_t usr_nextion_texto_menu_error_termocuplas[];
extern uint8_t usr_nextion_texto_menu_error_rtc[];

extern uint8_t usr_nextion_pantalla_actual;
extern uint8_t usr_nextion_com_pantalla_ejecutar[];

//Se declaran las funciones necesarias
HAL_StatusTypeDef USR_NEXTION_Iniciar_Recibir_Trama(void);
HAL_StatusTypeDef USR_NEXTION_Detener_Recibir_Trama(void);
HAL_StatusTypeDef USR_NEXTION_Recibir_Trama(void);
HAL_StatusTypeDef USR_NEXTION_Cambiar_Pantalla(uint32_t pantalla);
HAL_StatusTypeDef USR_NEXTION_Enviar_Trama(uint8_t* trama_enviar, uint16_t tam_trama);
uint32_t USR_NEXTION_Obtener_Dato_Trama(uint32_t posicion);
void USR_NEXTION_Guardar_RTC(void);
void USR_NEXTION_Visualizar_RTC(void);
void USR_NEXTION_Visualizar_Alerta(void);
void USR_NEXTION_Visualizar_Termocuplas(void);
void USR_NEXTION_Temperatura(void);
void USR_NEXTION_Verificar_Temperatura(void);
void USR_NEXTION_Verificar_RTC(void);
void USR_NEXTION_Mostrar_Programa(uint8_t numProg);
void USR_NEXTION_Mostrar_Programa_Retomado(void);
void USR_NEXTION_Activar_Recipiente(uint8_t recipiente);
void USR_NEXTION_Ejecutar_Comando(void);
void USR_NEXTION_Administrador(void);



#endif /* USR_NEXTION_H_ */
