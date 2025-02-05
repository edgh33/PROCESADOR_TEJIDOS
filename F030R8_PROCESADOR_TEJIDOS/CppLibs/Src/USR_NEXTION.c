/*
 * USR_NEXTION.c
 *
 *  Created on: 1/09/2018
 *      Author: edgh3
 */

#include "USR_NEXTION.h"

HAL_StatusTypeDef usr_nextion_status;
uint8_t usr_nextion_pantalla_actual = USR_NEXTION_PANTALLA_INICIO;
uint8_t usr_nextion_buffer_primer_dato;
uint8_t usr_nextion_buffer_rx[USR_NEXTION_BUFFER_RX_TAM];
uint8_t usr_nextion_comando_numero[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
usr_nextion_trama usr_nextion_estado_trama = USR_NEXTION_TRAMA_NO_RECIBIDA;

uint8_t usr_nextion_com_pantalla_inicio[]="page inicio";
uint8_t usr_nextion_com_pantalla_menu[]="page menu";
uint8_t usr_nextion_com_pantalla_ejecutar[]="page ejecutar";
uint8_t usr_nextion_com_pantalla_configurar[]="page configurar";
uint8_t usr_nextion_com_pantalla_reloj[]="page reloj";
uint8_t usr_nextion_com_pantalla_manual[]="page manual";
uint8_t usr_nextion_com_pantalla_calendario[]="page calendario";
uint8_t usr_nextion_com_pantalla_alerta[]="page alerta";
uint8_t usr_nextion_com_pantalla_termocuplas[]="page termocuplas";

uint8_t usr_nextion_com_vison_b0[]="vis b0,1";
uint8_t usr_nextion_com_visoff_b0[]="vis b0,0";

uint8_t usr_nextion_com_vison_bt0[]="vis bt0,1";
uint8_t usr_nextion_com_visoff_bt0[]="vis bt0,0";

uint8_t usr_nextion_com_vison_bt14[]="vis bt14,1";
uint8_t usr_nextion_com_visoff_bt14[]="vis bt14,0";

uint8_t usr_nextion_com_todo_habilitado[]="tsw 255,1";
uint8_t usr_nextion_com_todo_inhabilitado[]="tsw 255,0";

uint8_t usr_nextion_com_habilita_bt20[]="tsw bt20,1";
uint8_t usr_nextion_com_deshabilita_bt20[]="tsw bt20,0";

uint8_t usr_nextion_com_habilita_bt21[]="tsw bt21,1";
uint8_t usr_nextion_com_deshabilita_bt21[]="tsw bt21,0";

uint8_t usr_nextion_texto_rtc_ok[]="t5.txt=\"       Reloj configurado                correctamente!!!\"";
uint8_t usr_nextion_texto_rtc_error[]="t5.txt=\"Error al configurar reloj...\"";

uint8_t usr_nextion_texto_memoria_ok[]="t0.txt=\"    Programa guardado       correctamente!!!\"";
uint8_t usr_nextion_texto_memoria_error[]="t0.txt=\"    Error al guardar      programa, intente          de nuevo...\"";

uint8_t usr_nextion_texto_termocuplas_ok[]="t0.txt=\"Termocuplas guardadas   correctamente!!!\"";
uint8_t usr_nextion_texto_termocuplas_error[]="t0.txt=\"  Error al guardar,  intente nuevamente...\"";

uint8_t usr_nextion_texto_memoria_rst_ok[]="t0.txt=\"  Memoria reiniciada      correctamente!!!\"";
uint8_t usr_nextion_texto_memoria_rst_error[]="t0.txt=\"  Error al reiniciar          memoria...\"";

uint8_t usr_nextion_texto_menu_ok[]="t0.txt=\"Equipo funcionando correctamente\"";
uint8_t usr_nextion_texto_menu_error_termocuplas[]="t0.txt=\"Fallo en una o mas termocuplas...\"";
uint8_t usr_nextion_texto_menu_error_rtc[]="t0.txt=\"Reloj desconfigurado...\"";

uint8_t usr_nextion_texto_programa_finalizado[]="t17.txt=\"FINALIZADO!!!\"";
uint8_t usr_nextion_texto_programa_ejecutando[]="t17.txt=\"EJECUTANDO\"";
uint8_t usr_nextion_texto_programa_pausado[]="t17.txt=\"PAUSADO...\"";
uint8_t usr_nextion_texto_programa_retardo[]="t17.txt=\"EN RETARDO...\"";
uint8_t usr_nextion_texto_programa_rotando[]="t17.txt=\"ROTANDO\"";

uint8_t usr_nextion_texto_canasta_arriba[]="t0.txt=\"CANASTA ARRIBA\"";
uint8_t usr_nextion_texto_canasta_abajo[]="t0.txt=\"CANASTA ABAJO\"";

uint8_t endflag[] = {0xFF, 0xFF, 0xFF, 0};


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	usr_nextion_status = USR_NEXTION_Recibir_Trama();
	if(usr_nextion_status == HAL_OK)
	{
		usr_nextion_estado_trama = USR_NEXTION_TRAMA_RECIBIDA;
	}
	else
	{
		usr_nextion_estado_trama = USR_NEXTION_TRAMA_NO_RECIBIDA;
	}

}


HAL_StatusTypeDef USR_NEXTION_Iniciar_Recibir_Trama(void)
{

	__HAL_UART_FLUSH_DRREGISTER(&huart2);
	usr_nextion_status = HAL_UART_Receive_IT(&huart2, &usr_nextion_buffer_primer_dato, 1);
	return usr_nextion_status;
}

HAL_StatusTypeDef USR_NEXTION_Detener_Recibir_Trama(void)
{

	usr_nextion_status = HAL_UART_AbortReceive_IT(&huart2);
	return usr_nextion_status;
}



HAL_StatusTypeDef USR_NEXTION_Recibir_Trama(void)
{
	uint8_t bandera_error = 0;
	uint8_t* pbuffer_rx = usr_nextion_buffer_rx;

	//Se almacena el primer dato recibido y se aumenta el apuntador
	*pbuffer_rx=usr_nextion_buffer_primer_dato;
	pbuffer_rx++;

	//Se reciben los demas caracteres hasta que se encuentre un enter...
	do
	{
		if(HAL_UART_Receive(&huart2, &usr_nextion_buffer_primer_dato, 1, 2) != HAL_OK)
		{
			bandera_error = 1;
			break;
		}
		else
		{
			*pbuffer_rx = usr_nextion_buffer_primer_dato;
			pbuffer_rx++;
		}

	}while(usr_nextion_buffer_primer_dato != 0x0D);

	pbuffer_rx--;
	*pbuffer_rx = 0;

	if(bandera_error == 0)
	{
		if(HAL_UART_Receive(&huart2, &usr_nextion_buffer_primer_dato, 1, 2) != HAL_OK)
		{
			return HAL_ERROR;
		}
		else
		{
			if(usr_nextion_buffer_primer_dato == 0x0A)
			{
				return HAL_OK;
			}
			else
			{
				return HAL_ERROR;
			}
		}
	}
	else
	{
		return HAL_ERROR;
	}

}


HAL_StatusTypeDef USR_NEXTION_Enviar_Trama(uint8_t* trama_enviar, uint16_t tam_trama)
{
	uint8_t usr_nextion_comando[120];
	sprintf((char *)usr_nextion_comando,"%s",(char *)trama_enviar);

	strcat((char *)usr_nextion_comando, (const char *)endflag);

	usr_nextion_status = HAL_UART_Transmit(&huart2, usr_nextion_comando, strlen((char *)usr_nextion_comando), 70);
	return usr_nextion_status;

}

uint32_t USR_NEXTION_Obtener_Dato_Trama(uint32_t posicion)
{
	char *trama_dato;
	trama_dato = (char *) malloc(10);
	char *trama_dato_temp = trama_dato;
	uint8_t *usr_nextion_buffer_rx_temp = usr_nextion_buffer_rx;
	uint32_t valor_dato = 0;
	uint32_t contador_pos = 0;

	while(contador_pos < posicion)
	{
		if(*usr_nextion_buffer_rx_temp == ':')
		{
			contador_pos++;
		}
		usr_nextion_buffer_rx_temp++;
	}

	while((*usr_nextion_buffer_rx_temp != ':') && (*usr_nextion_buffer_rx_temp != 0))
	{
		*trama_dato_temp = *usr_nextion_buffer_rx_temp;
		trama_dato_temp++;
		usr_nextion_buffer_rx_temp++;
	}

	*trama_dato_temp = 0;

	valor_dato = atoi(trama_dato);

	free(trama_dato);

	return valor_dato;

}


void USR_NEXTION_Guardar_RTC(void)
{

	uint8_t dato_rtc = 0;
	uint8_t dato_ampm = 0;

	usr_rtc_config[USR_RTC_POSICION_SEGUNDO] = 0;
	dato_rtc = USR_NEXTION_Obtener_Dato_Trama((uint32_t)USR_NEXTION_POSICION_RTC_MINUTO);
	usr_rtc_config[USR_RTC_POSICION_MINUTO] = RTC_ByteToBcd2(dato_rtc);
	dato_rtc = USR_NEXTION_Obtener_Dato_Trama((uint32_t)USR_NEXTION_POSICION_RTC_HORA);
	dato_ampm = USR_NEXTION_Obtener_Dato_Trama((uint32_t)USR_NEXTION_POSICION_RTC_AMPM);
	usr_rtc_config[USR_RTC_POSICION_HORA] = RTC_ByteToBcd2(dato_rtc) | (dato_ampm<<USR_RTC_AMPM_POS) | USR_RTC_12H_MASK;
	dato_rtc = USR_NEXTION_Obtener_Dato_Trama((uint32_t)USR_NEXTION_POSICION_RTC_DIA_SEMANA);
	usr_rtc_config[USR_RTC_POSICION_DIA_SEMANA] = RTC_ByteToBcd2(dato_rtc);
	dato_rtc = USR_NEXTION_Obtener_Dato_Trama((uint32_t)USR_NEXTION_POSICION_RTC_DIA);
	usr_rtc_config[USR_RTC_POSICION_DIA] = RTC_ByteToBcd2(dato_rtc);
	dato_rtc = USR_NEXTION_Obtener_Dato_Trama((uint32_t)USR_NEXTION_POSICION_RTC_MES);
	usr_rtc_config[USR_RTC_POSICION_MES] = RTC_ByteToBcd2(dato_rtc);
	dato_rtc = USR_NEXTION_Obtener_Dato_Trama((uint32_t)USR_NEXTION_POSICION_RTC_ANHO);
	usr_rtc_config[USR_RTC_POSICION_ANHO] = RTC_ByteToBcd2(dato_rtc);
	usr_rtc_config[USR_RTC_POSICION_CONFIGURACION] = 0;

	USR_RTC_Escribir_Fecha_Hora();

	HAL_Delay(10);

	if (USR_RTC_Comparar() == USR_RTC_ESTADO_OK)
	{
		USR_NEXTION_Enviar_Trama(usr_nextion_texto_rtc_ok,  (uint16_t)strlen((const char *)usr_nextion_texto_rtc_ok));
	}
	else
	{
		USR_NEXTION_Enviar_Trama(usr_nextion_texto_rtc_error,  (uint16_t)strlen((const char *)usr_nextion_texto_rtc_error));
	}

}


void USR_NEXTION_Visualizar_RTC(void)
{

	uint8_t minuto = 0, hora = 0, dia_sem = 0, dia = 0, mes = 0, anho = 0;
	uint8_t texto_fecha[30];
	uint8_t texto_anho[10];

	USR_RTC_Leer_Fecha_Hora();

	minuto = usr_rtc_read[USR_RTC_POSICION_MINUTO];
	minuto = RTC_Bcd2ToByte(minuto);

	hora = usr_rtc_read[USR_RTC_POSICION_HORA];

	if(hora & USR_RTC_AMPM_MASK){

		hora = RTC_Bcd2ToByte(hora & 0x1F);
		sprintf((char *)texto_fecha,"t1.txt=\"%02u:%02uPM",hora,minuto);
	}
	else
	{
		hora = RTC_Bcd2ToByte(hora & 0x1F);
		sprintf((char *)texto_fecha,"t1.txt=\"%02u:%02uAM",hora,minuto);

	}

	dia_sem = usr_rtc_read[USR_RTC_POSICION_DIA_SEMANA];

	//Dependiendo el dia de la semana se arma la trama y se envia a la pantalla
	switch(dia_sem){

		case USR_RTC_DIA_LUNES:

			strcat((char *)texto_fecha, "-Lunes\"");

		break;

		case USR_RTC_DIA_MARTES:

			strcat((char *)texto_fecha, "-Martes\"");

		break;

		case USR_RTC_DIA_MIERCOLES:

			strcat((char *)texto_fecha, "-Miercoles\"");

		break;

		case USR_RTC_DIA_JUEVES:

			strcat((char *)texto_fecha, "-Jueves\"");

		break;

		case USR_RTC_DIA_VIERNES:

			strcat((char *)texto_fecha, "-Viernes\"");

		break;

		case USR_RTC_DIA_SABADO:

			strcat((char *)texto_fecha, "-Sabado\"");

		break;

		case USR_RTC_DIA_DOMINGO:

			strcat((char *)texto_fecha, "-Domingo\"");

		break;

	}//switch(dia_sem)


	//Se envia el dato a la pantalla
	USR_NEXTION_Enviar_Trama(texto_fecha, (uint16_t)strlen((const char *)texto_fecha));

	dia = usr_rtc_read[USR_RTC_POSICION_DIA];
	dia = RTC_Bcd2ToByte(dia);

	sprintf((char *)texto_fecha,"t2.txt=\"%02u",dia);

	mes = usr_rtc_read[USR_RTC_POSICION_MES];
	mes = RTC_Bcd2ToByte(mes);

	//Dependiendo el mes se arma la trama y se envia a la pantalla
	switch(mes){

		case USR_RTC_MES_ENERO:

			strcat((char *)texto_fecha, "/Enero/");

		break;

		case USR_RTC_MES_FEBRERO:

			strcat((char *)texto_fecha, "/Febrero/");

		break;

		case USR_RTC_MES_MARZO:

			strcat((char *)texto_fecha, "/Marzo/");

		break;

		case USR_RTC_MES_ABRIL:

			strcat((char *)texto_fecha, "/Abril/");

		break;

		case USR_RTC_MES_MAYO:

			strcat((char *)texto_fecha, "/Mayo/");

		break;

		case USR_RTC_MES_JUNIO:

			strcat((char *)texto_fecha, "/Junio/");

		break;

		case USR_RTC_MES_JULIO:

			strcat((char *)texto_fecha, "/Julio/");

		break;

		case USR_RTC_MES_AGOSTO:

			strcat((char *)texto_fecha, "/Agosto/");

		break;

		case USR_RTC_MES_SEPTIEMBRE:

			strcat((char *)texto_fecha, "/Septiembre/");

		break;

		case USR_RTC_MES_OCTUBRE:

			strcat((char *)texto_fecha, "/Octubre/");

		break;

		case USR_RTC_MES_NOVIEMBRE:

			strcat((char *)texto_fecha, "/Noviembre/");

		break;

		case USR_RTC_MES_DICIEMBRE:

			strcat((char *)texto_fecha, "/Diciembre/");

		break;

	}//switch(dia_sem)

	anho = usr_rtc_read[USR_RTC_POSICION_ANHO];
	anho = RTC_Bcd2ToByte(anho);

	sprintf((char *)texto_anho,"20%02u\"",anho);

	strcat((char *)texto_fecha, (const char *)texto_anho);

	//Se envia el dato a la pantalla
	USR_NEXTION_Enviar_Trama(texto_fecha, (uint16_t)strlen((const char *)texto_fecha));

}


void USR_NEXTION_Visualizar_Alerta(void)
{

	uint8_t avisos_alerta[120];

	sprintf((char *)avisos_alerta, "t50.txt=\"  TEMPERATURA    ALTA       REINICIE EL     EQUIPO...\"");
	USR_NEXTION_Enviar_Trama(avisos_alerta, (uint16_t)strlen((const char *)avisos_alerta));

	USR_PROCESADOR_ReleAC(USR_PROCESADOR_RELEAC_INACTIVO);
	USR_PROCESADOR_RELE_MANUAL_ACTIVO();

	while(1)
	{
		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
		HAL_Delay(1000);
		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
		HAL_Delay(1000);
	}
}


void USR_NEXTION_Visualizar_Termocuplas(void)
{
	uint8_t contador;
	uint8_t trama_temp[40];
	uint32_t valor_termocupla;
	uint32_t cant_termocupla;

	USR_FLASH_Leer_Termocuplas();

	cant_termocupla =  USR_FLASH_Obtener_Dato_Trama(USR_TEMPERATURA_POSICION_CANTIDAD);

	for(contador = 0; contador < USR_TEMPERATURA_CANTIDAD_MAX_TERMOCUPLAS; contador++)
	{

		valor_termocupla = USR_FLASH_Obtener_Dato_Trama(USR_TEMPERATURA_POSICION_TEMOCUPLA_1+contador);

		if(contador < cant_termocupla)
		{
			switch(valor_termocupla)
			{

				case USR_TEMPERATURA_TERMOCUPLA_B:
					sprintf((char *)trama_temp,"t%d.txt=\"Termocupla %d: TIPO B\"",(contador+20),(contador+1));
					break;

				case USR_TEMPERATURA_TERMOCUPLA_E:
					sprintf((char *)trama_temp,"t%d.txt=\"Termocupla %d: TIPO E\"",(contador+20),(contador+1));
					break;

				case USR_TEMPERATURA_TERMOCUPLA_J:
					sprintf((char *)trama_temp,"t%d.txt=\"Termocupla %d: TIPO J\"",(contador+20),(contador+1));
							break;

				case USR_TEMPERATURA_TERMOCUPLA_K:
					sprintf((char *)trama_temp,"t%d.txt=\"Termocupla %d: TIPO K\"",(contador+20),(contador+1));
							break;

				case USR_TEMPERATURA_TERMOCUPLA_N:
					sprintf((char *)trama_temp,"t%d.txt=\"Termocupla %d: TIPO N\"",(contador+20),(contador+1));
							break;

				case USR_TEMPERATURA_TERMOCUPLA_R:
					sprintf((char *)trama_temp,"t%d.txt=\"Termocupla %d: TIPO R\"",(contador+20),(contador+1));
					break;

				case USR_TEMPERATURA_TERMOCUPLA_S:
					sprintf((char *)trama_temp,"t%d.txt=\"Termocupla %d: TIPO S\"",(contador+20),(contador+1));
					break;

				case USR_TEMPERATURA_TERMOCUPLA_T:
					sprintf((char *)trama_temp,"t%d.txt=\"Termocupla %d: TIPO T\"",(contador+20),(contador+1));
					break;

				default:
					sprintf((char *)trama_temp,"t%d.txt=\"Termocupla %d: ERROR\"",(contador+20),(contador+1));

			}

		}
		else
		{
			sprintf((char *)trama_temp,"t%d.txt=\"Termocupla %d: N.C.\"",(contador+20),(contador+1));
		}

		USR_NEXTION_Enviar_Trama(trama_temp, (uint16_t)strlen((const char *)trama_temp));
	}

}


void USR_NEXTION_Mostrar_Programa(uint8_t numProg)
{

	uint32_t valTemp = 0;
	uint8_t numero_elemento = 0;

	USR_FLASH_Leer_Programa(numProg);

	//El programa queda almacenado en: usr_flash_buffer_programa

	for(numero_elemento = 0; numero_elemento < USR_PROCESADOR_NUMERO_ELEMENTOS; numero_elemento++)
	{
		//Envio la hora a los elemento correspondiente
		valTemp = USR_FLASH_Obtener_Dato_Trama((uint32_t)((numero_elemento*2) + USR_PROCESADOR_POSICION_R1_HORA));
		sprintf((char *)usr_nextion_comando_numero,"n%d.val=%d",(int)(numero_elemento*2),(int)valTemp);
		USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

		if(usr_nextion_pantalla_actual == USR_NEXTION_PANTALLA_CONFIGURAR)
		{
			sprintf((char *)usr_nextion_comando_numero,"h%d.val=%d",(int)(numero_elemento+1),(int)valTemp);
			USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));
		}
		//Envio los minutos a los elemento correspondiente
		valTemp = USR_FLASH_Obtener_Dato_Trama((uint32_t)((numero_elemento*2) + USR_PROCESADOR_POSICION_R1_MINUTO));
		sprintf((char *)usr_nextion_comando_numero,"n%d.val=%d",(int)((numero_elemento*2)+1),(int)valTemp);
		USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

		if(usr_nextion_pantalla_actual == USR_NEXTION_PANTALLA_CONFIGURAR)
		{
			sprintf((char *)usr_nextion_comando_numero,"m%d.val=%d",(int)(numero_elemento+1),(int)valTemp);
			USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));
		}
	}

	numero_elemento = 1;
	if(usr_nextion_pantalla_actual == USR_NEXTION_PANTALLA_EJECUTAR)
	{

		sprintf((char *)usr_nextion_comando_numero,"vis q%d,1",(int)(numero_elemento));
		USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

	}
	else if(usr_nextion_pantalla_actual == USR_NEXTION_PANTALLA_CONFIGURAR)
	{
		sprintf((char *)usr_nextion_comando_numero,"bt%d.val=1",(int)(numero_elemento));
		USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));
	}

	for(numero_elemento = 2; numero_elemento < USR_PROCESADOR_NUMERO_ELEMENTOS+1; numero_elemento++)
	{
		if(usr_nextion_pantalla_actual == USR_NEXTION_PANTALLA_EJECUTAR)
		{

			sprintf((char *)usr_nextion_comando_numero,"vis q%d,0",(int)(numero_elemento));
			USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

		}
		else if(usr_nextion_pantalla_actual == USR_NEXTION_PANTALLA_CONFIGURAR)
		{
			sprintf((char *)usr_nextion_comando_numero,"bt%d.val=0",(int)(numero_elemento));
			USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));
		}
	}

}


void USR_NEXTION_Mostrar_Programa_Retomado(void)
{

	uint32_t valTemp = 0, valTemp2 = 0;
	uint8_t numero_elemento = 0;
	uint8_t elemento_actual = 0;

	//El programa queda almacenado en: usr_flash_buffer_estado_actual
	//USR_FLASH_Leer_Estado_Actual();

	//Se muestra el boton de detener...
	sprintf((char *)usr_nextion_comando_numero,"vis b4,0");
	USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

	sprintf((char *)usr_nextion_comando_numero,"vis b1,1");
	USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

	//Se oculta el boton de programa
	sprintf((char *)usr_nextion_comando_numero,"vis b6,0");
	USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

	//Se muestra el texto de PROGRAMA
	sprintf((char *)usr_nextion_comando_numero,"vis t0,1");
	USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));


	//Se muestra el programa actual
	valTemp = USR_FLASH_Obtener_Dato_Trama_Estado_Actual((uint32_t)((numero_elemento*2) + USR_PROCESADOR_POSICION_PROGRAMA));
	sprintf((char *)usr_nextion_comando_numero,"n24.val=%d", (int)valTemp);
	USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

	sprintf((char *)usr_nextion_comando_numero,"prog.val=%d", (int)valTemp);
	USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

	//Se ubican los numeros de las horas y los minutos de cada proceso en el estado actual...
	for(numero_elemento = 0; numero_elemento < USR_PROCESADOR_NUMERO_ELEMENTOS; numero_elemento++)
	{
		//Envio la hora a los elemento correspondiente
		valTemp = USR_FLASH_Obtener_Dato_Trama_Estado_Actual((uint32_t)((numero_elemento*2) + USR_PROCESADOR_POSICION_R1_HORA));
		sprintf((char *)usr_nextion_comando_numero,"n%d.val=%d",(int)(numero_elemento*2),(int)valTemp);
		USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

		//Envio los minutos a los elemento correspondiente
		valTemp = USR_FLASH_Obtener_Dato_Trama_Estado_Actual((uint32_t)((numero_elemento*2) + USR_PROCESADOR_POSICION_R1_MINUTO));
		sprintf((char *)usr_nextion_comando_numero,"n%d.val=%d",(int)((numero_elemento*2)+1),(int)valTemp);
		USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

	}

	//Se activa el recipiente actual
	elemento_actual = USR_FLASH_Obtener_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_POSICION_ACTUAL);

	USR_NEXTION_Activar_Recipiente(elemento_actual+1);

	//Se carga el ultimo valor registrado de temperatura 1
	valTemp = USR_FLASH_Obtener_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_T1_VALOR);
	arreglo_termocuplas[USR_TEMPERATURA_TERMOCUPLA_1].temperatura_configurada = valTemp;

	sprintf((char *)usr_nextion_comando_numero,"temp1.val=%d", (int)valTemp);
	USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

	sprintf((char *)usr_nextion_comando_numero,"n25.val=%d", (int)valTemp);
	USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

	//Se ajustan los botones para la temperatura 1
	if( USR_FLASH_Obtener_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_T1_ESTADO) == USR_TEMPERATURA_ESTADO_CONTROL_ACTIV0)
	{

		sprintf((char *)usr_nextion_comando_numero,"vis b2,0");
		USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

		sprintf((char *)usr_nextion_comando_numero,"vis b3,0");
		USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

		sprintf((char *)usr_nextion_comando_numero,"bt20.val=1");
		USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

		sprintf((char *)usr_nextion_comando_numero,"vis n29,1");
		USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

		arreglo_termocuplas[USR_TEMPERATURA_TERMOCUPLA_1].estado_control = USR_TEMPERATURA_ESTADO_CONTROL_ACTIV0;
		arreglo_termocuplas[USR_TEMPERATURA_TERMOCUPLA_1].estado_pwm = USR_TEMPERATURA_ESTADO_PWM_INACTIV0;

	}
	else
	{
		arreglo_termocuplas[USR_TEMPERATURA_TERMOCUPLA_1].estado_control = USR_TEMPERATURA_ESTADO_CONTROL_INACTIV0;
		arreglo_termocuplas[USR_TEMPERATURA_TERMOCUPLA_1].estado_pwm = USR_TEMPERATURA_ESTADO_PWM_INACTIV0;
	}

	//Se carga el ultimo valor registrado de temperatura 2
	valTemp = USR_FLASH_Obtener_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_T2_VALOR);
	arreglo_termocuplas[USR_TEMPERATURA_TERMOCUPLA_2].temperatura_configurada = valTemp;

	sprintf((char *)usr_nextion_comando_numero,"temp2.val=%d", (int)valTemp);
	USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

	sprintf((char *)usr_nextion_comando_numero,"n26.val=%d", (int)valTemp);
	USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

	//Se ajustan los botones para la temperatura 2
	if( USR_FLASH_Obtener_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_T2_ESTADO) == USR_TEMPERATURA_ESTADO_CONTROL_ACTIV0)
	{

		sprintf((char *)usr_nextion_comando_numero,"vis b5,0");
		USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

		sprintf((char *)usr_nextion_comando_numero,"vis b7,0");
		USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

		sprintf((char *)usr_nextion_comando_numero,"bt21.val=1");
		USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

		sprintf((char *)usr_nextion_comando_numero,"vis n30,1");
		USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

		arreglo_termocuplas[USR_TEMPERATURA_TERMOCUPLA_2].estado_control = USR_TEMPERATURA_ESTADO_CONTROL_ACTIV0;
		arreglo_termocuplas[USR_TEMPERATURA_TERMOCUPLA_2].estado_pwm = USR_TEMPERATURA_ESTADO_PWM_INACTIV0;

	}
	else
	{
		arreglo_termocuplas[USR_TEMPERATURA_TERMOCUPLA_2].estado_control = USR_TEMPERATURA_ESTADO_CONTROL_INACTIV0;
		arreglo_termocuplas[USR_TEMPERATURA_TERMOCUPLA_2].estado_pwm = USR_TEMPERATURA_ESTADO_PWM_INACTIV0;
	}

	//Se ocultan los botones de control de retardo
	sprintf((char *)usr_nextion_comando_numero,"vis b0,0");
	USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

	sprintf((char *)usr_nextion_comando_numero,"vis b8,0");
	USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

	sprintf((char *)usr_nextion_comando_numero,"vis b9,0");
	USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

	sprintf((char *)usr_nextion_comando_numero,"vis b10,0");
	USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));


	//Se ajustan los numeros del retardo si aun estaba en retardo... y se ajusta el texto
	valTemp = USR_FLASH_Obtener_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_RETARDO_HORAS);
	valTemp2 = USR_FLASH_Obtener_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_RETARDO_MINUTOS);

	if(( valTemp != 0) || (valTemp2 != 0))
	{

		sprintf((char *)usr_nextion_comando_numero,"n27.val=%d", (int)valTemp);
		USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

		sprintf((char *)usr_nextion_comando_numero,"n28.val=%d", (int)valTemp2);
		USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

		USR_NEXTION_Enviar_Trama(usr_nextion_texto_programa_retardo, (uint16_t)strlen((const char *)usr_nextion_texto_programa_retardo));

	}
	else
	{
		//Se ajusta el boton de ejecutar, el de programa
		sprintf((char *)usr_nextion_comando_numero,"bt14.val=1");
		USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

		sprintf((char *)usr_nextion_comando_numero,"bt14.txt=\"PAUSAR\"");
		USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

		sprintf((char *)usr_nextion_comando_numero,"ejec.val=1");
		USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));

		if(USR_FLASH_Obtener_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_ESTADO_ACTUAL) == USR_PROCESADOR_ESTADO_ROTANDO)
		{
			USR_PROCESADOR_Cambiar_Posicion();
		}

		USR_NEXTION_Enviar_Trama(usr_nextion_texto_programa_ejecutando, (uint16_t)strlen((const char *)usr_nextion_texto_programa_ejecutando));

	}

}


void USR_NEXTION_Temperatura(void)
{
	uint8_t termocupla_actual;

	termocupla_actual = USR_NEXTION_Obtener_Dato_Trama((uint32_t)USR_NEXTION_TEMPERATURA_POSICION_TERMOCUPLA);

	arreglo_termocuplas[termocupla_actual].temperatura_configurada = USR_NEXTION_Obtener_Dato_Trama((uint32_t)USR_NEXTION_TEMPERATURA_POSICION_VALOR);

	if(arreglo_termocuplas[termocupla_actual].estado_termocupla == USR_TEMPERATURA_ESTADO_TERMOCUPLA_OK)
	{
		arreglo_termocuplas[termocupla_actual].estado_control = USR_NEXTION_Obtener_Dato_Trama((uint32_t)USR_NEXTION_TEMPERATURA_POSICION_CONTROL);

		if(arreglo_termocuplas[termocupla_actual].estado_control == USR_TEMPERATURA_ESTADO_CONTROL_ACTIV0)
		{
			USR_TEMPERATURA_Control();

		}
		if(arreglo_termocuplas[termocupla_actual].estado_control == USR_TEMPERATURA_ESTADO_CONTROL_INACTIV0)
		{
			USR_TEMPERATURA_Desactivar_Termocupla(termocupla_actual);

		}

		//USR_TEMPERATURA_ESTADO_RELE();

	}

	switch(termocupla_actual)
	{
		case USR_TEMPERATURA_TERMOCUPLA_1:
			USR_FLASH_Cambiar_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_T1_VALOR, arreglo_termocuplas[termocupla_actual].temperatura_configurada);
			USR_FLASH_Cambiar_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_T1_ESTADO, arreglo_termocuplas[termocupla_actual].estado_control);
			break;

		case USR_TEMPERATURA_TERMOCUPLA_2:
			USR_FLASH_Cambiar_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_T2_VALOR, arreglo_termocuplas[termocupla_actual].temperatura_configurada);
			USR_FLASH_Cambiar_Dato_Trama_Estado_Actual(USR_PROCESADOR_POSICION_T2_ESTADO, arreglo_termocuplas[termocupla_actual].estado_control);
			break;

	}

	//Despues de hacer las modificaciones a la trama se guarda...
	USR_FLASH_Guardar_Estado_Actual();

}


void USR_NEXTION_Verificar_Temperatura(void)
{
	if(USR_TEMPERATURA_Revisar_Estado() != USR_TEMPERATURA_ESTADO_TERMOCUPLA_OK)
	{
		if(usr_nextion_pantalla_actual == USR_NEXTION_PANTALLA_MENU)
		{
			USR_NEXTION_Enviar_Trama(usr_nextion_texto_menu_error_termocuplas, strlen((char *)usr_nextion_texto_menu_error_termocuplas));
		}
	}
	else
	{
		if(usr_nextion_pantalla_actual == USR_NEXTION_PANTALLA_MENU)
		{
			USR_NEXTION_Enviar_Trama(usr_nextion_texto_menu_ok, strlen((char *)usr_nextion_texto_menu_ok));
		}
	}

}


void USR_NEXTION_Verificar_RTC(void)
{
	if(usr_rtc_estado_actual != USR_RTC_ESTADO_OK)
	{
		if(usr_nextion_pantalla_actual == USR_NEXTION_PANTALLA_MENU)
		{
			USR_NEXTION_Enviar_Trama(usr_nextion_texto_menu_error_rtc, strlen((char *)usr_nextion_texto_menu_error_rtc));
		}
	}

}


void USR_NEXTION_Activar_Recipiente(uint8_t recipiente)
{

	uint8_t numero_elemento = 1;

	while(numero_elemento < USR_PROCESADOR_NUMERO_ELEMENTOS+1)
	{
		if(numero_elemento == recipiente)
		{
			sprintf((char *)usr_nextion_comando_numero,"vis q%d,1",(int)(numero_elemento));
			USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));
		}
		else
		{
			sprintf((char *)usr_nextion_comando_numero,"vis q%d,0",(int)(numero_elemento));
			USR_NEXTION_Enviar_Trama(usr_nextion_comando_numero, (uint16_t)strlen((const char *)usr_nextion_comando_numero));
		}
		numero_elemento++;
	}

}


HAL_StatusTypeDef USR_NEXTION_Cambiar_Pantalla(uint32_t pantalla)
{

	usr_nextion_pantalla_actual = pantalla;

	switch(usr_nextion_pantalla_actual)
	{

		case USR_NEXTION_PANTALLA_INICIO:
			usr_nextion_status = USR_NEXTION_Enviar_Trama(usr_nextion_com_pantalla_inicio, (uint16_t)strlen((const char *)usr_nextion_com_pantalla_inicio));
			break;

		case USR_NEXTION_PANTALLA_MENU:
			usr_nextion_status = USR_NEXTION_Enviar_Trama(usr_nextion_com_pantalla_menu, (uint16_t)strlen((const char *)usr_nextion_com_pantalla_menu));
			USR_PROCESADOR_ReleAC(USR_PROCESADOR_RELEAC_INACTIVO);
			USR_NEXTION_Verificar_RTC();
			USR_NEXTION_Visualizar_RTC();
			USR_NEXTION_Verificar_Temperatura();
			break;

		case USR_NEXTION_PANTALLA_EJECUTAR:
			usr_nextion_status = USR_NEXTION_Enviar_Trama(usr_nextion_com_pantalla_ejecutar, (uint16_t)strlen((const char *)usr_nextion_com_pantalla_ejecutar));
			USR_NEXTION_Visualizar_RTC();
			USR_NEXTION_Mostrar_Programa(1);
			break;

		case USR_NEXTION_PANTALLA_MANUAL:
			usr_nextion_status = USR_NEXTION_Enviar_Trama(usr_nextion_com_pantalla_manual, (uint16_t)strlen((const char *)usr_nextion_com_pantalla_manual));
			USR_NEXTION_Visualizar_RTC();
			USR_NEXTION_Enviar_Trama(usr_nextion_texto_canasta_abajo, (uint16_t)strlen((const char *)usr_nextion_texto_canasta_abajo));
			USR_PROCESADOR_ReleAC(USR_PROCESADOR_RELEAC_ACTIVO);
			break;

		case USR_NEXTION_PANTALLA_CONFIGURAR:
			usr_nextion_status = USR_NEXTION_Enviar_Trama(usr_nextion_com_pantalla_configurar, (uint16_t)strlen((const char *)usr_nextion_com_pantalla_configurar));
			USR_NEXTION_Visualizar_RTC();
			USR_NEXTION_Mostrar_Programa(1);
			break;

		case USR_NEXTION_PANTALLA_CALENDARIO:
			usr_nextion_status = USR_NEXTION_Enviar_Trama(usr_nextion_com_pantalla_calendario, (uint16_t)strlen((const char *)usr_nextion_com_pantalla_calendario));
			USR_NEXTION_Visualizar_RTC();
			break;

		case USR_NEXTION_PANTALLA_RELOJ:
			usr_nextion_status = USR_NEXTION_Enviar_Trama(usr_nextion_com_pantalla_reloj, (uint16_t)strlen((const char *)usr_nextion_com_pantalla_reloj));
			break;

		case USR_NEXTION_PANTALLA_ALERTA:
			usr_nextion_status = USR_NEXTION_Enviar_Trama(usr_nextion_com_pantalla_alerta, (uint16_t)strlen((const char *)usr_nextion_com_pantalla_alerta));
			USR_NEXTION_Visualizar_Alerta();
			break;

		case USR_NEXTION_PANTALLA_TERMOCUPLAS:
			usr_nextion_status = USR_NEXTION_Enviar_Trama(usr_nextion_com_pantalla_termocuplas, (uint16_t)strlen((const char *)usr_nextion_com_pantalla_termocuplas));
			USR_NEXTION_Visualizar_Termocuplas();
			break;

	}

	return usr_nextion_status;
}


void USR_NEXTION_Ejecutar_Comando(void)
{
	uint32_t comando = USR_NEXTION_Obtener_Dato_Trama((uint32_t)USR_NEXTION_POSICION_COMANDO);

	USR_PROCESADOR_Buzzer();

	switch(comando)
	{

		case USR_NEXTION_COMANDO_PANTALLA:

			USR_NEXTION_Cambiar_Pantalla(USR_NEXTION_Obtener_Dato_Trama((uint32_t)USR_NEXTION_POSICION_PANTALLA));

			break;

		case USR_NEXTION_COMANDO_GUARDAR_RTC:

			USR_NEXTION_Guardar_RTC();

			break;

		case USR_NEXTION_COMANDO_GUARDAR_PROGRAMA:

			usr_nextion_status = USR_FLASH_Guardar_Programa(USR_NEXTION_Obtener_Dato_Trama((uint32_t)USR_PROCESADOR_POSICION_PROGRAMA));
			if (usr_nextion_status == HAL_OK)
			{
				USR_NEXTION_Enviar_Trama(usr_nextion_texto_memoria_ok,  (uint16_t)strlen((const char *)usr_nextion_texto_memoria_ok));
			}
			else
			{
				USR_NEXTION_Enviar_Trama(usr_nextion_texto_memoria_error,  (uint16_t)strlen((const char *)usr_nextion_texto_memoria_error));
			}

			break;

		case USR_NEXTION_COMANDO_REINICIAR_MEMORIA:

			usr_nextion_status = USR_FLASH_Reiniciar_Programas();
			if (usr_nextion_status == HAL_OK)
			{
				USR_NEXTION_Enviar_Trama(usr_nextion_texto_memoria_rst_ok,  (uint16_t)strlen((const char *)usr_nextion_texto_memoria_rst_ok));
			}
			else
			{
				USR_NEXTION_Enviar_Trama(usr_nextion_texto_memoria_rst_error,  (uint16_t)strlen((const char *)usr_nextion_texto_memoria_rst_error));
			}

			break;


		case USR_NEXTION_COMANDO_PROGRAMA:

			USR_NEXTION_Mostrar_Programa((uint8_t)USR_NEXTION_Obtener_Dato_Trama((uint32_t)USR_PROCESADOR_POSICION_PROGRAMA));

			break;


		case USR_NEXTION_COMANDO_EJECUTAR:

			USR_PROCESADOR_Ejecutar_Programa();

			break;

		case USR_NEXTION_COMANDO_DETENER:

			USR_PROCESADOR_Detener_Programa();

			break;

		case USR_NEXTION_COMANDO_PAUSAR:

			USR_PROCESADOR_Pausar_Programa();

			break;

		case USR_NEXTION_COMANDO_MANUAL:

			if( USR_NEXTION_Obtener_Dato_Trama((uint32_t)USR_PROCESADOR_POSICION_PROGRAMA) == USR_PROCESADOR_MANUAL_ELEVAR_CANASTA)
			{
				USR_PROCESADOR_Elevar_Canasta();
			}
			else
			{
				USR_PROCESADOR_Bajar_Canasta();
			}

			break;


		case USR_NEXTION_COMANDO_BUZZER:

			break;

		case USR_NEXTION_COMANDO_TEMPERATURA:

			USR_NEXTION_Temperatura();

			break;

		case USR_NEXTION_COMANDO_GUARDAR_TERMOCUPLAS:

			usr_nextion_status = USR_FLASH_Guardar_Termocuplas();
			if (usr_nextion_status == HAL_OK)
			{
				USR_NEXTION_Enviar_Trama(usr_nextion_texto_termocuplas_ok,  (uint16_t)strlen((const char *)usr_nextion_texto_memoria_ok));
				HAL_Delay(10);
				USR_NEXTION_Visualizar_Termocuplas();
			}
			else
			{
				USR_NEXTION_Enviar_Trama(usr_nextion_texto_termocuplas_error,  (uint16_t)strlen((const char *)usr_nextion_texto_termocuplas_error));
			}
			HAL_Delay(10);

			//Se reinician los chips para actualizar los cambios...
			USR_TEMPERATURA_Iniciar_Chips();

			break;
	}

	USR_NEXTION_Habilitar_Touch_Pantalla();

}

void USR_NEXTION_Administrador(void)
{

	if(usr_nextion_estado_trama == USR_NEXTION_TRAMA_RECIBIDA)
	{

		USR_NEXTION_Ejecutar_Comando();

		usr_nextion_estado_trama = USR_NEXTION_TRAMA_NO_RECIBIDA;
		USR_NEXTION_Iniciar_Recibir_Trama();

	}

}
