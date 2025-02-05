/*
 * USR_TIM.c
 *
 *  Created on: 1/09/2018
 *      Author: edgh3
 */

#include "USR_TIM.h"

usr_tim_bandera_control_temperatura usr_tim_estado_control_temperatura = USR_TIM_BANDERA_CONTROL_TEMPERATURA_NO_CAMBIO;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	USR_NEXTION_Detener_Recibir_Trama();

	if(htim->Instance == TIM6)
	{
		if((usr_nextion_pantalla_actual != USR_NEXTION_PANTALLA_INICIO) && (usr_nextion_pantalla_actual != USR_NEXTION_PANTALLA_RELOJ) && (usr_nextion_pantalla_actual != USR_NEXTION_PANTALLA_ALERTA))
		{
			USR_NEXTION_Visualizar_RTC();
			USR_NEXTION_Verificar_RTC();
		}

		if(usr_procesador_estado_actual == USR_PROCESADOR_ESTADO_EJECUTANDO)
		{
			usr_procesador_bandera_estado_minuto = USR_PROCESADOR_BANDERA__MINUTO_COMPLETO;
		}
	}

	if(htim->Instance == TIM17)
	{
		usr_tim_estado_control_temperatura = USR_TIM_BANDERA_CONTROL_TEMPERATURA_CAMBIO;

	}

	USR_NEXTION_Iniciar_Recibir_Trama();

}


void USR_TIM_Iniciar_Conteo_1minuto(void)
{
	__HAL_TIM_CLEAR_FLAG(&htim6, TIM_FLAG_UPDATE);
	HAL_TIM_Base_Start_IT(&htim6);
}

/********************************* REMPLAZAR ESTA FUNCION de stm32f0xx_hal_tim.c ****************************/

/**
  * @brief  Configure the Polarity and Filter for TI2.
  * @param  TIMx  to select the TIM peripheral.
  * @param  TIM_ICPolarity The Input Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM_ICPOLARITY_RISING
  *            @arg TIM_ICPOLARITY_FALLING
  *            @arg TIM_ICPOLARITY_BOTHEDGE
  * @param  TIM_ICFilter Specifies the Input Capture Filter.
  *          This parameter must be a value between 0x00 and 0x0F.
  * @retval None
  */

/*
static void TIM_TI2_ConfigInputStage(TIM_TypeDef *TIMx, uint32_t TIM_ICPolarity, uint32_t TIM_ICFilter)
{
  uint32_t tmpccmr1 = 0U;
  uint32_t tmpccer = 0U;

  //Disable the Channel 2: Reset the CC2E Bit
  TIMx->CCER &= ~TIM_CCER_CC2E;
  tmpccmr1 = TIMx->CCMR1;
  tmpccer = TIMx->CCER;

  // Set the filter
  //-----------------------------------ORIGINAL!!!!!--------------------------------

  //tmpccmr1 &= ~TIM_CCMR1_IC2F;
  //tmpccmr1 |= (TIM_ICFilter << 12U);
  //--------------------------------------------------------------------------------

  //-----------------------  MODIFICACION --------------------------------------------
  //No solo se debe borrar la posicion del filtro si no tambien la del mapeo del pin
  //que son los 2 bits CC2S
  tmpccmr1 &= ~(TIM_CCMR1_IC2F | TIM_CCMR1_CC2S);
  //Se debe escribir no solo el filtro si no tambien el valor 01 en los bits CC2S
  //para que el pin quede mapeado como entrada
  tmpccmr1 |= (TIM_ICFilter << 12U) | TIM_CCMR1_CC2S_0;
  //----------------------------------------------------------------------------------

  //Select the Polarity and set the CC2E Bit
  tmpccer &= ~(TIM_CCER_CC2P | TIM_CCER_CC2NP);

  //-------------ORIGINAL!!!!!------------------
  //tmpccer |= (TIM_ICPolarity << 4U);
  //--------------------------------------------

  //-------------------- MI MODIFICACION ------------------------------------
  //Se corre 5 posiciones porque se debe configurar el bit 5 correspondiente
  //a CC2P que selecciona el flanco de subida
  tmpccer |= (TIM_ICPolarity << 5U);
  //-------------------------------------------------------------------------

  //Write to TIMx CCMR1 and CCER registers
  TIMx->CCMR1 = tmpccmr1 ;
  TIMx->CCER = tmpccer;

}
*/

