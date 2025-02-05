/*
 * USR_TIM.h
 *
 *  Created on: 1/09/2018
 *      Author: edgh3
 */

#ifndef USR_TIM_H_
#define USR_TIM_H_

#include "tim.h"
#include "USR_NEXTION.h"
#include "USR_RTC.h"
#include "USR_PROCESADOR.h"

#define USR_TIM_BASE_TIEMPO_TIM17_SEG		5
#define USR_TIM_BASE_TIEMPO_TIM17			(USR_TIM_BASE_TIEMPO_TIM17_SEG * 1000)

/*30/11/21
 * Se agrega la macro de base de tiempo del PWM*/
#define USR_TIM_BASE_TIEMPO_PWM				8200U
#define USR_Reiniciar_Conteo_1minuto() 		__HAL_TIM_SET_COUNTER(&htim6, 0)
#define USR_TIM_Detener_Conteo_1minuto()	HAL_TIM_Base_Stop_IT(&htim6)


typedef enum xusr_tim_bandera_control_temperatura
{
	USR_TIM_BANDERA_CONTROL_TEMPERATURA_NO_CAMBIO,
	USR_TIM_BANDERA_CONTROL_TEMPERATURA_CAMBIO,

}usr_tim_bandera_control_temperatura;

extern usr_tim_bandera_control_temperatura usr_tim_estado_control_temperatura;

void USR_TIM_Iniciar_Conteo_1minuto(void);

#endif /* USR_TIM_H_ */
