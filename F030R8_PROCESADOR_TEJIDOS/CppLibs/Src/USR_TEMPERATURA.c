/*
 * USR_TEMPERATURA.c
 *
 *  Created on: 4/09/2018
 *      Author: edgh3
 */

#include "USR_TEMPERATURA.h"

/*************************************************************************************************
 * Modificacion 30/11/21:
 * 1. 	Se cambia la inicializacion de arreglo_termocuplas de:
 * 		usr_struc_termocupla arreglo_termocuplas[USR_TEMPERATURA_CANTIDAD_TERMOCUPLAS] a
 * 		usr_struc_termocupla arreglo_termocuplas[USR_TEMPERATURA_CANTIDAD_MAX_TERMOCUPLAS]
 * 		siempre se separa el espacio de memoria para las 6 termocuplas que maximo se pueden manejar
 * 		y dentro del codigo se inicializan solo las necesarias, esto pensando tambien en el codigo
 *		para la central de inclusion
 *
 * 2. 	Se crea la estructura para el control PID de las salidas y se deja dentro de la estructura
 * 		de arreglo termocuplas
 *
 * 3. 	Se agrega una variable llamada cantidad_termocuplas, este se inicializa por ahora con
 * 		el valor de la macro USR_TEMPERATURA_CANTIDAD_TERMOCUPLAS, pero se supone que debe
 * 		cargarse con un valor almacenado en memoria
 *************************************************************************************************/
usr_struc_termocupla arreglo_termocuplas[USR_TEMPERATURA_CANTIDAD_MAX_TERMOCUPLAS];

uint8_t cantidad_termocuplas;
usr_temperatura_alerta usr_temperatura_estado_alerta = USR_TEMPERATURA_ALERTA_INACTIVA;



/*01/12/21
 * Funcion modificada para el control PID, faltan detalles menores de memoria*/
void USR_TEMPERATURA_Iniciar_Chips(void)
{
	/*30/11/21
	 * Se cambia el nombre a esta variable para que no se confunda con el contador externo
	 * se usa esta variable solo de manera local en la funcion*/
	uint8_t contador_termocuplas_ini = 0;

	/*30/11/21
	 * Se inicializan el valor de cantidad termocuplas por ahora con una macro pero debe venir de un
	 * registro de memoria*/
	cantidad_termocuplas = USR_TEMPERATURA_CANTIDAD_TERMOCUPLAS;

	//Se leen los valores de las termocuplas para que los chips queden bien configurados
	USR_FLASH_Leer_Termocuplas();

	//Se inician todas las termocuplas...
	for(contador_termocuplas_ini = 0; contador_termocuplas_ini < cantidad_termocuplas; contador_termocuplas_ini++)
	{

		arreglo_termocuplas[contador_termocuplas_ini].temperatura_configurada = 60;
		arreglo_termocuplas[contador_termocuplas_ini].tipo_termocupla = USR_FLASH_Obtener_Dato_Trama(USR_TEMPERATURA_POSICION_TEMOCUPLA_1+contador_termocuplas_ini);
		arreglo_termocuplas[contador_termocuplas_ini].tipo_muestreo= USR_TEMPERATURA_MUESTREO_4SAMPLES;
		arreglo_termocuplas[contador_termocuplas_ini].numero_termocupla = contador_termocuplas_ini;
		arreglo_termocuplas[contador_termocuplas_ini].estado_control = USR_TEMPERATURA_ESTADO_CONTROL_INACTIV0;
		arreglo_termocuplas[contador_termocuplas_ini].estado_pwm = USR_TEMPERATURA_ESTADO_PWM_INACTIV0;
		arreglo_termocuplas[contador_termocuplas_ini].tipo_control = USR_TEMPERATURA_TIPO_CONTROL_ONOFF;
		arreglo_termocuplas[contador_termocuplas_ini].porcentaje_potencia = 90;

		/*30/11/21
		 * Se inicializan los valores del control PID de cada termocupla, por el momento los valores
		 * de las constantes P I y D se inicializan por macrodefiniciones, estos valores se deben guardar
		 * en memoria y se deben configurar desde la interfaz nextion*/

		arreglo_termocuplas[contador_termocuplas_ini].pid.kp = USR_TEMPERATURA_PID_P;
		arreglo_termocuplas[contador_termocuplas_ini].pid.ki = USR_TEMPERATURA_PID_I;
		arreglo_termocuplas[contador_termocuplas_ini].pid.kd = USR_TEMPERATURA_PID_D;

		arreglo_termocuplas[contador_termocuplas_ini].pid.Error = 0.0;
		arreglo_termocuplas[contador_termocuplas_ini].pid.Proporcional = 0.0;
		arreglo_termocuplas[contador_termocuplas_ini].pid.Integral = 0.0;
		arreglo_termocuplas[contador_termocuplas_ini].pid.Derivativo = 0.0;
		arreglo_termocuplas[contador_termocuplas_ini].pid.Error_0 = 0.0;
		arreglo_termocuplas[contador_termocuplas_ini].pid.Control = 0;

		USR_TEMPERATURA_Configurar_MAX31856(&arreglo_termocuplas[contador_termocuplas_ini]);


		if(usr_temperatura_estado_alerta == USR_TEMPERATURA_ALERTA_ACTIVA)
		{
			break;
		}
	}

}

/*01/12/21
 * Funcion sin modificar, no afecta para el control PID*/
void USR_TEMPERATURA_Configurar_MAX31856(usr_struc_termocupla * prvusr_temperatura_termocupla)
{

	uint8_t spi_trama_datos_tx[2] = {0,0};

	//Se detiene la medicion
	spi_trama_datos_tx[0] = USR_TEMPERATURA_DIR_CR0_ESCRITURA;
	spi_trama_datos_tx[1] = USR_TEMPERATURA_MASK_DETENER_MEDICION;

	USR_TEMPERATURA_CS_Posicion(GPIO_PIN_RESET, prvusr_temperatura_termocupla->numero_termocupla);
	HAL_SPI_Transmit(&hspi1, spi_trama_datos_tx, sizeof(spi_trama_datos_tx), 10);
	USR_TEMPERATURA_CS_Posicion(GPIO_PIN_SET, prvusr_temperatura_termocupla->numero_termocupla);

	//Se configura el tipo de termocupla y la velocidad de muestreo
	spi_trama_datos_tx[0] = USR_TEMPERATURA_DIR_CR1_ESCRITURA;
	spi_trama_datos_tx[1] = (prvusr_temperatura_termocupla->tipo_termocupla) | ((prvusr_temperatura_termocupla->tipo_muestreo)<<4);

	USR_TEMPERATURA_CS_Posicion(GPIO_PIN_RESET, prvusr_temperatura_termocupla->numero_termocupla);
	HAL_SPI_Transmit(&hspi1, spi_trama_datos_tx, sizeof(spi_trama_datos_tx), 10);
	USR_TEMPERATURA_CS_Posicion(GPIO_PIN_SET, prvusr_temperatura_termocupla->numero_termocupla);

	//Se hace una lectura del chip para configurar los otros parametros de la estructura...
	USR_TEMPERATURA_Leer_Temperatura(prvusr_temperatura_termocupla);

}


/*01/12/21
 * Funcion sin modificar, no afecta para el control PID*/
void USR_TEMPERATURA_Leer_Temperatura(usr_struc_termocupla * prvusr_temperatura_termocupla)
{

	uint8_t spi_trama_datos_tx[2] = {0,0};
	uint8_t spi_trama_datos_rx[3] = {0,0,0};
	uint8_t contador_lectura = 0;

	/*02/03/22
	 * Se crea variable para calcular un offset no lineal */
	float temp_float = 0.0;
	float offset = 0.0;
	usr_temperatura_dato offset_nor = 0;
	usr_temperatura_dato temp_offset = 0;

	//Se hace una lectura de la temperatura para verificar el valor actual o si est� abierta
	spi_trama_datos_tx[0] = USR_TEMPERATURA_DIR_CR0_ESCRITURA;
	spi_trama_datos_tx[1] = USR_TEMPERATURA_MASK_LECTURA_SIMPLE | USR_TEMPERATURA_MASK_FAULT_HABILITADO;

	USR_TEMPERATURA_CS_Posicion(GPIO_PIN_RESET, prvusr_temperatura_termocupla->numero_termocupla);
	HAL_SPI_Transmit(&hspi1, spi_trama_datos_tx, sizeof(spi_trama_datos_tx), 10);
	USR_TEMPERATURA_CS_Posicion(GPIO_PIN_SET, prvusr_temperatura_termocupla->numero_termocupla);

	//Se espera hasta que se finalice la lectura...
	contador_lectura = 0;
	do
	{
		spi_trama_datos_tx[0] = USR_TEMPERATURA_DIR_CR0_LECTURA;

		USR_TEMPERATURA_CS_Posicion(GPIO_PIN_RESET, prvusr_temperatura_termocupla->numero_termocupla);
		HAL_SPI_TransmitReceive(&hspi1, spi_trama_datos_tx, spi_trama_datos_rx, sizeof(spi_trama_datos_tx), 10);
		USR_TEMPERATURA_CS_Posicion(GPIO_PIN_SET, prvusr_temperatura_termocupla->numero_termocupla);

		HAL_Delay(50);
		contador_lectura++;

	}while((spi_trama_datos_rx[1] & USR_TEMPERATURA_MASK_LECTURA_SIMPLE) && contador_lectura < USR_TEMPERATURA_INTENTOS_LECTURA);

	//Si no se finalizo la lectura es porque hubo error con el chip..
	if(contador_lectura >= USR_TEMPERATURA_INTENTOS_LECTURA)
	{
		USR_TEMPERATURA_Cambio_Led(prvusr_temperatura_termocupla->numero_termocupla, USR_TEMPERATURA_ESTADO_LED_ROJO);
		prvusr_temperatura_termocupla->estado_termocupla = USR_TEMPERATURA_ESTADO_TERMOCUPLA_ERROR;
		prvusr_temperatura_termocupla->temperatura_actual = 0;
		usr_temperatura_estado_alerta = USR_TEMPERATURA_ALERTA_INACTIVA;
	}
	else
	{

		//Si hubo una lectura se verifica si esta abierta la termocupla
		spi_trama_datos_tx[0] = USR_TEMPERATURA_DIR_SR_LECTURA;

		USR_TEMPERATURA_CS_Posicion(GPIO_PIN_RESET, prvusr_temperatura_termocupla->numero_termocupla);
		HAL_SPI_TransmitReceive(&hspi1, spi_trama_datos_tx, spi_trama_datos_rx, sizeof(spi_trama_datos_tx), 10);
		USR_TEMPERATURA_CS_Posicion(GPIO_PIN_SET, prvusr_temperatura_termocupla->numero_termocupla);

		if(spi_trama_datos_rx[1] & USR_TEMPERATURA_MASK_TERMOCUPLA_ABIERTA)
		{
			//Si esta abierta se cambia los parametro de la esctructura y se finaliza la funcion
			prvusr_temperatura_termocupla->estado_termocupla = USR_TEMPERATURA_ESTADO_TERMOCUPLA_ABIERTA;
			prvusr_temperatura_termocupla->temperatura_actual = 0;

			USR_TEMPERATURA_Cambio_Led(prvusr_temperatura_termocupla->numero_termocupla, USR_TEMPERATURA_ESTADO_LED_NARANJA);
			usr_temperatura_estado_alerta = USR_TEMPERATURA_ALERTA_INACTIVA;

		}
		else
		{
			//Si no esta abierta se guarda el dato de la temperatura se cambia los parametro de la estructura y se finaliza la funcion...
			spi_trama_datos_tx[0] = USR_TEMPERATURA_DIR_LTCBH_LECTURA;

			USR_TEMPERATURA_CS_Posicion(GPIO_PIN_RESET, prvusr_temperatura_termocupla->numero_termocupla);
			HAL_SPI_TransmitReceive(&hspi1, spi_trama_datos_tx, spi_trama_datos_rx, sizeof(spi_trama_datos_rx), 10);
			USR_TEMPERATURA_CS_Posicion(GPIO_PIN_SET, prvusr_temperatura_termocupla->numero_termocupla);

			//prvusr_temperatura_termocupla->temperatura_actual = (((usr_temperatura_dato)spi_trama_datos_rx[1]<<4) & 0x0FF0) | (((usr_temperatura_dato)spi_trama_datos_rx[2]>>4) & 0x000F);
			temp_offset = (((usr_temperatura_dato)spi_trama_datos_rx[1]<<4) & 0x0FF0) | (((usr_temperatura_dato)spi_trama_datos_rx[2]>>4) & 0x000F);

#if USR_TEMPERATURA_HABILITAR_OFFSET == 1
			//primera version de offset, funciona bien hasta 60 - 70 grados
			//prvusr_temperatura_termocupla->temperatura_actual = prvusr_temperatura_termocupla->temperatura_actual + (prvusr_temperatura_termocupla->temperatura_actual / 10);

			//Segunda version de offset, se intenta que ajuste mas a valores mas altos
			//temp_float = (float)prvusr_temperatura_termocupla->temperatura_actual;
			temp_float = (float)temp_offset;
			offset = temp_float / 20;
			if(offset >= 1.0)
			{

				offset = offset * offset;
				offset_nor = (usr_temperatura_dato)offset;
				//prvusr_temperatura_termocupla->temperatura_actual = prvusr_temperatura_termocupla->temperatura_actual + offset_nor;
				temp_offset = temp_offset + offset_nor;
				prvusr_temperatura_termocupla->temperatura_actual = temp_offset;
			}
			else
			{
				prvusr_temperatura_termocupla->temperatura_actual = temp_offset + 1;
			}

#endif

			//Se verifica si hay sobretemperatura... si no se mantiene la lectura normal...
			if(prvusr_temperatura_termocupla->temperatura_actual < USR_TEMPERATURA_MAXIMA)
			{
				USR_TEMPERATURA_Cambio_Led(prvusr_temperatura_termocupla->numero_termocupla, USR_TEMPERATURA_ESTADO_LED_VERDE);
				prvusr_temperatura_termocupla->estado_termocupla = USR_TEMPERATURA_ESTADO_TERMOCUPLA_OK;
				usr_temperatura_estado_alerta = USR_TEMPERATURA_ALERTA_INACTIVA;
			}
			else
			{
				USR_TEMPERATURA_Cambio_Led(prvusr_temperatura_termocupla->numero_termocupla, USR_TEMPERATURA_ESTADO_LED_ROJO);
				prvusr_temperatura_termocupla->estado_termocupla = USR_TEMPERATURA_ESTADO_TERMOCUPLA_SOBRETEMPERATURA;
				usr_temperatura_estado_alerta = USR_TEMPERATURA_ALERTA_ACTIVA;
			}

		}

	}

}

/*01/12/21
 * Funcion modificada para el control PID, faltan detalles menores de memoria*/
void USR_TEMPERATURA_Iniciar_Control(void)
{

	//Se inicia el contador de control que hace que cada periodo de tiempo se controle UNA salida
	/**************************************************************************************************
	Modificacion 29/11/21: se van a hacer pruebas cambiando el valor de USR_TIM_BASE_TIEMPO_TIM17,
	actualmente esta en 7000, que corresponde a 7 segundos, se va  a cambiar a 5000 para 5 segundos
	***************************************************************************************************/
	__HAL_TIM_SET_AUTORELOAD(&htim17,(USR_TIM_BASE_TIEMPO_TIM17-1));
	HAL_TIM_Base_Start_IT(&htim17);

}

/*01/12/21
 * Funcion modificada para el control PID, faltan detalles menores de memoria*/
void USR_TEMPERATURA_Detener_Control(void)
{

	HAL_TIM_Base_Stop_IT(&htim17);

}



void USR_TEMPERATURA_Control(void)
{
	uint8_t contador_control;

	__HAL_TIM_DISABLE_IT(&htim17, TIM_IT_UPDATE);

	for(contador_control = 0; contador_control < cantidad_termocuplas; contador_control++)
	{
		if((arreglo_termocuplas[contador_control].estado_control == USR_TEMPERATURA_ESTADO_CONTROL_ACTIV0) && (arreglo_termocuplas[contador_control].estado_termocupla == USR_TEMPERATURA_ESTADO_TERMOCUPLA_OK))
		{
			switch (arreglo_termocuplas[contador_control].tipo_control)
			{

				case USR_TEMPERATURA_TIPO_CONTROL_ONOFF:

					USR_TEMPERATURA_Control_ONOFF(&arreglo_termocuplas[contador_control]);

					break;

				case USR_TEMPERATURA_TIPO_CONTROL_PID:

					USR_TEMPERATURA_Control_PID(&arreglo_termocuplas[contador_control]);

					break;

				default:
					break;
			}
		}
	}

	//MOSTRAR VALOR ACTUAL DE LA TERMOCUPLA EN PANTALLAS!!!!
	/*01/12/21
	 * SE DEBE CAMBIAR ESTA FUNCION PARA MOSTRAR TODOS LOS VALORES AL TIEMPO*/
	USR_TEMPERATURA_Visualizar_Temperatura_Actual();

	usr_tim_estado_control_temperatura = USR_TIM_BANDERA_CONTROL_TEMPERATURA_NO_CAMBIO;
	__HAL_TIM_ENABLE_IT(&htim17, TIM_IT_UPDATE);

	return;
}



/*03/12/21
 * Se crea una funcion para revisar si el rele debe o no estar activo*/
void USR_TEMPERATURA_ESTADO_RELE(void)
{
	uint8_t contador_control;
	uint8_t bandera_estado = 0;

	for(contador_control = 0; contador_control < cantidad_termocuplas; contador_control++)
	{
		if((arreglo_termocuplas[contador_control].estado_control == USR_TEMPERATURA_ESTADO_CONTROL_ACTIV0) && (arreglo_termocuplas[contador_control].estado_termocupla == USR_TEMPERATURA_ESTADO_TERMOCUPLA_OK))
		{
			bandera_estado++;
		}
	}

	if(bandera_estado > 0)
	{
		if(USR_PROCESADOR_ESTADO_RELE_MANUAL() == GPIO_PIN_SET)
		{
			USR_PROCESADOR_RELE_MANUAL_INACTIVO();
		}
	}
	else
	{
		if(USR_PROCESADOR_ESTADO_RELE_MANUAL() == GPIO_PIN_RESET)
		{
			USR_PROCESADOR_RELE_MANUAL_ACTIVO();
		}
	}

	return;
}



/*01/12/21
 * Esta funcion remplazaria a la funcion de control, se llama cada vez que hay interrupcion de TIM
 * y se ejecuta el control de todas las salidas en ese instante de tiempo*/
void USR_TEMPERATURA_Control_PID(usr_struc_termocupla * prvusr_temperatura_termocupla)
{

	//Se adquiere el dato de temperatura...
	USR_TEMPERATURA_Leer_Temperatura(prvusr_temperatura_termocupla);

	if((prvusr_temperatura_termocupla->estado_control == USR_TEMPERATURA_ESTADO_CONTROL_ACTIV0) && (prvusr_temperatura_termocupla->estado_termocupla == USR_TEMPERATURA_ESTADO_TERMOCUPLA_OK))
	{

		/*01/12/21
		 * ACA SE DEBE LLAMAR A LA FUNCION DE CONTROL PID!!!!!, el valor de salida se configura como el valor
		 * de compare del canal correspondiente */
		USR_TEMPERATURA_PID(prvusr_temperatura_termocupla);

		switch(prvusr_temperatura_termocupla->numero_termocupla)
		{
			case USR_TEMPERATURA_TERMOCUPLA_1:

				/*01/12/21
				 * el valor de salida se configura como el valor de compare del canal correspondiente */
				if(prvusr_temperatura_termocupla->estado_pwm == USR_TEMPERATURA_ESTADO_PWM_INACTIV0)
				{
					HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
					__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
					__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, prvusr_temperatura_termocupla->pid.Control);
					__HAL_TIM_SET_COUNTER(&htim1, 0);
					HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
					prvusr_temperatura_termocupla->estado_pwm = USR_TEMPERATURA_ESTADO_PWM_ACTIV0;
				}
				else
				{

					while(!(__HAL_TIM_GET_FLAG(&htim1, TIM_IT_UPDATE)));
					__HAL_TIM_CLEAR_FLAG(&htim1, TIM_IT_UPDATE);
					__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, prvusr_temperatura_termocupla->pid.Control);
				}

				break;

			case USR_TEMPERATURA_TERMOCUPLA_2:

				/*01/12/21
				 * el valor de salida se configura como el valor de compare del canal correspondiente */
				if(prvusr_temperatura_termocupla->estado_pwm == USR_TEMPERATURA_ESTADO_PWM_INACTIV0)
				{
					HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
					__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
					__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, prvusr_temperatura_termocupla->pid.Control);
					__HAL_TIM_SET_COUNTER(&htim1, 0);
					HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
					prvusr_temperatura_termocupla->estado_pwm = USR_TEMPERATURA_ESTADO_PWM_ACTIV0;
				}
				else
				{

					while(!(__HAL_TIM_GET_FLAG(&htim1, TIM_IT_UPDATE)));
					__HAL_TIM_CLEAR_FLAG(&htim1, TIM_IT_UPDATE);
					__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, prvusr_temperatura_termocupla->pid.Control);
				}

				break;

			case USR_TEMPERATURA_TERMOCUPLA_3:

				if(prvusr_temperatura_termocupla->estado_pwm == USR_TEMPERATURA_ESTADO_PWM_INACTIV0)
				{
					HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);
					__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
					__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, prvusr_temperatura_termocupla->pid.Control);
					__HAL_TIM_SET_COUNTER(&htim1, 0);
					HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
					prvusr_temperatura_termocupla->estado_pwm = USR_TEMPERATURA_ESTADO_PWM_ACTIV0;
				}
				else
				{

					while(!(__HAL_TIM_GET_FLAG(&htim1, TIM_IT_UPDATE)));
					__HAL_TIM_CLEAR_FLAG(&htim1, TIM_IT_UPDATE);
					__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, prvusr_temperatura_termocupla->pid.Control);
				}

				break;

			case USR_TEMPERATURA_TERMOCUPLA_4:

				if(prvusr_temperatura_termocupla->estado_pwm == USR_TEMPERATURA_ESTADO_PWM_INACTIV0)
				{
					HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
					__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);
					__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, prvusr_temperatura_termocupla->pid.Control);
					__HAL_TIM_SET_COUNTER(&htim3, 0);
					HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
					prvusr_temperatura_termocupla->estado_pwm = USR_TEMPERATURA_ESTADO_PWM_ACTIV0;
				}
				else
				{

					while(!(__HAL_TIM_GET_FLAG(&htim3, TIM_IT_UPDATE)));
					__HAL_TIM_CLEAR_FLAG(&htim3, TIM_IT_UPDATE);
					__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, prvusr_temperatura_termocupla->pid.Control);
				}

				break;

			case USR_TEMPERATURA_TERMOCUPLA_5:

				if(prvusr_temperatura_termocupla->estado_pwm == USR_TEMPERATURA_ESTADO_PWM_INACTIV0)
				{
					HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
					__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);
					__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, prvusr_temperatura_termocupla->pid.Control);
					__HAL_TIM_SET_COUNTER(&htim3, 0);
					HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
					prvusr_temperatura_termocupla->estado_pwm = USR_TEMPERATURA_ESTADO_PWM_ACTIV0;
				}
				else
				{

					while(!(__HAL_TIM_GET_FLAG(&htim3, TIM_IT_UPDATE)));
					__HAL_TIM_CLEAR_FLAG(&htim3, TIM_IT_UPDATE);
					__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, prvusr_temperatura_termocupla->pid.Control);
				}

				break;

			case USR_TEMPERATURA_TERMOCUPLA_6:

				if(prvusr_temperatura_termocupla->estado_pwm == USR_TEMPERATURA_ESTADO_PWM_INACTIV0)
				{
					HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
					__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);
					__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, prvusr_temperatura_termocupla->pid.Control);
					__HAL_TIM_SET_COUNTER(&htim3, 0);
					HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
					prvusr_temperatura_termocupla->estado_pwm = USR_TEMPERATURA_ESTADO_PWM_ACTIV0;
				}
				else
				{

					while(!(__HAL_TIM_GET_FLAG(&htim3, TIM_IT_UPDATE)));
					__HAL_TIM_CLEAR_FLAG(&htim3, TIM_IT_UPDATE);
					__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, prvusr_temperatura_termocupla->pid.Control);
				}

				break;

		}

	}

}


/*02/12/21
 * COMO RESPALDO AL CONTROL SE DEJA UNA FUNCION DE TIPO ONOFF*/
void USR_TEMPERATURA_Control_ONOFF(usr_struc_termocupla * prvusr_temperatura_termocupla)
{

	//Se adquiere el dato de temperatura...
    USR_TEMPERATURA_Leer_Temperatura(prvusr_temperatura_termocupla);

	//Si la termocupla tiene activo el control se lee la temperatura actual...
	if((prvusr_temperatura_termocupla->estado_control == USR_TEMPERATURA_ESTADO_CONTROL_ACTIV0) && (prvusr_temperatura_termocupla->estado_termocupla == USR_TEMPERATURA_ESTADO_TERMOCUPLA_OK))
	{

		//Si la temperatura actual es menor que la configurada se activa el pwm en onepulsemode
		if(prvusr_temperatura_termocupla->temperatura_actual < prvusr_temperatura_termocupla->temperatura_configurada)
		{
			if(prvusr_temperatura_termocupla->estado_pwm == USR_TEMPERATURA_ESTADO_PWM_INACTIV0)
			{

				switch(prvusr_temperatura_termocupla->numero_termocupla)
				{
					case USR_TEMPERATURA_TERMOCUPLA_1:

						HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
						__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (((USR_TIM_BASE_TIEMPO_PWM - 1) * (101 - prvusr_temperatura_termocupla->porcentaje_potencia)) / 100));
						__HAL_TIM_SET_COUNTER(&htim1, 0);
						HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

						break;

					case USR_TEMPERATURA_TERMOCUPLA_2:

						HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
						__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, (((USR_TIM_BASE_TIEMPO_PWM - 1) * (101 - prvusr_temperatura_termocupla->porcentaje_potencia)) / 100));
						__HAL_TIM_SET_COUNTER(&htim1, 0);
						HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

						break;

					case USR_TEMPERATURA_TERMOCUPLA_3:

						HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);
						__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, (((USR_TIM_BASE_TIEMPO_PWM - 1) * prvusr_temperatura_termocupla->porcentaje_potencia) / 100));
						__HAL_TIM_SET_COUNTER(&htim1, 0);
						HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

						break;

					case USR_TEMPERATURA_TERMOCUPLA_4:

						HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
						__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (((USR_TIM_BASE_TIEMPO_PWM - 1) * prvusr_temperatura_termocupla->porcentaje_potencia) / 100));
						__HAL_TIM_SET_COUNTER(&htim3, 0);
						HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

						break;

					case USR_TEMPERATURA_TERMOCUPLA_5:

						HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
						__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, (((USR_TIM_BASE_TIEMPO_PWM - 1) * prvusr_temperatura_termocupla->porcentaje_potencia) / 100));
						__HAL_TIM_SET_COUNTER(&htim3, 0);
						HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

						break;

					case USR_TEMPERATURA_TERMOCUPLA_6:

						HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
						__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, (((USR_TIM_BASE_TIEMPO_PWM - 1) * prvusr_temperatura_termocupla->porcentaje_potencia) / 100));
						__HAL_TIM_SET_COUNTER(&htim3, 0);
						HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

						break;

				}

				prvusr_temperatura_termocupla->estado_pwm = USR_TEMPERATURA_ESTADO_PWM_ACTIV0;
			}
		}
		//Si la temperatura actual es mayor que la configurada se desactiva el pwm en onepulsemode
		else{

			if(prvusr_temperatura_termocupla->estado_pwm == USR_TEMPERATURA_ESTADO_PWM_ACTIV0)
			{

				switch(prvusr_temperatura_termocupla->numero_termocupla)
				{
					case USR_TEMPERATURA_TERMOCUPLA_1:


						HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);

						break;

					case USR_TEMPERATURA_TERMOCUPLA_2:

						HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);

						break;

					case USR_TEMPERATURA_TERMOCUPLA_3:

						HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);

						break;

					case USR_TEMPERATURA_TERMOCUPLA_4:

						HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);

						break;

					case USR_TEMPERATURA_TERMOCUPLA_5:

						HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);

						break;

					case USR_TEMPERATURA_TERMOCUPLA_6:

						HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);

						break;

				}

				prvusr_temperatura_termocupla->estado_pwm = USR_TEMPERATURA_ESTADO_PWM_INACTIV0;
			}
		}

	}

}


void USR_TEMPERATURA_PID(usr_struc_termocupla * prvusr_temperatura_termocupla)
{

	prvusr_temperatura_termocupla->pid.Error = prvusr_temperatura_termocupla->temperatura_configurada - prvusr_temperatura_termocupla->temperatura_actual;
	prvusr_temperatura_termocupla->pid.Proporcional = prvusr_temperatura_termocupla->pid.kp * prvusr_temperatura_termocupla->pid.Error;
	prvusr_temperatura_termocupla->pid.Integral = prvusr_temperatura_termocupla->pid.Integral + (prvusr_temperatura_termocupla->pid.Error * prvusr_temperatura_termocupla->pid.ki * USR_TIM_BASE_TIEMPO_TIM17_SEG);
	prvusr_temperatura_termocupla->pid.Derivativo = ((prvusr_temperatura_termocupla->pid.Error - prvusr_temperatura_termocupla->pid.Error_0) * prvusr_temperatura_termocupla->pid.kd) / USR_TIM_BASE_TIEMPO_TIM17_SEG;

	if(prvusr_temperatura_termocupla->pid.Integral > USR_TEMPERATURA_PID_MAX)
	{
		prvusr_temperatura_termocupla->pid.Integral = USR_TEMPERATURA_PID_MAX;
	}
	if(prvusr_temperatura_termocupla->pid.Derivativo > USR_TEMPERATURA_PID_MAX)
	{
		prvusr_temperatura_termocupla->pid.Derivativo = USR_TEMPERATURA_PID_MAX;
	}

	prvusr_temperatura_termocupla->pid.Control = (uint16_t)( prvusr_temperatura_termocupla->pid.Proporcional + prvusr_temperatura_termocupla->pid.Integral + prvusr_temperatura_termocupla->pid.Derivativo);

	if(prvusr_temperatura_termocupla->pid.Control > USR_TEMPERATURA_PID_MAX)
	{
		prvusr_temperatura_termocupla->pid.Control = USR_TEMPERATURA_PID_MAX;
	}
	else if(prvusr_temperatura_termocupla->pid.Control < USR_TEMPERATURA_PID_MIN)
	{
		prvusr_temperatura_termocupla->pid.Control = USR_TEMPERATURA_PID_MIN;
	}

	prvusr_temperatura_termocupla->pid.Control = USR_TEMPERATURA_PID_MAX - prvusr_temperatura_termocupla->pid.Control;

}

void USR_TEMPERATURA_Desactivar_Termocupla(usr_temperatura_termocupla termocupla)
{

	switch(termocupla)
	{
		case USR_TEMPERATURA_TERMOCUPLA_1:

			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);

			break;

		case USR_TEMPERATURA_TERMOCUPLA_2:

			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);

			break;

		case USR_TEMPERATURA_TERMOCUPLA_3:

			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);

			break;

		case USR_TEMPERATURA_TERMOCUPLA_4:

			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);

			break;

		case USR_TEMPERATURA_TERMOCUPLA_5:

			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);

			break;

		case USR_TEMPERATURA_TERMOCUPLA_6:

			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);

			break;

	}

	arreglo_termocuplas[termocupla].estado_control = USR_TEMPERATURA_ESTADO_CONTROL_INACTIV0;
	arreglo_termocuplas[termocupla].estado_pwm = USR_TEMPERATURA_ESTADO_PWM_INACTIV0;

}

/*01/12/21
 * Se modifica para que visualize todos los valores de temperatura de una sola vez*/
void USR_TEMPERATURA_Visualizar_Temperatura_Actual(void)
{
	uint8_t trama_envio[15];
	uint8_t cont_termocupla = 0;

	if(usr_nextion_pantalla_actual == USR_NEXTION_PANTALLA_EJECUTAR)
	{

		for(cont_termocupla = 0; cont_termocupla < cantidad_termocuplas; cont_termocupla++)
		{
			switch(cont_termocupla)
			{
				case USR_TEMPERATURA_TERMOCUPLA_1:

					sprintf((char *)trama_envio, "n29.val=%d", arreglo_termocuplas[cont_termocupla].temperatura_actual);

					break;

				case USR_TEMPERATURA_TERMOCUPLA_2:

					sprintf((char *)trama_envio, "n30.val=%d", arreglo_termocuplas[cont_termocupla].temperatura_actual);

					break;

				default:

					return;

			}

			USR_NEXTION_Enviar_Trama(trama_envio, strlen((const char *)trama_envio));
		}
	}

}

usr_temperatura_estado_termocupla USR_TEMPERATURA_Revisar_Estado(void)
{
	uint8_t contador;

	for(contador = 0; contador < cantidad_termocuplas; contador++)
	{
		if(arreglo_termocuplas[contador].estado_termocupla != USR_TEMPERATURA_ESTADO_TERMOCUPLA_OK)
		{
			return arreglo_termocuplas[contador].estado_termocupla;
		}

	}

	return USR_TEMPERATURA_ESTADO_TERMOCUPLA_OK;
}

void USR_TEMPERATURA_CS_Posicion(GPIO_PinState onoff, uint8_t pin)
{
	switch(pin)
	{
		case USR_TEMPERATURA_TERMOCUPLA_1:
			HAL_GPIO_WritePin(SPI_CS1_GPIO_Port, SPI_CS1_Pin, onoff);
			break;

		case USR_TEMPERATURA_TERMOCUPLA_2:
			HAL_GPIO_WritePin(SPI_CS2_GPIO_Port, SPI_CS2_Pin, onoff);
			break;

		case USR_TEMPERATURA_TERMOCUPLA_3:
			HAL_GPIO_WritePin(SPI_CS3_GPIO_Port, SPI_CS3_Pin, onoff);
			break;

		case USR_TEMPERATURA_TERMOCUPLA_4:
			HAL_GPIO_WritePin(SPI_CS4_GPIO_Port, SPI_CS4_Pin, onoff);
			break;

		case USR_TEMPERATURA_TERMOCUPLA_5:
			HAL_GPIO_WritePin(SPI_CS5_GPIO_Port, SPI_CS5_Pin, onoff);
			break;

		case USR_TEMPERATURA_TERMOCUPLA_6:
			HAL_GPIO_WritePin(SPI_CS6_GPIO_Port, SPI_CS6_Pin, onoff);
			break;

	}

}

void USR_TEMPERATURA_Cambio_Led(usr_temperatura_led led, usr_temperatura_estado_led estado)
{

	GPIO_TypeDef* PuertoVerde, *PuertoRojo;
	uint16_t	PinVerde, PinRojo;

	PuertoVerde = LED_VERDE_1_GPIO_Port;
	PinVerde = LED_VERDE_1_Pin;
	PuertoRojo = LED_ROJO_1_GPIO_Port;
	PinRojo = LED_ROJO_1_Pin;

	switch(led)
	{
		case USR_TEMPERATURA_LED_1:

			PuertoVerde = LED_VERDE_1_GPIO_Port;
			PinVerde = LED_VERDE_1_Pin;
			PuertoRojo = LED_ROJO_1_GPIO_Port;
			PinRojo = LED_ROJO_1_Pin;

			break;

		case USR_TEMPERATURA_LED_2:

			PuertoVerde = LED_VERDE_2_GPIO_Port;
			PinVerde = LED_VERDE_2_Pin;
			PuertoRojo = LED_ROJO_2_GPIO_Port;
			PinRojo = LED_ROJO_2_Pin;

			break;

		case USR_TEMPERATURA_LED_3:

			PuertoVerde = LED_VERDE_3_GPIO_Port;
			PinVerde = LED_VERDE_3_Pin;
			PuertoRojo = LED_ROJO_3_GPIO_Port;
			PinRojo = LED_ROJO_3_Pin;

			break;

		case USR_TEMPERATURA_LED_4:

			PuertoVerde = LED_VERDE_4_GPIO_Port;
			PinVerde = LED_VERDE_4_Pin;
			PuertoRojo = LED_ROJO_4_GPIO_Port;
			PinRojo = LED_ROJO_4_Pin;

			break;

		case USR_TEMPERATURA_LED_5:

			PuertoVerde = LED_VERDE_5_GPIO_Port;
			PinVerde = LED_VERDE_5_Pin;
			PuertoRojo = LED_ROJO_5_GPIO_Port;
			PinRojo = LED_ROJO_5_Pin;

			break;

		case USR_TEMPERATURA_LED_6:

			PuertoVerde = LED_VERDE_6_GPIO_Port;
			PinVerde = LED_VERDE_6_Pin;
			PuertoRojo = LED_ROJO_6_GPIO_Port;
			PinRojo = LED_ROJO_6_Pin;

			break;

	}

	switch(estado)
	{
		case USR_TEMPERATURA_ESTADO_LED_APAGADO:

			HAL_GPIO_WritePin(PuertoVerde, PinVerde, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PuertoRojo, PinRojo, GPIO_PIN_RESET);

			break;

		case USR_TEMPERATURA_ESTADO_LED_VERDE:

			HAL_GPIO_WritePin(PuertoVerde, PinVerde, GPIO_PIN_SET);
			HAL_GPIO_WritePin(PuertoRojo, PinRojo, GPIO_PIN_RESET);

			break;

		case USR_TEMPERATURA_ESTADO_LED_ROJO:

			HAL_GPIO_WritePin(PuertoVerde, PinVerde, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(PuertoRojo, PinRojo, GPIO_PIN_SET);

			break;

		case USR_TEMPERATURA_ESTADO_LED_NARANJA:

			HAL_GPIO_WritePin(PuertoVerde, PinVerde, GPIO_PIN_SET);
			HAL_GPIO_WritePin(PuertoRojo, PinRojo, GPIO_PIN_SET);

			break;

	}

}
