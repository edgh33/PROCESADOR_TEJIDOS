/*
 * USR_TEMPERATURA.h
 *
 *  Created on: 4/09/2018
 *      Author: edgh3
 */

#ifndef USR_TEMPERATURA_H_
#define USR_TEMPERATURA_H_

#include "spi.h"
#include "tim.h"
//#include <stdio.h>
#include "USR_TIM.h"
#include "USR_NEXTION.h"
#include "USR_FLASH.h"

#define USR_TEMPERATURA_CANTIDAD_TERMOCUPLAS		2U
#define USR_TEMPERATURA_CANTIDAD_MAX_TERMOCUPLAS	6U
#define USR_TEMPERATURA_INTENTOS_LECTURA			10U
#define USR_TEMPERATURA_MAXIMA						90U

/*30/11/21
 * Se agregan las macros para los valores maximos y minimos del PID*/
#define USR_TEMPERATURA_PID_MAX						(USR_TIM_BASE_TIEMPO_PWM - 1)
#define USR_TEMPERATURA_PID_MIN						1U

#define USR_TEMPERATURA_PID_P						40.0
#define USR_TEMPERATURA_PID_I						0.5
#define USR_TEMPERATURA_PID_D						80

/*09/02/22
 * Se agrega una macro para habilitar o no el uso de un offset para la temperatura
 * El offset se va a calcular como Tactual = Tactual + (Tactual / 10)*/
#define USR_TEMPERATURA_HABILITAR_OFFSET		1U 	// 0: offset deshabilitado, 1: offset habilitado

#define USR_TEMPERATURA_POSICION_CANTIDAD		1U
#define USR_TEMPERATURA_POSICION_TEMOCUPLA_1	2U

 #define USR_TEMPERATURA_MASK_DETENER_MEDICION 	0x00
 #define USR_TEMPERATURA_MASK_LECTURA_SIMPLE 	0x40
 #define USR_TEMPERATURA_MASK_LECTURA_CONTINUA 	0x80
 #define USR_TEMPERATURA_MASK_FAULT_HABILITADO 	0x10

 #define USR_TEMPERATURA_MASK_TERMOCUPLA_ABIERTA	0x01

 //Valor para chip_select
 #define  USR_TEMPERATURA_CHIP_TERMOCUPLA_1 0x01
 #define  USR_TEMPERATURA_CHIP_TERMOCUPLA_2 0x02
 #define  USR_TEMPERATURA_CHIP_TERMOCUPLA_3 0x03
 #define  USR_TEMPERATURA_CHIP_TERMOCUPLA_4 0x04
 #define  USR_TEMPERATURA_CHIP_TERMOCUPLA_5 0x05
 #define  USR_TEMPERATURA_CHIP_TERMOCUPLA_6 0x06

 //Direcciones de memoria del MAX31856
 #define USR_TEMPERATURA_DIR_CR0_LECTURA 		0x00
 #define USR_TEMPERATURA_DIR_CR1_LECTURA 		0x01
 #define USR_TEMPERATURA_DIR_MASK_LECTURA 		0x02
 #define USR_TEMPERATURA_DIR_CJHF_LECTURA 		0x03
 #define USR_TEMPERATURA_DIR_CJLF_LECTURA 		0x04
 #define USR_TEMPERATURA_DIR_LTHFTH_LECTURA 	0x05
 #define USR_TEMPERATURA_DIR_LTHFTL_LECTURA 	0x06
 #define USR_TEMPERATURA_DIR_LTLFTH_LECTURA 	0x07
 #define USR_TEMPERATURA_DIR_LTLFTL_LECTURA 	0x08
 #define USR_TEMPERATURA_DIR_CJTO_LECTURA 		0x09
 #define USR_TEMPERATURA_DIR_CJTH_LECTURA 		0x0A
 #define USR_TEMPERATURA_DIR_CJTL_LECTURA 		0x0B
 #define USR_TEMPERATURA_DIR_LTCBH_LECTURA 		0x0C
 #define USR_TEMPERATURA_DIR_LTCBM_LECTURA 		0x0D
 #define USR_TEMPERATURA_DIR_LTCBL_LECTURA 		0x0E
 #define USR_TEMPERATURA_DIR_SR_LECTURA 		0x0F

 #define USR_TEMPERATURA_DIR_CR0_ESCRITURA 		0x80
 #define USR_TEMPERATURA_DIR_CR1_ESCRITURA 		0x81
 #define USR_TEMPERATURA_DIR_MASK_ESCRITURA 	0x82
 #define USR_TEMPERATURA_DIR_CJHF_ESCRITURA 	0x83
 #define USR_TEMPERATURA_DIR_CJLF_ESCRITURA 	0x84
 #define USR_TEMPERATURA_DIR_LTHFTH_ESCRITURA 	0x85
 #define USR_TEMPERATURA_DIR_LTHFTL_ESCRITURA 	0x86
 #define USR_TEMPERATURA_DIR_LTLFTH_ESCRITURA 	0x87
 #define USR_TEMPERATURA_DIR_LTLFTL_ESCRITURA 	0x88
 #define USR_TEMPERATURA_DIR_CJTO_ESCRITURA 	0x89
 #define USR_TEMPERATURA_DIR_CJTH_ESCRITURA 	0x8A
 #define USR_TEMPERATURA_DIR_CJTL_ESCRITURA 	0x8B

typedef int16_t usr_temperatura_dato;

 //Tipo de termocupla
typedef enum xusr_temperatura_tipo_termocupla
{

	USR_TEMPERATURA_TERMOCUPLA_B,
	USR_TEMPERATURA_TERMOCUPLA_E,
	USR_TEMPERATURA_TERMOCUPLA_J,
	USR_TEMPERATURA_TERMOCUPLA_K,
	USR_TEMPERATURA_TERMOCUPLA_N,
	USR_TEMPERATURA_TERMOCUPLA_R,
	USR_TEMPERATURA_TERMOCUPLA_S,
	USR_TEMPERATURA_TERMOCUPLA_T,

}usr_temperatura_tipo_termocupla;

//Tipo de muestreo
typedef enum xusr_temperatura_muestreo
{

	USR_TEMPERATURA_MUESTREO_1SAMPLE,
	USR_TEMPERATURA_MUESTREO_2SAMPLES,
	USR_TEMPERATURA_MUESTREO_4SAMPLES,
	USR_TEMPERATURA_MUESTREO_8SAMPLES,
	USR_TEMPERATURA_MUESTREO_16SAMPLES,

}usr_temperatura_muestreo;

typedef enum xusr_temperatura_estado_termocupla
{

	USR_TEMPERATURA_ESTADO_TERMOCUPLA_OK,
	USR_TEMPERATURA_ESTADO_TERMOCUPLA_ERROR,
	USR_TEMPERATURA_ESTADO_TERMOCUPLA_ABIERTA,
	USR_TEMPERATURA_ESTADO_TERMOCUPLA_SOBRETEMPERATURA,

}usr_temperatura_estado_termocupla;


typedef enum xusr_temperatura_estado_control
{

	USR_TEMPERATURA_ESTADO_CONTROL_INACTIV0,
	USR_TEMPERATURA_ESTADO_CONTROL_ACTIV0,

}usr_temperatura_estado_control;

typedef enum xusr_temperatura_estado_pwm
{

	USR_TEMPERATURA_ESTADO_PWM_INACTIV0,
	USR_TEMPERATURA_ESTADO_PWM_ACTIV0,

}usr_temperatura_estado_pwm;

typedef enum xusr_temperatura_tipo_control
{

	USR_TEMPERATURA_TIPO_CONTROL_ONOFF,
	USR_TEMPERATURA_TIPO_CONTROL_PID,

}usr_temperatura_tipo_control;


typedef enum xusr_temperatura_termocupla
{

	USR_TEMPERATURA_TERMOCUPLA_1,
	USR_TEMPERATURA_TERMOCUPLA_2,
	USR_TEMPERATURA_TERMOCUPLA_3,
	USR_TEMPERATURA_TERMOCUPLA_4,
	USR_TEMPERATURA_TERMOCUPLA_5,
	USR_TEMPERATURA_TERMOCUPLA_6,

}usr_temperatura_termocupla;


typedef enum xusr_temperatura_led
{

	USR_TEMPERATURA_LED_1,
	USR_TEMPERATURA_LED_2,
	USR_TEMPERATURA_LED_3,
	USR_TEMPERATURA_LED_4,
	USR_TEMPERATURA_LED_5,
	USR_TEMPERATURA_LED_6,

}usr_temperatura_led;


typedef enum xusr_temperatura_estado_led
{

	USR_TEMPERATURA_ESTADO_LED_APAGADO,
	USR_TEMPERATURA_ESTADO_LED_VERDE,
	USR_TEMPERATURA_ESTADO_LED_ROJO,
	USR_TEMPERATURA_ESTADO_LED_NARANJA,

}usr_temperatura_estado_led;

typedef enum xusr_temperatura_alerta
{

	USR_TEMPERATURA_ALERTA_INACTIVA,
	USR_TEMPERATURA_ALERTA_ACTIVA,

}usr_temperatura_alerta;

typedef struct xusr_struc_pid
{

	float kp;
	float ki;
	float kd;
	float Error; //Error = Sensor - Referencia
	float Proporcional; //Proporcional = Error * Kp
	float Integral; //Integral = Integral + Error * Ki * T
	float Derivativo; //Derivativo= (Error - Error_0) * Kd / T
	float Error_0; //Error_0 = Error
	uint16_t Control; //Control = Proporcional + Integral + Derivativo

}usr_struc_pid;

typedef struct xusr_struc_termocupla
{

	uint8_t numero_termocupla;
	usr_temperatura_dato temperatura_configurada;
	usr_temperatura_dato temperatura_actual;
	usr_temperatura_estado_termocupla estado_termocupla;
	usr_temperatura_tipo_termocupla tipo_termocupla;
	usr_temperatura_muestreo tipo_muestreo;
	usr_temperatura_estado_control estado_control;
	usr_temperatura_estado_pwm estado_pwm;
	usr_temperatura_tipo_control tipo_control;
	uint16_t porcentaje_potencia;
	usr_struc_pid pid;

}usr_struc_termocupla;



/*30/11/21
 * Se cambia tamaño del arreglo de las termocuplas a USR_TEMPERATURA_CANTIDAD_MAX_TERMOCUPLAS*/
extern usr_struc_termocupla arreglo_termocuplas[USR_TEMPERATURA_CANTIDAD_MAX_TERMOCUPLAS];


extern uint8_t contador_termocuplas;
extern uint8_t cantidad_termocuplas;

extern usr_temperatura_alerta usr_temperatura_estado_alerta;

void USR_TEMPERATURA_Iniciar_Chips(void);
void USR_TEMPERATURA_Configurar_MAX31856(usr_struc_termocupla * prvusr_temperatura_termocupla);
void USR_TEMPERATURA_Leer_Temperatura(usr_struc_termocupla * prvusr_temperatura_termocupla);
void USR_TEMPERATURA_Iniciar_Control(void);
void USR_TEMPERATURA_Control(void);

/*03/12/21
 * Se crea una funcion para revisar si el rele debe o no estar activo*/
/*02/03/22
 * Esta funcion solo estaba en void USR_NEXTION_Temperatura(void), se elimina de esta funcion*/
void USR_TEMPERATURA_ESTADO_RELE(void);

/*03/12/21
 * Se crean estas funciones para el control PID y ONOFF*/
void USR_TEMPERATURA_Control_PID(usr_struc_termocupla * prvusr_temperatura_termocupla);
void USR_TEMPERATURA_Control_ONOFF(usr_struc_termocupla * prvusr_temperatura_termocupla);
void USR_TEMPERATURA_PID(usr_struc_termocupla * prvusr_temperatura_termocupla);
void USR_TEMPERATURA_Detener_Control(void);

void USR_TEMPERATURA_Desactivar_Termocupla(usr_temperatura_termocupla termocupla);
void USR_TEMPERATURA_Visualizar_Temperatura_Actual(void);

usr_temperatura_estado_termocupla USR_TEMPERATURA_Revisar_Estado(void);
void USR_TEMPERATURA_CS_Posicion(GPIO_PinState onoff, uint8_t pin);
void USR_TEMPERATURA_Cambio_Led(usr_temperatura_led led, usr_temperatura_estado_led estado);

#endif /* USR_TEMPERATURA_H_ */
