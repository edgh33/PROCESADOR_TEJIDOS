/*
 * EDG_UART_CALLBACKS.h
 *
 *  Created on: 08/04/2023
 *      Author: Elkin Granados
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EDG_UART_CALLBACKS_H_
#define EDG_UART_CALLBACKS_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "EDG_CONFIG.h"

#if EDG_CONFIG_IDE_SEL == EDG_CONFIG_IDE_CUBEIDE

#include "main.h"

#endif

/* Include the headers files related with uart callbacks  ----------------------*/
#include "EDG_NEXTION.h"

#ifdef __cplusplus
}
#endif


#endif /* EDG_UART_CALLBACKS_H_ */
