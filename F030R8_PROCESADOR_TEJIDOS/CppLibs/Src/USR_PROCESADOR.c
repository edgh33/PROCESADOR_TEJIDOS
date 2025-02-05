/*
 * USR_PROCESADOR.c
 *
 *  Created on: 1/09/2018
 *      Author: edgh3
 */

#include "USR_PROCESADOR.h"

uint32_t usr_procesador_recipiente_actual = 0;
uint32_t usr_procesador_recipiente_cantidad = 0;

uint32_t usr_procesador_recipiente_hora_actual = 0;
uint32_t usr_procesador_recipiente_minuto_actual = 0;
uint32_t usr_procesador_tiempo_total_recipiente_actual = 0;

uint32_t usr_procesador_delay_hora_actual = 0;
uint32_t usr_procesador_delay_minuto_actual = 0;
uint32_t usr_procesador_tiempo_delay_total_minutos = 0;


usr_procesador_estado usr_procesador_estado_actual = USR_PROCESADOR_ESTADO_DETENIDO;
usr_procesador_primera_ejecucion usr_procesador_bandera_ejecucion = USR_PROCESADOR_BANDERA_PRIMERA_EJECUCION;
usr_procesador_bandera_minuto usr_procesador_bandera_estado_minuto = USR_PROCESADOR_BANDERA_MINUTO_CORRIENDO;

uint8_t usr_procesador_comando_numero[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void USR_PROCESADOR_Ejecutar_Programa(void)
{

	uint8_t contador_buzzer;

	if(usr_procesador_bandera_ejecucion == USR_PROCESADOR_BANDERA_PRIMERA_EJECUCION)
	{
		USR_TIM_Detener_Conteo_1minuto();
		usr_procesador_recipiente_actual = 0;
		usr_procesador_recipiente_hora_actual = USR_FLASH_Obtener_Dato_Trama((uint32_t)USR_PROCESADOR_POSICION_R1_HORA);
		usr_procesador_recipiente_minuto_actual = USR_FLASH_Obtener_Dato_Trama((uint32_t)USR_PROCESADOR_POSICION_R1_MINUTO);
		usr_procesador_recipiente_cantidad = USR_FLASH_Obtener_Dato_Trama((uint32_t)USR_PROCESADOR_POSICION_RECIPIENTES);
		//Se reduce en una unidad la cantidad para evitar errores
		//usr_procesador_recipiente_cantidad--;
		usr_procesador_delay_hora_actual = USR_NEXTION_Obtener_Dato_Trama((uint32_t)USR_NEXTION_POSICION_DELAY_HORAS);
		usr_procesador_delay_minuto_actual = USR_NEXTION_Obtener_Dato_Trama((uint32_t)USR_NEXTION_POSICION_DELAY_MINUTOS);


		usr_procesador_tiempo_total_recipiente_actual = (usr_procesador_recipiente_hora_actual*60) + usr_procesador_recipiente_minuto_actual;
		usr_procesador_tiempo_delay_total_minutos = (usr_procesador_delay_hora_actual*60)+usr_procesador_delay_minuto_actual;

		usr_procesador_bandera_ejecucion = USR_PROCESADOR_BANDERA_NO_PRIMERA_EJECUCION;
		usr_procesador_bandera_estado_minuto = USR_PROCESADOR_BANDERA_MINUTO_CORRIENDO;
		usr_procesador_estado_actual = USR_PROCESADOR_ESTADO_EJECUTANDO;

		if(usr_procesador_tiempo_delay_total_minutos == 0)
		{
			USR_NEXTION_Enviar_Trama(usr_nextion_texto_programa_ejecutando, (uint16_t)strlen((const char *)usr_nextion_texto_programa_ejecutando));
			//Se activa el nuevo recipiente en la pantalla
			USR_NEXTION_Activar_Recipiente(usr_procesador_recipiente_actual+1);
			USR_PROCESADOR_ReleAC(USR_PROCESADOR_RELEAC_ACTIVO);
		}
		else
		{
			USR_NEXTION_Enviar_Trama(usr_nextion_texto_programa_retardo, (uint16_t)strlen((const char *)usr_nextion_texto_programa_retardo));
			USR_PROCESADOR_ReleAC(USR_PROCESADOR_RELEAC_INACTIVO);
		}

		/************ ACA SE CREA LA TRAMA DE ESTADO ACTUAL*************/
		USR_FLASH_Crear_Trama_Estado_Actual();
		USR_FLASH_Guardar_Estado_Actual();

		USR_Reiniciar_Conteo_1minuto();
		USR_TIM_Iniciar_Conteo_1minuto();

	}
	else
	{
		//Si se entra a ejecutar y estaba en estado de pausa este se cambia para que se reanude el conteo
		if(usr_procesador_estado_actual == USR_PROCESADOR_ESTADO_PAUSADO)
		{
			if(usr_procesador_tiempo_delay_total_minutos == 0)
			{
				USR_NEXTION_Enviar_Trama(usr_nextion_texto_programa_ejecutando, (uint16_t)strlen((const char *)usr_nextion_texto_programa_ejecutando));
			}
			else
			{
				USR_NEXTION_Enviar_Trama(usr_nextion_texto_programa_retardo, (uint16_t)strlen((const char *)usr_nextion_texto_programa_retardo));
			}

			usr_procesador_estado_actual = USR_PROCESADOR_ESTADO_EJECUTANDO;
		}
		//Se verifica si se cumplio el minuto y se ejecutan las tareas correspondientes
		if(usr_procesador_bandera_estado_minuto == USR_PROCESADOR_BANDERA__MINUTO_COMPLETO)
		{

			//Si ya paso el Delay se ejecuta el programa en la posicion actual
			if(usr_procesador_tiempo_delay_total_minutos == 0)
			{

				if(usr_procesador_recipiente_minuto_actual == 0)
				{
					usr_procesador_recipiente_minuto_actual = 59;
					usr_procesador_recipiente_hora_actual--;
				}
				else
				{
					usr_procesador_recipiente_minuto_actual--;
				}
				//Se resta al tiempo total
				usr_procesador_tiempo_total_recipiente_actual--;

				//Se cambia la hora del recipiente actual
				sprintf((char *)usr_procesador_comando_numero,"n%d.val=%d",(int)(usr_procesador_recipiente_actual*2),(int)usr_procesador_recipiente_hora_actual);
				USR_NEXTION_Enviar_Trama(usr_procesador_comando_numero, (uint16_t)strlen((const char *)usr_procesador_comando_numero));

				//Se cambia el minuto del recipiente actual
				sprintf((char *)usr_procesador_comando_numero,"n%d.val=%d",(int)((usr_procesador_recipiente_actual*2)+1),(int)usr_procesador_recipiente_minuto_actual);
				USR_NEXTION_Enviar_Trama(usr_procesador_comando_numero, (uint16_t)strlen((const char *)usr_procesador_comando_numero));

				USR_FLASH_Cambiar_Dato_Trama_Estado_Actual((USR_PROCESADOR_POSICION_R1_HORA+(usr_procesador_recipiente_actual*2)), usr_procesador_recipiente_hora_actual);
				USR_FLASH_Cambiar_Dato_Trama_Estado_Actual((USR_PROCESADOR_POSICION_R1_MINUTO+(usr_procesador_recipiente_actual*2)), usr_procesador_recipiente_minuto_actual);
				USR_FLASH_Guardar_Estado_Actual();

				if(usr_procesador_tiempo_total_recipiente_actual == 0)
				{
					//Se aumenta el valor del recipiente actual
					usr_procesador_recipiente_actual++;

					USR_FLASH_Cambiar_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_POSICION_ACTUAL, usr_procesador_recipiente_actual);
					USR_FLASH_Guardar_Estado_Actual();

					//Se verifica que no se haya finalizado el proceso
					if(usr_procesador_recipiente_actual < usr_procesador_recipiente_cantidad)
					{

						USR_FLASH_Cambiar_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_ESTADO_ACTUAL, USR_PROCESADOR_ESTADO_ROTANDO);
						USR_FLASH_Guardar_Estado_Actual();

						/***********************FUNCION DE CAMBIO DE POSICION!!!!!!!!!!!!!!!***********/
#if USR_PROCESADOR_DESHABILITAR_MOTOR == 0
						USR_PROCESADOR_Cambiar_Posicion();

						USR_FLASH_Cambiar_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_ESTADO_ACTUAL, USR_PROCESADOR_ESTADO_EJECUTANDO);
						USR_FLASH_Guardar_Estado_Actual();
#endif
						//Se cargan los nuevos datos
						usr_procesador_recipiente_hora_actual = USR_FLASH_Obtener_Dato_Trama((uint32_t)(USR_PROCESADOR_POSICION_R1_HORA+(usr_procesador_recipiente_actual*2)));
						usr_procesador_recipiente_minuto_actual = USR_FLASH_Obtener_Dato_Trama((uint32_t)(USR_PROCESADOR_POSICION_R1_MINUTO+(usr_procesador_recipiente_actual*2)));
						usr_procesador_tiempo_total_recipiente_actual = (usr_procesador_recipiente_hora_actual*60) + usr_procesador_recipiente_minuto_actual;

						//Se activa el nuevo recipiente en la pantalla
						USR_NEXTION_Activar_Recipiente(usr_procesador_recipiente_actual+1);

						//Se cambia la hora del recipiente actual
						sprintf((char *)usr_procesador_comando_numero,"n%d.val=%d",(int)(usr_procesador_recipiente_actual*2),(int)usr_procesador_recipiente_hora_actual);
						USR_NEXTION_Enviar_Trama(usr_procesador_comando_numero, (uint16_t)strlen((const char *)usr_procesador_comando_numero));

						//Se cambia el minuto del recipiente actual
						sprintf((char *)usr_procesador_comando_numero,"n%d.val=%d",(int)((usr_procesador_recipiente_actual*2)+1),(int)usr_procesador_recipiente_minuto_actual);
						USR_NEXTION_Enviar_Trama(usr_procesador_comando_numero, (uint16_t)strlen((const char *)usr_procesador_comando_numero));
						//SE REINICIA LA BANDERA
						usr_procesador_bandera_estado_minuto = USR_PROCESADOR_BANDERA_MINUTO_CORRIENDO;
					}
					else
					{
						usr_procesador_estado_actual = USR_PROCESADOR_ESTADO_DETENIDO;
						USR_NEXTION_Enviar_Trama(usr_nextion_texto_programa_finalizado,  (uint16_t)strlen((const char *)usr_nextion_texto_programa_finalizado));
						USR_NEXTION_Enviar_Trama(usr_nextion_com_visoff_bt14,  (uint16_t)strlen((const char *)usr_nextion_com_visoff_bt14));

						USR_NEXTION_Enviar_Trama(usr_nextion_com_todo_inhabilitado,  (uint16_t)strlen((const char *)usr_nextion_com_todo_inhabilitado));

						for(contador_buzzer = 0; contador_buzzer < 3; contador_buzzer++)
						{
							HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
							HAL_Delay(500);
							HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
							HAL_Delay(500);
						}

						USR_NEXTION_Enviar_Trama(usr_nextion_com_todo_habilitado,  (uint16_t)strlen((const char *)usr_nextion_com_todo_habilitado));

					}
				}
				else
				{
					//SE REINICIA LA BANDERA
					usr_procesador_bandera_estado_minuto = USR_PROCESADOR_BANDERA_MINUTO_CORRIENDO;
				}
			}
			//Si no a finalizado el delay se sigue contando el tiempo
			else
			{

				if(usr_procesador_delay_minuto_actual == 0)
				{
					usr_procesador_delay_minuto_actual = 59;
					usr_procesador_delay_hora_actual--;
				}
				else
				{
					usr_procesador_delay_minuto_actual--;
				}
				//Se resta al tiempo total
				usr_procesador_tiempo_delay_total_minutos--;

				//Se cambia la hora del recipiente actual
				sprintf((char *)usr_procesador_comando_numero,"n27.val=%d",(int)usr_procesador_delay_hora_actual);
				USR_NEXTION_Enviar_Trama(usr_procesador_comando_numero, (uint16_t)strlen((const char *)usr_procesador_comando_numero));

				//Se cambia el minuto del recipiente actual
				sprintf((char *)usr_procesador_comando_numero,"n28.val=%d",(int)usr_procesador_delay_minuto_actual);
				USR_NEXTION_Enviar_Trama(usr_procesador_comando_numero, (uint16_t)strlen((const char *)usr_procesador_comando_numero));

				USR_FLASH_Cambiar_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_RETARDO_HORAS, usr_procesador_delay_hora_actual);
				USR_FLASH_Cambiar_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_RETARDO_MINUTOS, usr_procesador_delay_minuto_actual);
				USR_FLASH_Guardar_Estado_Actual();

				if(usr_procesador_tiempo_delay_total_minutos == 0)
				{
					//Se activa el nuevo recipiente en la pantalla
					USR_NEXTION_Activar_Recipiente(usr_procesador_recipiente_actual+1);
					USR_NEXTION_Enviar_Trama(usr_nextion_texto_programa_ejecutando, (uint16_t)strlen((const char *)usr_nextion_texto_programa_ejecutando));
					USR_PROCESADOR_ReleAC(USR_PROCESADOR_RELEAC_ACTIVO);
				}

				//SE REINICIA LA BANDERA
				usr_procesador_bandera_estado_minuto = USR_PROCESADOR_BANDERA_MINUTO_CORRIENDO;
			}
		}
	}
}


void USR_PROCESADOR_Pausar_Programa(void)
{

	usr_procesador_estado_actual = USR_PROCESADOR_ESTADO_PAUSADO;
	usr_procesador_bandera_estado_minuto = USR_PROCESADOR_BANDERA_MINUTO_CORRIENDO;
	USR_NEXTION_Enviar_Trama(usr_nextion_texto_programa_pausado, (uint16_t)strlen((const char *)usr_nextion_texto_programa_pausado));

}

void USR_PROCESADOR_Detener_Programa(void)
{

	usr_procesador_bandera_ejecucion = USR_PROCESADOR_BANDERA_PRIMERA_EJECUCION;
	usr_procesador_estado_actual = USR_PROCESADOR_ESTADO_DETENIDO;

	USR_NEXTION_Mostrar_Programa((uint8_t)USR_FLASH_Obtener_Dato_Trama((uint32_t)USR_PROCESADOR_POSICION_PROGRAMA));

	//Se limpian los valores de delay
	sprintf((char *)usr_procesador_comando_numero,"n27.val=%d",(int)usr_procesador_delay_hora_actual);
	USR_NEXTION_Enviar_Trama(usr_procesador_comando_numero, (uint16_t)strlen((const char *)usr_procesador_comando_numero));
	sprintf((char *)usr_procesador_comando_numero,"retHora.val=0");
	USR_NEXTION_Enviar_Trama(usr_procesador_comando_numero, (uint16_t)strlen((const char *)usr_procesador_comando_numero));

	//Se cambia el minuto del recipiente actual
	sprintf((char *)usr_procesador_comando_numero,"n28.val=%d",(int)usr_procesador_delay_minuto_actual);
	USR_NEXTION_Enviar_Trama(usr_procesador_comando_numero, (uint16_t)strlen((const char *)usr_procesador_comando_numero));
	sprintf((char *)usr_procesador_comando_numero,"retMin.val=0");
	USR_NEXTION_Enviar_Trama(usr_procesador_comando_numero, (uint16_t)strlen((const char *)usr_procesador_comando_numero));

	USR_FLASH_Cambiar_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_ESTADO_ACTUAL, USR_PROCESADOR_ESTADO_DETENIDO);
	USR_FLASH_Guardar_Estado_Actual();

	USR_PROCESADOR_ReleAC(USR_PROCESADOR_RELEAC_INACTIVO);

}

void USR_PROCESADOR_Cambiar_Posicion(void)
{

#if USR_PROCESADOR_VERSION_FW == 0

	//Se hace un timeout de 1 minuto para que no se bloquee el equipo completamente
	uint32_t tickstart = HAL_GetTick();
	uint32_t wait = USR_PROCESADOR_TIMEOUT_ROTACION;

	/* Add a period to guarantee minimum wait */
	if (wait < HAL_MAX_DELAY)
	{
	 wait++;
	}

	//Se detiene momentaneamente el conteo del minuto para el proceso
	usr_procesador_estado_actual = USR_PROCESADOR_ESTADO_PAUSADO;

	USR_NEXTION_Inhabilitar_Touch_Pantalla();
	USR_NEXTION_Enviar_Trama(usr_nextion_texto_programa_rotando, (uint16_t)strlen((const char *)usr_nextion_texto_programa_rotando));

	//Se mantiene el rele en corto...
	USR_PROCESADOR_RELE_START_INACTIVO();
	HAL_Delay(2000);

	//Mientras el sensor este en 0 y no se cumpla el timeout se mantiene rotando...
	//while((USR_PROCESADOR_ESTADO_SENSOR_AC() == GPIO_PIN_RESET) && ((HAL_GetTick() - tickstart) < wait))
	while((USR_PROCESADOR_ESTADO_SENSOR_AC() == GPIO_PIN_SET) && ((HAL_GetTick() - tickstart) < wait))
	{

		if(usr_tim_estado_control_temperatura == USR_TIM_BANDERA_CONTROL_TEMPERATURA_CAMBIO)
		{
			//Si el equipo esta rotando, se hace el control de temperatura en la espera, si no se hace en el ciclo del main...
			USR_TEMPERATURA_Control();
			if(usr_temperatura_estado_alerta == USR_TEMPERATURA_ALERTA_ACTIVA)
			{
				USR_NEXTION_Cambiar_Pantalla(USR_NEXTION_PANTALLA_ALERTA);
			}
		}

	}

	USR_PROCESADOR_RELE_START_ACTIVO();

	USR_NEXTION_Enviar_Trama(usr_nextion_texto_programa_ejecutando, (uint16_t)strlen((const char *)usr_nextion_texto_programa_ejecutando));
	USR_NEXTION_Habilitar_Touch_Pantalla();


	//Se reinicia el conteo del minuto
	USR_TIM_Detener_Conteo_1minuto();
	USR_Reiniciar_Conteo_1minuto();
	usr_procesador_estado_actual = USR_PROCESADOR_ESTADO_EJECUTANDO;
	usr_procesador_bandera_estado_minuto = USR_PROCESADOR_BANDERA_MINUTO_CORRIENDO;
	USR_TIM_Iniciar_Conteo_1minuto();

#endif

#if USR_PROCESADOR_VERSION_FW == 1

	//Se hace un timeout de 1 minuto para que no se bloquee el equipo completamente
	uint32_t tickstart = HAL_GetTick();
	uint32_t wait = USR_PROCESADOR_TIMEOUT_ROTACION_E1;

	/* Add a period to guarantee minimum wait */
	if (wait < HAL_MAX_DELAY)
	{
	 wait++;
	}

	//Se detiene momentaneamente el conteo del minuto para el proceso
	usr_procesador_estado_actual = USR_PROCESADOR_ESTADO_PAUSADO;

	USR_NEXTION_Inhabilitar_Touch_Pantalla();
	USR_NEXTION_Enviar_Trama(usr_nextion_texto_programa_rotando, (uint16_t)strlen((const char *)usr_nextion_texto_programa_rotando));

	//Se pone el rele start en corto....
	USR_PROCESADOR_RELE_START_INACTIVO();
	//Original en 200
	HAL_Delay(200);

	//Mientras el sensor este en 1 y no se cumpla el timeout se mantiene rotando...
//	while((USR_PROCESADOR_ESTADO_SENSOR_AC() == GPIO_PIN_SET) && ((HAL_GetTick() - tickstart) < wait))
	while((USR_PROCESADOR_ESTADO_SENSOR_AC() == GPIO_PIN_RESET) && ((HAL_GetTick() - tickstart) < wait))
	{

		if(usr_tim_estado_control_temperatura == USR_TIM_BANDERA_CONTROL_TEMPERATURA_CAMBIO)
		{
			//Si el equipo esta rotando, se hace el control de temperatura en la espera, si no se mantiene en la espera...
			USR_TEMPERATURA_Control();
			if(usr_temperatura_estado_alerta == USR_TEMPERATURA_ALERTA_ACTIVA)
			{
				USR_NEXTION_Cambiar_Pantalla(USR_NEXTION_PANTALLA_ALERTA);
			}
		}

	}

	//Se hace una espera de 7 segundos para garantizar que se mantenga el sensor en 0 y se libere el sensor de atras
	HAL_Delay(7000);
	//Se pone el rele start en abierto....
	USR_PROCESADOR_RELE_START_ACTIVO();
	HAL_Delay(1000);

	//Se configura el time out
	tickstart = HAL_GetTick();
	wait = USR_PROCESADOR_TIMEOUT_ROTACION_E2;

	/* Add a period to guarantee minimum wait */
	if (wait < HAL_MAX_DELAY)
	{
	 wait++;
	}

	//Mientras el sensor este en 0 y no se cumpla el timeout se mantiene rotando...
//	while((USR_PROCESADOR_ESTADO_SENSOR_AC() == GPIO_PIN_RESET) && ((HAL_GetTick() - tickstart) < wait))
    while((USR_PROCESADOR_ESTADO_SENSOR_AC() == GPIO_PIN_SET) && ((HAL_GetTick() - tickstart) < wait))
	{

		if(usr_tim_estado_control_temperatura == USR_TIM_BANDERA_CONTROL_TEMPERATURA_CAMBIO)
		{
			//Si el equipo esta rotando, se hace el control de temperatura en la espera, si no se hace en el ciclo del main...
			USR_TEMPERATURA_Control();
			if(usr_temperatura_estado_alerta == USR_TEMPERATURA_ALERTA_ACTIVA)
			{
				USR_NEXTION_Cambiar_Pantalla(USR_NEXTION_PANTALLA_ALERTA);
			}
		}

	}

	//Original en 1000
	HAL_Delay(1000);

	//Se configura el time out
	tickstart = HAL_GetTick();
	wait = USR_PROCESADOR_TIMEOUT_ROTACION_E3;

	/* Add a period to guarantee minimum wait */
	if (wait < HAL_MAX_DELAY)
	{
	 wait++;
	}

	//Mientras el sensor este en 1 y no se cumpla el timeout se mantiene rotando...
//	while((USR_PROCESADOR_ESTADO_SENSOR_AC() == GPIO_PIN_SET) && ((HAL_GetTick() - tickstart) < wait))
	while((USR_PROCESADOR_ESTADO_SENSOR_AC() == GPIO_PIN_RESET) && ((HAL_GetTick() - tickstart) < wait))
	{

		if(usr_tim_estado_control_temperatura == USR_TIM_BANDERA_CONTROL_TEMPERATURA_CAMBIO)
		{
			//Si el equipo esta rotando, se hace el control de temperatura en la espera, si no se mantiene en la espera...
			USR_TEMPERATURA_Control();
			if(usr_temperatura_estado_alerta == USR_TEMPERATURA_ALERTA_ACTIVA)
			{
				USR_NEXTION_Cambiar_Pantalla(USR_NEXTION_PANTALLA_ALERTA);
			}
		}

	}

	USR_NEXTION_Enviar_Trama(usr_nextion_texto_programa_ejecutando, (uint16_t)strlen((const char *)usr_nextion_texto_programa_ejecutando));
	USR_NEXTION_Habilitar_Touch_Pantalla();


	//Se reinicia el conteo del minuto
	USR_TIM_Detener_Conteo_1minuto();
	USR_Reiniciar_Conteo_1minuto();
	usr_procesador_estado_actual = USR_PROCESADOR_ESTADO_EJECUTANDO;
	usr_procesador_bandera_estado_minuto = USR_PROCESADOR_BANDERA_MINUTO_CORRIENDO;
	USR_TIM_Iniciar_Conteo_1minuto();

#endif

}

void USR_PROCESADOR_Elevar_Canasta(void)
{

#if USR_PROCESADOR_VERSION_FW == 0

#if USR_PROCESADOR_DESHABILITAR_MOTOR == 0

	uint32_t tickstart = HAL_GetTick();
	uint32_t wait = USR_PROCESADOR_TIMEOUT_CAMBIOS;

	/* Add a period to guarantee minimum wait */
	if (wait < HAL_MAX_DELAY)
	{
	 wait++;
	}

	USR_PROCESADOR_RELE_MANUAL_INACTIVO();
	HAL_Delay(3000);
	//Mientras el sensor este en 0 se mantiene rotando
	//while((USR_PROCESADOR_ESTADO_SENSOR_AC() == GPIO_PIN_RESET)  && ((HAL_GetTick() - tickstart) < wait)){}
	while((USR_PROCESADOR_ESTADO_SENSOR_AC() == GPIO_PIN_SET)  && ((HAL_GetTick() - tickstart) < wait)){}

#endif

	USR_NEXTION_Enviar_Trama(usr_nextion_texto_canasta_arriba, (uint16_t)strlen((const char *)usr_nextion_texto_canasta_arriba));
	USR_NEXTION_Enviar_Trama(usr_nextion_com_vison_bt0, (uint16_t)strlen((const char *)usr_nextion_com_vison_bt0));

#endif

#if USR_PROCESADOR_VERSION_FW == 1

#if USR_PROCESADOR_DESHABILITAR_MOTOR == 0

	uint32_t tickstart = HAL_GetTick();
	uint32_t wait = USR_PROCESADOR_TIMEOUT_ROTACION_E1;

	/* Add a period to guarantee minimum wait */
	if (wait < HAL_MAX_DELAY)
	{
	 wait++;
	}

	//Se pone el rele start en corto....
	USR_PROCESADOR_RELE_START_INACTIVO();
	HAL_Delay(200);

	//Mientras el sensor este en 1 se mantiene subiendo
	//while((USR_PROCESADOR_ESTADO_SENSOR_AC() == GPIO_PIN_SET)  && ((HAL_GetTick() - tickstart) < wait)){}
	while((USR_PROCESADOR_ESTADO_SENSOR_AC() == GPIO_PIN_RESET)  && ((HAL_GetTick() - tickstart) < wait)){}

	//Se hace una espera de 7 segundos para garantizar que se mantenga el sensor en 0 y se libere el sensor de atras
	HAL_Delay(7000);
	//Se pone el rele start en abierto....
	USR_PROCESADOR_RELE_START_ACTIVO();
	HAL_Delay(1000);

	//Se configura el time out
	tickstart = HAL_GetTick();
	wait = USR_PROCESADOR_TIMEOUT_ROTACION_E2;

	/* Add a period to guarantee minimum wait */
	if (wait < HAL_MAX_DELAY)
	{
	 wait++;
	}

	//Mientras el sensor este en 0 se mantiene subiendo
	//while((USR_PROCESADOR_ESTADO_SENSOR_AC() == GPIO_PIN_RESET) && ((HAL_GetTick() - tickstart) < wait)){}
	while((USR_PROCESADOR_ESTADO_SENSOR_AC() == GPIO_PIN_SET) && ((HAL_GetTick() - tickstart) < wait)){}

	//Se pone el rele start en corto y esto indica que el carrusel esta arriba....
	USR_PROCESADOR_RELE_START_INACTIVO();

#endif

	USR_NEXTION_Enviar_Trama(usr_nextion_texto_canasta_arriba, (uint16_t)strlen((const char *)usr_nextion_texto_canasta_arriba));
	USR_NEXTION_Enviar_Trama(usr_nextion_com_vison_bt0, (uint16_t)strlen((const char *)usr_nextion_com_vison_bt0));

#endif

}

void USR_PROCESADOR_Bajar_Canasta(void)
{

#if USR_PROCESADOR_VERSION_FW == 0

#if USR_PROCESADOR_DESHABILITAR_MOTOR == 0

	uint32_t tickstart = HAL_GetTick();
	uint32_t wait = USR_PROCESADOR_TIMEOUT_CAMBIOS;

	/* Add a period to guarantee minimum wait */
	if (wait < HAL_MAX_DELAY)
	{
	 wait++;
	}

	USR_PROCESADOR_RELE_MANUAL_ACTIVO();
	HAL_Delay(3000);
	//while((USR_PROCESADOR_ESTADO_SENSOR_AC() == GPIO_PIN_RESET) && ((HAL_GetTick() - tickstart) < wait)){}
	while((USR_PROCESADOR_ESTADO_SENSOR_AC() == GPIO_PIN_SET) && ((HAL_GetTick() - tickstart) < wait)){}

#endif

	USR_NEXTION_Enviar_Trama(usr_nextion_texto_canasta_abajo, (uint16_t)strlen((const char *)usr_nextion_texto_canasta_abajo));
	USR_NEXTION_Enviar_Trama(usr_nextion_com_vison_bt0, (uint16_t)strlen((const char *)usr_nextion_com_vison_bt0));
	USR_NEXTION_Enviar_Trama(usr_nextion_com_vison_b0, (uint16_t)strlen((const char *)usr_nextion_com_vison_b0));

#endif

#if USR_PROCESADOR_VERSION_FW == 1

#if USR_PROCESADOR_DESHABILITAR_MOTOR == 0

	uint32_t tickstart = HAL_GetTick();
	uint32_t wait = USR_PROCESADOR_TIMEOUT_ROTACION_E2;

	/* Add a period to guarantee minimum wait */
	if (wait < HAL_MAX_DELAY)
	{
	 wait++;
	}

	//Se pone el sensor en corto y se espera un tiempo para que el sensor detecte el estado...
	USR_PROCESADOR_RELE_START_ACTIVO();
	HAL_Delay(1000);
//	while((USR_PROCESADOR_ESTADO_SENSOR_AC() == GPIO_PIN_SET) && ((HAL_GetTick() - tickstart) < wait)){}
	while((USR_PROCESADOR_ESTADO_SENSOR_AC() == GPIO_PIN_RESET) && ((HAL_GetTick() - tickstart) < wait)){}

#endif

	USR_NEXTION_Enviar_Trama(usr_nextion_texto_canasta_abajo, (uint16_t)strlen((const char *)usr_nextion_texto_canasta_abajo));
	USR_NEXTION_Enviar_Trama(usr_nextion_com_vison_bt0, (uint16_t)strlen((const char *)usr_nextion_com_vison_bt0));
	USR_NEXTION_Enviar_Trama(usr_nextion_com_vison_b0, (uint16_t)strlen((const char *)usr_nextion_com_vison_b0));

#endif

}

void USR_PROCESADOR_Buzzer(void)
{
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
	HAL_Delay(50);
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);

}

void USR_PROCESADOR_ReleAC(usr_procesador_releac estado)
{

	switch(estado)
	{
		case USR_PROCESADOR_RELEAC_INACTIVO:

			USR_PROCESADOR_RELE_AC_OFF_ON();
			HAL_Delay(200);
			USR_PROCESADOR_RELE_AC_OFF_OFF();

			break;

		case USR_PROCESADOR_RELEAC_ACTIVO:

			USR_PROCESADOR_RELE_AC_ON_ON();
			HAL_Delay(200);
			USR_PROCESADOR_RELE_AC_ON_OFF();

			break;

	}

}

void USR_PROCESADOR_Retomar_Programa_Actual(void)
{

	uint8_t contador = 0;

	//Se cambia la pantalla a la de ejecutar
	usr_nextion_pantalla_actual = USR_NEXTION_PANTALLA_EJECUTAR;
	USR_NEXTION_Enviar_Trama(usr_nextion_com_pantalla_ejecutar, (uint16_t)strlen((const char *)usr_nextion_com_pantalla_ejecutar));
	USR_NEXTION_Visualizar_RTC();

	usr_procesador_recipiente_actual = USR_FLASH_Obtener_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_POSICION_ACTUAL);
	usr_procesador_recipiente_cantidad = USR_FLASH_Obtener_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_RECIPIENTES);
	usr_procesador_recipiente_hora_actual = USR_FLASH_Obtener_Dato_Trama_Estado_Actual((uint32_t)(USR_PROCESADOR_POSICION_R1_HORA+(usr_procesador_recipiente_actual*2)));
	usr_procesador_recipiente_minuto_actual = USR_FLASH_Obtener_Dato_Trama_Estado_Actual((uint32_t)(USR_PROCESADOR_POSICION_R1_MINUTO+(usr_procesador_recipiente_actual*2)));
	usr_procesador_tiempo_total_recipiente_actual = (usr_procesador_recipiente_hora_actual*60) + usr_procesador_recipiente_minuto_actual;

	usr_procesador_delay_hora_actual = USR_FLASH_Obtener_Dato_Trama_Estado_Actual((uint32_t)USR_PROCESADOR_POSICION_RETARDO_HORAS);
	usr_procesador_delay_minuto_actual = USR_FLASH_Obtener_Dato_Trama_Estado_Actual((uint32_t)USR_PROCESADOR_POSICION_RETARDO_MINUTOS);
	usr_procesador_tiempo_delay_total_minutos = (usr_procesador_delay_hora_actual*60)+usr_procesador_delay_minuto_actual;

	usr_procesador_bandera_ejecucion = USR_PROCESADOR_BANDERA_NO_PRIMERA_EJECUCION;
	usr_procesador_bandera_estado_minuto = USR_PROCESADOR_BANDERA_MINUTO_CORRIENDO;
	usr_procesador_estado_actual = USR_PROCESADOR_ESTADO_EJECUTANDO;

	if(usr_procesador_tiempo_delay_total_minutos == 0)
	{

		USR_PROCESADOR_ReleAC(USR_PROCESADOR_RELEAC_ACTIVO);
	}
	else
	{
		USR_PROCESADOR_ReleAC(USR_PROCESADOR_RELEAC_INACTIVO);
	}

	USR_NEXTION_Mostrar_Programa_Retomado();

	//Se ejecuta dos veces el control de temperatura para que queden actualizados los arreglos de las 2 termocuplas
	USR_TEMPERATURA_Control();
	USR_TEMPERATURA_Control();
	//Se reinicia la bandera de activacion del control de temperatura
	usr_tim_estado_control_temperatura = USR_TIM_BANDERA_CONTROL_TEMPERATURA_NO_CAMBIO;

	//Se copia la trama del estado actual al buffer del programa...
	for(contador = 0; contador < USR_FLASH_BUFFER_PROGRAMA_TAM_8B; contador++)
	{
		usr_flash_buffer_programa[contador] = usr_flash_buffer_estado_actual[contador];
	}

	//Se habilita los botones de la pantalla...
	USR_NEXTION_Habilitar_Touch_Pantalla();
}
