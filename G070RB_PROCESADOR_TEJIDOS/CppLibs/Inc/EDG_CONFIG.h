/*
 * EDG_CONFIG.h
 *
 *  Created on: 18/01/2023
 *      Author: Elkin Granados
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EDG_CONFIG_H_
#define EDG_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Definiciones para seleccionar el IDE	-------------------------------------*/

#define EDG_CONFIG_IDE_PLATFORMIO 			0
#define EDG_CONFIG_IDE_PLATFORMIO_PLUS_HAL	1
#define	EDG_CONFIG_IDE_CUBEIDE				2

#define EDG_CONFIG_IDE_SEL EDG_CONFIG_IDE_CUBEIDE

#define EDG_CONFIG_VERSION_FIRMWARE		"Firmware V2.0.1"

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#ifdef __cplusplus
}
#endif

#endif /* EDG_CONFIG_H_ */
