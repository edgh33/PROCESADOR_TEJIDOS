/*
 * EDG_BUZZER.cpp
 *
 *  Created on: 26/01/2023
 *      Author: Elkin Granados
 */

/*** Include header file ***/
#include "EDG_BUZZER.h"

EDG_BUZZER_HandleTypeDef hedgBuzzer;

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_BUZZER_Init(EDG_BUZZER_HandleTypeDef * ptrhedgBuzzer)
{
	EDG_BUZZER_PIN_LOW();
	ptrhedgBuzzer->FlagsStatus.AllFlags = 0;
	ptrhedgBuzzer->FlagsStatus.FlagSoundOn = 1;
	ptrhedgBuzzer->period10msLow = 0;
	ptrhedgBuzzer->period10msHigh = 0;
	ptrhedgBuzzer->cicles = 0;
	ptrhedgBuzzer->repeats = 0;
	ptrhedgBuzzer->delay = 0;

	return;

}

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_BUZZER_Sound(uint16_t periodHigh10ms,
					  uint16_t periodLow10ms,
					  uint16_t delay10ms,
					  uint16_t cicles,
					  uint16_t repeats)
{
     hedgBuzzer.FlagsStatus.FlagTrigger = true;
     if(repeats > 0)
     {
        hedgBuzzer.FlagsStatus.FlagisFinite = true;
     }
     else
     {
        hedgBuzzer.FlagsStatus.FlagisFinite = false;
     }

     hedgBuzzer.repeats = repeats;
     hedgBuzzer.period10msHigh = periodHigh10ms;
     hedgBuzzer.period10msLow = periodLow10ms;
     hedgBuzzer.cicles = cicles;

     if(delay10ms > 0)
     {
        hedgBuzzer.delay = delay10ms;
        hedgBuzzer.FlagsStatus.FlagHasDelay = true;
     }

     HAL_TIM_Base_Stop_IT(&EDG_BUZZER_TIM_BASE_HANDLER);
	 __HAL_TIM_SET_COUNTER(&EDG_BUZZER_TIM_BASE_HANDLER, 0);
     HAL_TIM_Base_Start_IT(&EDG_BUZZER_TIM_BASE_HANDLER);
     EDG_BUZZER_PIN_HIGH();
     return;
 }

/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_BUZZER_Constant(void)
{
	EDG_BUZZER_PIN_HIGH();
	return;
}


/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_BUZZER_Stop(void)
{
	EDG_BUZZER_PIN_LOW();
	hedgBuzzer.FlagsStatus.FlagTrigger = false;
	hedgBuzzer.FlagsStatus.FlagActive = false;
	HAL_TIM_Base_Stop_IT(&EDG_BUZZER_TIM_BASE_HANDLER);
	__HAL_TIM_SET_COUNTER(&EDG_BUZZER_TIM_BASE_HANDLER, 0);
	return;
}


/**
  * @brief
  * @note
  * @param
  * @retval
  */
void EDG_BUZZER_IrqFunction(void)
{
    static uint16_t counterHigh = 0;
    static uint16_t counterLow = 0;
    static uint16_t counterRepeats = 0;
    static uint16_t counterDelays = 0;
    static uint16_t counterCicles = 0;
    static bool inDelay = false;

    if(hedgBuzzer.FlagsStatus.FlagTrigger)
    {
        hedgBuzzer.FlagsStatus.FlagTrigger = false;
        hedgBuzzer.FlagsStatus.FlagActive = true;
        hedgBuzzer.FlagsStatus.FlagIsHigh = true;
        counterHigh = hedgBuzzer.period10msHigh;
        counterLow = hedgBuzzer.period10msLow;
        counterCicles = hedgBuzzer.cicles;
        counterRepeats = hedgBuzzer.repeats;
        counterDelays = hedgBuzzer.delay;
        inDelay = false;
    }

    if(hedgBuzzer.FlagsStatus.FlagActive)
    {
       if(counterCicles != 0)
       {
           if(hedgBuzzer.FlagsStatus.FlagIsHigh)
           {
               if(--counterHigh == 0)
               {
                   EDG_BUZZER_PIN_LOW();
                   hedgBuzzer.FlagsStatus.FlagIsHigh = false;
                   counterHigh = hedgBuzzer.period10msHigh;
               }
           }
           else
           {
               if(--counterLow == 0)
               {
                   counterLow = hedgBuzzer.period10msLow;
                   hedgBuzzer.FlagsStatus.FlagIsHigh = true;
                   if(--counterCicles != 0)
                   {
                       EDG_BUZZER_PIN_HIGH();
                   }
                   else
                   {
                       if(hedgBuzzer.FlagsStatus.FlagHasDelay)
                       {
                           inDelay = true;
                       }
                       else
                       {
                           inDelay = false;
                       }
                   }
               }
           }
       }
       else
       {
           if(hedgBuzzer.FlagsStatus.FlagHasDelay && inDelay)
           {
               if(--counterDelays == 0)
               {
                   counterDelays = hedgBuzzer.delay;
                   counterCicles = hedgBuzzer.cicles;
                   inDelay = false;
               }
           }
           if(!inDelay)
           {
               if(hedgBuzzer.FlagsStatus.FlagisFinite)
               {
                   if(--counterRepeats == 0)
                   {
                       hedgBuzzer.FlagsStatus.FlagTrigger = false;
                       hedgBuzzer.FlagsStatus.FlagActive = false;
                       EDG_BUZZER_Stop();
                   }
                   else
                   {
                       counterCicles = hedgBuzzer.cicles;
                       EDG_BUZZER_PIN_HIGH();
                   }
               }
               else
               {
                   counterCicles = hedgBuzzer.cicles;
                   EDG_BUZZER_PIN_HIGH();
               }
           }
       }
    }
    return;
}

