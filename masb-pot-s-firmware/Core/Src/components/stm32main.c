/*
 * stm32main.c
 *
 *  Created on: 15 may. 2021
 *      Author: mariamor
 */

#include "components/stm32main.h"
#include "components/masb_comm_s.h"
#include "main.h"   // tenemos que incluir main.h para que nos reconozca el pin EN
#include "components/ad5280_driver.h"
#include "components/mcp4725_driver.h"
#include "components/i2c_lib.h"

struct CV_Configuration_S cvConfiguration;
struct CA_Configuration_S caConfiguration;
struct Data_S data;


void setup(struct Handles_S *handles){

	// INICIALIZACIÓN DE PMU:
	HAL_GPIO_WritePin(EN_GPIO_Port,EN_Pin,1); // PMU es PA5 que hemos definido como EN

	//=========================== Potenciometro ====================================
	// Solo debemos de ejecutar este codigo una unica vez al inicio del programa.
	// Fijaremos una resistencia fija para siempre y no la modificaremos.

	// Creamos el handle de la libreria.

	AD5280_Handle_T hpot = NULL;

	hpot = AD5280_Init();

	// Configuramos su direccion I2C de esclavo, su resistencia total (hay
	// diferentes modelos; este tiene 50kohms) e indicamos que funcion queremos que
	// se encargue de la escritura a traves del I2C. Utilizaremos la funcion
	// I2C_Write de la libreria i2c_lib.
	AD5280_ConfigSlaveAddress(hpot, 0x2C);
	AD5280_ConfigNominalResistorValue(hpot, 50e3f);
	AD5280_ConfigWriteFunction(hpot, I2C_Write);

	// Fijamos la resistencia de, por ejemplo, 12kohms.
	AD5280_SetWBResistance(hpot, 12e3f);


	//================================== DAC =======================================
	// Esto lo realizaremos una unica vez al inicio del programa. ------------------

	// Creamos el handle de la libreria.
	MCP4725_Handle_T hdac = NULL;

	hdac = MCP4725_Init();

	// Configuramos la direccion I2C de esclavo, su tension de referencia (que es la
	// de alimentacion) e indicamos que funcion queremos que se encargue de la
	// escritura a traves del I2C. Utilizaremos la funcion I2C_Write de la libreria
	// i2c_lib.
	MCP4725_ConfigSlaveAddress(hdac, 0x66);
	MCP4725_ConfigVoltageReference(hdac, 4.0f);
	MCP4725_ConfigWriteFunction(hdac, I2C_Write);
}

void loop(void){
	if (MASB_COMM_S_dataReceived()) { // Si se ha recibido un mensaje...

			switch(MASB_COMM_S_command()) { // Miramos que comando hemos recibido

				case START_CV_MEAS: // Si hemos recibido START_CV_MEAS

	                // Leemos la configuracion que se nos ha enviado en el mensaje y
	                // la guardamos en la variable cvConfiguration
					cvConfiguration = MASB_COMM_S_getCvConfiguration();

					HAL_TIM_Base_Start_IT(&htim3);

					__NOP();

					break;

				case START_CA_MEAS:
					caConfiguration = MASB_COMM_S_getCaConfiguration();

					HAL_TIM_Base_Start_IT(&htim3);

					__NOP();

					break;

				case STOP_MEAS:

					__NOP();

					break;

				default:
					// no se que poner

					MASB_COMM_S_sendData(data);

					break;
			}

			MASB_COMM_S_waitForMessage();
	}

}
