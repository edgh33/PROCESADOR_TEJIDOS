/*
 * USR_FLASH.c
 *
 *  Created on: 1/09/2018
 *      Author: edgh3
 */

#include "USR_FLASH.h"


uint8_t  usr_flash_buffer_programa[USR_FLASH_BUFFER_PROGRAMA_TAM_8B];
uint8_t  usr_flash_buffer_estado_actual[USR_FLASH_BUFFER_ESTADO_ACTUAL_TAM_8B];
uint32_t buffer_programas[USR_FLASH_BUFFER_PROGRAMA_TAM_32B*USR_FLASH_CANTIDAD_PROGRAMAS];
uint32_t PageError = 0;

HAL_StatusTypeDef USR_FLASH_Guardar_Programa(uint8_t numProg)
{
	uint32_t contador = 0, contador_32b = 0;
	uint32_t direccion = 0;
	uint32_t valTemp = 0;
	/*Variable used for Erase procedure*/
	static FLASH_EraseInitTypeDef EraseInitStruct;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Unlock the Options Bytes *************************************************/
	HAL_FLASH_OB_Unlock();

    contador = 0;
    direccion = USR_FLASH_PAGINA_PROGRAMAS;

    //Se guarda en RAM los registros de memoria de la flash
	while (contador < USR_FLASH_BUFFER_PROGRAMA_TAM_32B*USR_FLASH_CANTIDAD_PROGRAMAS)
	{

		buffer_programas[contador] = (*(__IO uint32_t*) direccion);
		contador++;
		direccion+=4;

	}

	//Se sobreescribe el nuevo programa en la posicion correspondiente
	contador = 0;
	contador_32b = 0;

	while (contador_32b < USR_FLASH_BUFFER_PROGRAMA_TAM_32B)
	{
		valTemp = ((uint32_t)(usr_nextion_buffer_rx[contador]<<24)&0xFF000000)| ((uint32_t)(usr_nextion_buffer_rx[contador+1]<<16)&0x00FF0000)|((uint32_t)(usr_nextion_buffer_rx[contador+2]<<8)&0x0000FF00)|((uint32_t)(usr_nextion_buffer_rx[contador+3])&0x000000FF);
		buffer_programas[((numProg-1)*USR_FLASH_BUFFER_PROGRAMA_TAM_32B)+contador_32b] = valTemp;
		contador_32b++;
		contador+=4;
	}


	//Se borra la pagina para poder escribirla
    /* Fill EraseInit structure************************************************/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = USR_FLASH_PAGINA_PROGRAMAS;
    EraseInitStruct.NbPages     = 1;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
    	HAL_FLASH_Lock();
    	return HAL_ERROR;
    }

    //Se escribe todo el buffer con los nuevos datos
    contador = 0;
    direccion = USR_FLASH_PAGINA_PROGRAMAS;
	while (contador < USR_FLASH_BUFFER_PROGRAMA_TAM_32B*USR_FLASH_CANTIDAD_PROGRAMAS)
	{

		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, direccion, buffer_programas[contador]) == HAL_OK)
		{
			direccion += 4;
			contador++;
		}
		else
		{
			/* Error occurred while writing data in Flash memory.
			   User can add here some code to deal with this error */
			HAL_FLASH_Lock();
			return HAL_ERROR;
		}
	}

    /* Check the correctness of written data */
    contador = 0;
    direccion = USR_FLASH_PAGINA_PROGRAMAS;

	while (contador < USR_FLASH_BUFFER_PROGRAMA_TAM_32B*USR_FLASH_CANTIDAD_PROGRAMAS)
	{

		if((*(__IO uint32_t*) direccion) != buffer_programas[contador])
		{
			/* Error occurred while writing data in Flash memory.
			User can add here some code to deal with this error */
			HAL_FLASH_Lock();
			return HAL_ERROR;
		}
		direccion += 4;
		contador++;
	}

	HAL_FLASH_Lock();
	return HAL_OK;

}


HAL_StatusTypeDef USR_FLASH_Guardar_Termocuplas(void)
{
	uint32_t contador = 0, contador_32b = 0;
	uint32_t direccion = 0;
	uint32_t valTemp = 0;
	/*Variable used for Erase procedure*/
	static FLASH_EraseInitTypeDef EraseInitStruct;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Unlock the Options Bytes *************************************************/
	HAL_FLASH_OB_Unlock();

    contador = 0;
    direccion = USR_FLASH_PAGINA_TERMOCUPLAS;

	//Se sobreescribe la nueva configuracion de las termocuplas
	contador = 0;
	contador_32b = 0;

	while (contador_32b < USR_FLASH_BUFFER_PROGRAMA_TAM_32B)
	{
		valTemp = ((uint32_t)(usr_nextion_buffer_rx[contador]<<24)&0xFF000000)| ((uint32_t)(usr_nextion_buffer_rx[contador+1]<<16)&0x00FF0000)|((uint32_t)(usr_nextion_buffer_rx[contador+2]<<8)&0x0000FF00)|((uint32_t)(usr_nextion_buffer_rx[contador+3])&0x000000FF);
		buffer_programas[contador_32b] = valTemp;
		contador_32b++;
		contador+=4;
	}

	//Se borra la pagina para poder escribirla
    /* Fill EraseInit structure************************************************/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = USR_FLASH_PAGINA_TERMOCUPLAS;
    EraseInitStruct.NbPages     = 1;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
    	HAL_FLASH_Lock();
    	return HAL_ERROR;
    }

    //Se escribe todo el buffer con los nuevos datos
    contador = 0;
    direccion = USR_FLASH_PAGINA_TERMOCUPLAS;

	while (contador < USR_FLASH_BUFFER_PROGRAMA_TAM_32B)
	{

		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, direccion, buffer_programas[contador]) == HAL_OK)
		{
			direccion += 4;
			contador++;
		}
		else
		{
			/* Error occurred while writing data in Flash memory.
			   User can add here some code to deal with this error */
			HAL_FLASH_Lock();
			return HAL_ERROR;
		}
	}

    /* Check the correctness of written data */
    contador = 0;
    direccion = USR_FLASH_PAGINA_TERMOCUPLAS;

	while (contador < USR_FLASH_BUFFER_PROGRAMA_TAM_32B)
	{

		if((*(__IO uint32_t*) direccion) != buffer_programas[contador])
		{
			/* Error occurred while writing data in Flash memory.
			User can add here some code to deal with this error */
			HAL_FLASH_Lock();
			return HAL_ERROR;
		}
		direccion += 4;
		contador++;
	}

	HAL_FLASH_Lock();
	return HAL_OK;

}


HAL_StatusTypeDef USR_FLASH_Guardar_Estado_Actual(void)
{
	uint32_t contador = 0, contador_32b = 0;
	uint32_t direccion = 0;
	uint32_t valTemp = 0;
	/*Variable used for Erase procedure*/
	static FLASH_EraseInitTypeDef EraseInitStruct;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Unlock the Options Bytes *************************************************/
	HAL_FLASH_OB_Unlock();

    contador = 0;
    direccion = USR_FLASH_PAGINA_ESTADO_ACTUAL;

	//Se sobreescribe la nueva configuracion de las termocuplas
	contador = 0;
	contador_32b = 0;

	while (contador_32b < USR_FLASH_BUFFER_ESTADO_ACTUAL_TAM_32B)
	{
		valTemp = ((uint32_t)(usr_flash_buffer_estado_actual[contador]<<24)&0xFF000000)| ((uint32_t)(usr_flash_buffer_estado_actual[contador+1]<<16)&0x00FF0000)|((uint32_t)(usr_flash_buffer_estado_actual[contador+2]<<8)&0x0000FF00)|((uint32_t)(usr_flash_buffer_estado_actual[contador+3])&0x000000FF);
		buffer_programas[contador_32b] = valTemp;
		contador_32b++;
		contador+=4;
	}

	//Se borra la pagina para poder escribirla
    /* Fill EraseInit structure************************************************/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = USR_FLASH_PAGINA_ESTADO_ACTUAL;
    EraseInitStruct.NbPages     = 1;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
    	HAL_FLASH_Lock();
    	return HAL_ERROR;
    }

    //Se escribe todo el buffer con los nuevos datos
    contador = 0;
    direccion = USR_FLASH_PAGINA_ESTADO_ACTUAL;

	while (contador < USR_FLASH_BUFFER_ESTADO_ACTUAL_TAM_32B)
	{

		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, direccion, buffer_programas[contador]) == HAL_OK)
		{
			direccion += 4;
			contador++;
		}
		else
		{
			/* Error occurred while writing data in Flash memory.
			   User can add here some code to deal with this error */
			HAL_FLASH_Lock();
			return HAL_ERROR;
		}
	}

    /* Check the correctness of written data */
    contador = 0;
    direccion = USR_FLASH_PAGINA_ESTADO_ACTUAL;

	while (contador < USR_FLASH_BUFFER_ESTADO_ACTUAL_TAM_32B)
	{

		if((*(__IO uint32_t*) direccion) != buffer_programas[contador])
		{
			/* Error occurred while writing data in Flash memory.
			User can add here some code to deal with this error */
			HAL_FLASH_Lock();
			return HAL_ERROR;
		}
		direccion += 4;
		contador++;
	}

	HAL_FLASH_Lock();
	return HAL_OK;

}



void USR_FLASH_Leer_Programa(uint8_t numProg)
{

	uint8_t contador = 0;
	uint32_t direccion = 0;
	uint32_t valTemp = 0;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Unlock the Options Bytes *************************************************/
	HAL_FLASH_OB_Unlock();


    contador = 0;
    direccion = USR_FLASH_PAGINA_PROGRAMAS+((numProg-1)*USR_FLASH_BUFFER_PROGRAMA_TAM_8B);

	while (contador < USR_FLASH_BUFFER_PROGRAMA_TAM_8B)
	{

		valTemp = (*(__IO uint32_t*) direccion);
		usr_flash_buffer_programa[contador]= (valTemp>>24)&(0x000000FF);
		usr_flash_buffer_programa[contador+1]= (valTemp>>16)&(0x000000FF);
		usr_flash_buffer_programa[contador+2]= (valTemp>>8)&(0x000000FF);
		usr_flash_buffer_programa[contador+3]= (valTemp)&(0x000000FF);

		direccion += 4;
		contador += 4;
	}

	HAL_FLASH_Lock();

}

void USR_FLASH_Leer_Termocuplas(void)
{

	uint8_t contador = 0;
	uint32_t direccion = 0;
	uint32_t valTemp = 0;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Unlock the Options Bytes *************************************************/
	HAL_FLASH_OB_Unlock();


    contador = 0;
    direccion = USR_FLASH_PAGINA_TERMOCUPLAS;

	while (contador < USR_FLASH_BUFFER_PROGRAMA_TAM_8B)
	{

		valTemp = (*(__IO uint32_t*) direccion);
		usr_flash_buffer_programa[contador]= (valTemp>>24)&(0x000000FF);
		usr_flash_buffer_programa[contador+1]= (valTemp>>16)&(0x000000FF);
		usr_flash_buffer_programa[contador+2]= (valTemp>>8)&(0x000000FF);
		usr_flash_buffer_programa[contador+3]= (valTemp)&(0x000000FF);

		direccion += 4;
		contador += 4;
	}

	HAL_FLASH_Lock();

}

void USR_FLASH_Leer_Estado_Actual(void)
{

	uint8_t contador = 0;
	uint32_t direccion = 0;
	uint32_t valTemp = 0;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Unlock the Options Bytes *************************************************/
	HAL_FLASH_OB_Unlock();


    contador = 0;
    direccion = USR_FLASH_PAGINA_ESTADO_ACTUAL;

	while (contador < USR_FLASH_BUFFER_ESTADO_ACTUAL_TAM_8B)
	{

		valTemp = (*(__IO uint32_t*) direccion);
		usr_flash_buffer_estado_actual[contador]= (valTemp>>24)&(0x000000FF);
		usr_flash_buffer_estado_actual[contador+1]= (valTemp>>16)&(0x000000FF);
		usr_flash_buffer_estado_actual[contador+2]= (valTemp>>8)&(0x000000FF);
		usr_flash_buffer_estado_actual[contador+3]= (valTemp)&(0x000000FF);

		direccion += 4;
		contador += 4;
	}

	HAL_FLASH_Lock();

}

HAL_StatusTypeDef USR_FLASH_Reiniciar_Programas(void)
{
	uint32_t contador = 0, contador_32b = 0, contador_programas = 0, indexador = 0;
	uint32_t direccion = 0;
	uint32_t valTemp = 0;


	/*Variable used for Erase procedure*/
	static FLASH_EraseInitTypeDef EraseInitStruct;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Unlock the Options Bytes *************************************************/
	HAL_FLASH_OB_Unlock();

	//Se borra la pagina para poder escribirla
    /* Fill EraseInit structure************************************************/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = USR_FLASH_PAGINA_PROGRAMAS;
    EraseInitStruct.NbPages     = 1;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
    	HAL_FLASH_Lock();
    	return HAL_ERROR;
    }

    //Se escribe todo el buffer con los nuevos datos
    for(contador_programas = 1; contador_programas < USR_FLASH_CANTIDAD_PROGRAMAS+1; contador_programas++)
    {
    	contador = 0;
    	contador_32b = 0;
    	direccion = USR_FLASH_PAGINA_PROGRAMAS+(USR_FLASH_BUFFER_PROGRAMA_TAM_8B*(contador_programas-1));

    	sprintf((char *)usr_flash_buffer_programa,"7:%d:1:0:1:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0",(int)contador_programas);

    	while (contador < USR_FLASH_BUFFER_PROGRAMA_TAM_8B)
    	{
    		valTemp = ((uint32_t)(usr_flash_buffer_programa[contador]<<24)&0xFF000000)| ((uint32_t)(usr_flash_buffer_programa[contador+1]<<16)&0x00FF0000)|((uint32_t)(usr_flash_buffer_programa[contador+2]<<8)&0x0000FF00)|((uint32_t)(usr_flash_buffer_programa[contador+3])&0x000000FF);
    		indexador = ((contador_programas-1)*USR_FLASH_BUFFER_PROGRAMA_TAM_32B)+contador_32b;
    		buffer_programas[indexador] = valTemp;
    		contador_32b++;
    		contador+=4;
    	}

    }

    contador = 0;
    direccion = USR_FLASH_PAGINA_PROGRAMAS;

	while (contador < USR_FLASH_BUFFER_PROGRAMA_TAM_32B*USR_FLASH_CANTIDAD_PROGRAMAS)
	{

		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, direccion, buffer_programas[contador]) == HAL_OK)
		{
			direccion += 4;
			contador++;
		}
		else
		{
			/* Error occurred while writing data in Flash memory.
			   User can add here some code to deal with this error */
			HAL_FLASH_Lock();
			return HAL_ERROR;
		}
	}

    HAL_FLASH_Lock();
	return HAL_OK;

}

HAL_StatusTypeDef USR_FLASH_Reiniciar_Termocuplas(void)
{
	uint32_t contador = 0, contador_32b = 0;
	uint32_t direccion = 0;
	uint32_t valTemp = 0;


	/*Variable used for Erase procedure*/
	static FLASH_EraseInitTypeDef EraseInitStruct;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Unlock the Options Bytes *************************************************/
	HAL_FLASH_OB_Unlock();

	//Se borra la pagina para poder escribirla
    /* Fill EraseInit structure************************************************/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = USR_FLASH_PAGINA_TERMOCUPLAS;
    EraseInitStruct.NbPages     = 1;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
    	HAL_FLASH_Lock();
    	return HAL_ERROR;
    }

	contador = 0;
	contador_32b = 0;
	direccion = USR_FLASH_PAGINA_TERMOCUPLAS;

	sprintf((char *)usr_flash_buffer_programa,"11:2:3:3:0:0:0:0");

	while (contador < USR_FLASH_BUFFER_PROGRAMA_TAM_8B)
	{
		valTemp = ((uint32_t)(usr_flash_buffer_programa[contador]<<24)&0xFF000000)| ((uint32_t)(usr_flash_buffer_programa[contador+1]<<16)&0x00FF0000)|((uint32_t)(usr_flash_buffer_programa[contador+2]<<8)&0x0000FF00)|((uint32_t)(usr_flash_buffer_programa[contador+3])&0x000000FF);
		buffer_programas[contador_32b] = valTemp;
		contador_32b++;
		contador+=4;
	}

    contador = 0;
    direccion = USR_FLASH_PAGINA_TERMOCUPLAS;

	while (contador < USR_FLASH_BUFFER_PROGRAMA_TAM_32B)
	{

		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, direccion, buffer_programas[contador]) == HAL_OK)
		{
			direccion += 4;
			contador++;
		}
		else
		{
			/* Error occurred while writing data in Flash memory.
			   User can add here some code to deal with this error */
			HAL_FLASH_Lock();
			return HAL_ERROR;
		}
	}

    HAL_FLASH_Lock();
	return HAL_OK;

}

HAL_StatusTypeDef USR_FLASH_Reiniciar_Estado_Actual(void)
{
	uint32_t contador = 0, contador_32b = 0;
	uint32_t direccion = 0;
	uint32_t valTemp = 0;


	/*Variable used for Erase procedure*/
	static FLASH_EraseInitTypeDef EraseInitStruct;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Unlock the Options Bytes *************************************************/
	HAL_FLASH_OB_Unlock();

	//Se borra la pagina para poder escribirla
    /* Fill EraseInit structure************************************************/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = USR_FLASH_PAGINA_ESTADO_ACTUAL;
    EraseInitStruct.NbPages     = 1;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
    	HAL_FLASH_Lock();
    	return HAL_ERROR;
    }

	contador = 0;
	contador_32b = 0;
	direccion = USR_FLASH_PAGINA_ESTADO_ACTUAL;

	//ORIGINAL PARA REINICIAR!!!!!
	sprintf((char *)usr_flash_buffer_estado_actual,"7:1:1:0:1:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:60:0:60:0:0");

	//PARA PRUEBA DE ESTADO GUARDADO...
	//sprintf((char *)usr_flash_buffer_estado_actual,"7:4:7:0:0:0:0:0:1:0:2:0:1:0:2:0:1:0:0:0:0:0:0:0:0:0:0:0:0:1:65:1:70:2:1");

	while (contador < USR_FLASH_BUFFER_ESTADO_ACTUAL_TAM_8B)
	{
		valTemp = ((uint32_t)(usr_flash_buffer_estado_actual[contador]<<24)&0xFF000000)| ((uint32_t)(usr_flash_buffer_estado_actual[contador+1]<<16)&0x00FF0000)|((uint32_t)(usr_flash_buffer_estado_actual[contador+2]<<8)&0x0000FF00)|((uint32_t)(usr_flash_buffer_estado_actual[contador+3])&0x000000FF);
		buffer_programas[contador_32b] = valTemp;
		contador_32b++;
		contador+=4;
	}

    contador = 0;
    direccion = USR_FLASH_PAGINA_ESTADO_ACTUAL;

	while (contador < USR_FLASH_BUFFER_ESTADO_ACTUAL_TAM_32B)
	{

		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, direccion, buffer_programas[contador]) == HAL_OK)
		{
			direccion += 4;
			contador++;
		}
		else
		{
			/* Error occurred while writing data in Flash memory.
			   User can add here some code to deal with this error */
			HAL_FLASH_Lock();
			return HAL_ERROR;
		}
	}

    HAL_FLASH_Lock();
	return HAL_OK;

}

uint32_t USR_FLASH_Obtener_Dato_Trama(uint32_t posicion)
{
	char *trama_dato;
	trama_dato = (char *) malloc(10);
	char *trama_dato_temp = trama_dato;
	uint8_t *usr_flash_buffer_programa_temp = usr_flash_buffer_programa;
	uint32_t valor_dato = 0;
	uint32_t contador_pos = 0;

	while(contador_pos < posicion)
	{
		if(*usr_flash_buffer_programa_temp == ':')
		{
			contador_pos++;
		}
		usr_flash_buffer_programa_temp++;
	}

	while((*usr_flash_buffer_programa_temp != ':') && (*usr_flash_buffer_programa_temp != 0))
	{
		*trama_dato_temp = *usr_flash_buffer_programa_temp;
		trama_dato_temp++;
		usr_flash_buffer_programa_temp++;
	}

	*trama_dato_temp = 0;

	valor_dato = atoi(trama_dato);

	free(trama_dato);

	return valor_dato;

}

uint32_t USR_FLASH_Obtener_Dato_Trama_Estado_Actual(uint32_t posicion)
{
	char *trama_dato;
	trama_dato = (char *) malloc(10);
	char *trama_dato_temp = trama_dato;
	uint8_t *usr_flash_buffer_estado_actual_temp = usr_flash_buffer_estado_actual;
	uint32_t valor_dato = 0;
	uint32_t contador_pos = 0;

	while(contador_pos < posicion)
	{
		if(*usr_flash_buffer_estado_actual_temp == ':')
		{
			contador_pos++;
		}
		usr_flash_buffer_estado_actual_temp++;
	}

	while((*usr_flash_buffer_estado_actual_temp != ':') && (*usr_flash_buffer_estado_actual_temp != 0))
	{
		*trama_dato_temp = *usr_flash_buffer_estado_actual_temp;
		trama_dato_temp++;
		usr_flash_buffer_estado_actual_temp++;
	}

	*trama_dato_temp = 0;

	valor_dato = atoi(trama_dato);

	free(trama_dato);

	return valor_dato;

}


void USR_FLASH_Cambiar_Dato_Trama_Estado_Actual(uint32_t posicion, uint32_t dato)
{

	uint8_t usr_flash_buffer_estado_actual_temp[USR_FLASH_BUFFER_ESTADO_ACTUAL_TAM_8B];
	char trama_dato[4];

	uint32_t contador_general = 0;
	uint32_t contador_temporal = 0;
	uint32_t contador_pos = 0;

	//Se convierte el dato a string
	sprintf((char *)trama_dato, "%d:", (int)dato);

	//Se copia el arreglo hasta una posicion antes de donde se va a copiar
	contador_general = 0;

	while(contador_pos < posicion)
	{
		if(usr_flash_buffer_estado_actual[contador_general] == ':')
		{
			contador_pos++;
		}
		usr_flash_buffer_estado_actual_temp[contador_general] = usr_flash_buffer_estado_actual[contador_general];
		contador_general++;
	}

	//Se agrega un 0 en la ultima posicion para poder hacer una concatenacion de strings
	usr_flash_buffer_estado_actual_temp[contador_general] = 0;

	//Se concatena el nuevo valor a la trama
	strcat((char *)usr_flash_buffer_estado_actual_temp, (const char *)trama_dato);

	//el contador temporal sirve para
	contador_temporal = contador_general + strlen(trama_dato);

	while(usr_flash_buffer_estado_actual[contador_general] != ':')
	{
		contador_general++;
	}

	contador_general++;

	while(contador_general < USR_FLASH_BUFFER_ESTADO_ACTUAL_TAM_8B)
	{
		usr_flash_buffer_estado_actual_temp[contador_temporal] = usr_flash_buffer_estado_actual[contador_general];
		contador_temporal++;
		contador_general++;
	}

	//Se escribe la nueva trama en el buffer de trabajo
	contador_general = 0;
	while(contador_general < USR_FLASH_BUFFER_ESTADO_ACTUAL_TAM_8B)
	{
		usr_flash_buffer_estado_actual[contador_general] = usr_flash_buffer_estado_actual_temp[contador_general];

		contador_general++;
	}

}

void USR_FLASH_Iniciar_Trama_Estado_Actual(void)
{
	sprintf((char *)usr_flash_buffer_estado_actual,"7:1:1:0:1:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:60:0:60:0:0");
}

void USR_FLASH_Crear_Trama_Estado_Actual(void)
{
	char trama_dato[10];
	uint32_t contador_general = 0;

	contador_general = 0;

	//Se copia todo el programa actual
	while(usr_flash_buffer_programa[contador_general] != 0)
	{
		usr_flash_buffer_estado_actual[contador_general] = usr_flash_buffer_programa[contador_general];

		contador_general++;
	}

	//Se agregan todos los datos adicionales separados por ':'
	usr_flash_buffer_estado_actual[contador_general] = ':';
	contador_general++;
	usr_flash_buffer_estado_actual[contador_general] = 0;

	//Se agrega el dato de las horas de delay actual
	sprintf((char *)trama_dato, "%d:", (int)usr_procesador_delay_hora_actual);
	strcat((char *)usr_flash_buffer_estado_actual, (const char *)trama_dato);

	//Se agrega el dato de los minutos de delay actual
	sprintf((char *)trama_dato, "%d:", (int)usr_procesador_delay_minuto_actual);
	strcat((char *)usr_flash_buffer_estado_actual, (const char *)trama_dato);

	//Se agrega el dato del estado de t1
	sprintf((char *)trama_dato, "%d:", (int)arreglo_termocuplas[0].estado_control);
	strcat((char *)usr_flash_buffer_estado_actual, (const char *)trama_dato);

	//Se agrega el dato de la temperatura configurada de t1
	sprintf((char *)trama_dato, "%d:", (int)arreglo_termocuplas[0].temperatura_configurada);
	strcat((char *)usr_flash_buffer_estado_actual, (const char *)trama_dato);

	//Se agrega el dato del estado de t2
	sprintf((char *)trama_dato, "%d:", (int)arreglo_termocuplas[1].estado_control);
	strcat((char *)usr_flash_buffer_estado_actual, (const char *)trama_dato);

	//Se agrega el dato de la temperatura configurada de t2
	sprintf((char *)trama_dato, "%d:", (int)arreglo_termocuplas[1].temperatura_configurada);
	strcat((char *)usr_flash_buffer_estado_actual, (const char *)trama_dato);

	//Se agrega la posicion actual
	sprintf((char *)trama_dato, "%d:", (int)usr_procesador_recipiente_actual);
	strcat((char *)usr_flash_buffer_estado_actual, (const char *)trama_dato);

	//Se agrega el estado de ejecucion actual
	sprintf((char *)trama_dato, "%d", (int)USR_PROCESADOR_ESTADO_EJECUTANDO);
	strcat((char *)usr_flash_buffer_estado_actual, (const char *)trama_dato);


}
