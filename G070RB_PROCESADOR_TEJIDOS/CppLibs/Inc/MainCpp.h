/*
 * MainCpp.h
 *
 *  Created on: Jan 16, 2023
 *      Author: Elkin Granados
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAINCPP_H_
#define MAINCPP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include "main.h"
#include "usart.h"

#include "EDG_AC_CONTROL.h"
#include "EDG_CONFIG.h"
#include "EDG_MAX6675.h"
#include "EDG_DS18B20.h"
#include "EDG_MEMORY.h"
#include "EDG_RTC.h"
#include "EDG_TIM_CALLBACKS.h"
#include "EDG_TIMER.h"
#include "EDG_WS2812.h"
#include "EDG_STATE_MACHINE.h"
#include "EDG_UART_CALLBACKS.h"
#include "EDG_NEXTION.h"
#include "EDG_MEM_ADDR.h"
#include "EDG_SCHEDULE.h"
#include "EDG_BUZZER.h"

void InitCpp(void);
void MainCpp(void);

#ifdef __cplusplus
}
#endif

#endif /* MAINCPP_H_ */
