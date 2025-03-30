/*
 * EDG_MEM_ADDR.h
 *
 *  Created on: 11/04/2023
 *      Author: Elkin Granados
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EDG_MEM_ADDR_H_
#define EDG_MEM_ADDR_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Definiciones para seleccionar el IDE	-------------------------------------*/

#define EDG_MEM_ADDR_BASE					(0x0110U) //Not 0x000 for issues with 0xFF position
#define	EDG_MEM_ADDR_OFFSET					(0x0000)

#define EDG_MEM_ADDR_PROGRAM_QTY			(10U)
#define EDG_MEM_ADDR_POS_X_PROGRAM_VALUE	(1U)
#define EDG_MEM_ADDR_VALUES_X_PROGRAM		(30U) //Active container (1), Hours and minutes by container (12*2), drip seconds (1), t1 state (1), t1 value (1), t2 state (1), t2 value (1)
#define EDG_MEM_ADDR_PROGRAM_OFFSET			(EDG_MEM_ADDR_POS_X_PROGRAM_VALUE * EDG_MEM_ADDR_VALUES_X_PROGRAM)
#define EDG_MEM_ADDR_PROGRAM_TOTAL_SIZE		(EDG_MEM_ADDR_PROGRAM_QTY * EDG_MEM_ADDR_PROGRAM_OFFSET)

#define EDG_MEM_ADDR_SCHEDULE_QTY			(7U) //Week days
#define EDG_MEM_ADDR_POS_X_SCHEDULE_VALUE	(1U)
#define EDG_MEM_ADDR_VALUES_X_SCHEDULE		(5U) // State of day active or inactuve(1), Hour (1), Minutes (1), AmPm (1), Program (1)
#define EDG_MEM_ADDR_SCHEDULE_OFFSET		(EDG_MEM_ADDR_POS_X_SCHEDULE_VALUE * EDG_MEM_ADDR_VALUES_X_SCHEDULE)
#define EDG_MEM_ADDR_SCHEDULE_TOTAL_SIZE	(EDG_MEM_ADDR_SCHEDULE_QTY * EDG_MEM_ADDR_SCHEDULE_OFFSET)

#define EDG_MEM_ADDR_OFFSET_QTY				(1U)
#define EDG_MEM_ADDR_POS_X_OFFSET_VALUE		(1U)
#define EDG_MEM_ADDR_VALUES_X_OFFSET		(6U)
#define EDG_MEM_ADDR_OFFSET_OFFSET			(EDG_MEM_ADDR_VALUES_X_OFFSET * EDG_MEM_ADDR_POS_X_OFFSET_VALUE)
#define EDG_MEM_ADDR_OFFSET_TOTAL_SIZE		(EDG_MEM_ADDR_OFFSET_QTY * EDG_MEM_ADDR_OFFSET_OFFSET)

#define EDG_MEM_ADDR_CURR_PROC_QTY			(1U)
#define EDG_MEM_ADDR_POS_X_CURRENT_PROC  	(1U)
#define EDG_MEM_ADDR_VALUES_X_CURRENT_PROC	(34U) //Hours and minutes by container (12*2), drip seconds (1), t1 state (1), t1 value (1), t2 state (1), t2 value (1),
												  //current contaniner(1), carousel pos(1), delay hour(1), delay min(1), active run? (1)
#define EDG_MEM_ADDR_CURR_PROC_OFFSET		(EDG_MEM_ADDR_POS_X_CURRENT_PROC * EDG_MEM_ADDR_VALUES_X_CURRENT_PROC)
#define EDG_MEM_ADDR_CURR_PROC_TOTAL_SIZE 	(EDG_MEM_ADDR_CURR_PROC_QTY * EDG_MEM_ADDR_CURR_PROC_OFFSET)

#define EDG_MEM_ADDR_BASE_PROGRAM			(EDG_MEM_ADDR_BASE)
#define EDG_MEM_ADDR_BASE_SCHEDULE			(EDG_MEM_ADDR_BASE_PROGRAM + EDG_MEM_ADDR_PROGRAM_TOTAL_SIZE + EDG_MEM_ADDR_OFFSET)
#define EDG_MEM_ADDR_BASE_OFFSET			(EDG_MEM_ADDR_BASE_SCHEDULE + EDG_MEM_ADDR_SCHEDULE_TOTAL_SIZE + EDG_MEM_ADDR_OFFSET)
#define EDG_MEM_ADDR_BASE_CURR_PROC			(EDG_MEM_ADDR_BASE_OFFSET + EDG_MEM_ADDR_OFFSET_TOTAL_SIZE + EDG_MEM_ADDR_OFFSET)

#ifdef __cplusplus
}
#endif

#endif /* EDG_MEM_ADDR_H_ */
