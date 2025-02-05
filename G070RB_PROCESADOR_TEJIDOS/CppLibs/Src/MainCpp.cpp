/*
 * MainCpp.cpp
 *
 *  Created on: Jan 16, 2023
 *      Author: elkin
 */


#include "MainCpp.h"

/**
  * @brief
  * @note
  * @param  void
  * @retval void
  */
void InitCpp(void)
{
    /*** Just start the state machine handler ***/
	EDG_STATE_MACHINE_Init(&hedgStateMachine);

	return;

}

/**
  * @brief
  * @note
  * @param  void
  * @retval void
  */
void MainCpp(void)
{

	/*** Just run the state machine admin ***/
	EDG_STATE_MACHINE_Admin(&hedgStateMachine);

	return;

}

