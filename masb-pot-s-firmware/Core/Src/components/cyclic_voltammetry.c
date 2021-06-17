/**
  ******************************************************************************
  * @file		cyclic_voltammetry.c
  * @brief		Gestión de la voltammetría cíclica.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

#include "components/cyclic_voltammetry.h"
#include "components/stm32main.h"
#include "components/masb_comm_s.h"
#include "main.h"   // tenemos que incluir main.h para que nos reconozca el pin RELAY
#include "components/ad5280_driver.h"
#include "components/mcp4725_driver.h"
#include "components/i2c_lib.h"
#include "components/formulas.h"
#include <math.h>

static UART_HandleTypeDef *huart;
static MCP4725_Handle_T hdac;
extern ADC_HandleTypeDef hadc1;
static I2C_HandleTypeDef *hi2c1;
extern TIM_HandleTypeDef htim3;


extern _Bool tomarPunto;
static double RTIA = 50000; // definimos R_tia de 10 kohm


void CV_Start_Meas(struct CV_Configuration_S cvConfiguration){
	double Vcell_preADC_CV = cvConfiguration.eBegin;
	double V_DAC_CV = (1.65 - Vcell_preADC_CV/2.0);
	// double V_DAC_CV = calculateDacOutputVoltage(Vcell_preADC_CV);
	MCP4725_SetOutputVoltage(hdac, V_DAC_CV);

	double vObjetivo = cvConfiguration.eVertex1;

	// cerrar relé
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET); // set = 1 = apagar

	double eStep = cvConfiguration.eStep;
	double scanRate = cvConfiguration.scanRate;

	double ts_CV = eStep/scanRate;


	__HAL_TIM_SET_AUTORELOAD(&htim3, ts_CV * 10); // Fijamos el periodo.
	// estamos trabajando a 10 kHz --> TIM3 preescaler 8399 y counter period de 10000

	__HAL_TIM_SET_COUNTER(&htim3, 0); // Reiniciamos el contador del timer a 0.

	HAL_TIM_Base_Start_IT(&htim3);

	uint32_t Vcell_ADC_CV = 0;
	uint32_t Icell_ADC_CV = 0;

	HAL_ADC_Start(&hadc1); //iniciamos conversion
	HAL_ADC_PollForConversion(&hadc1, 100); // -->  NOU
	Vcell_ADC_CV = HAL_ADC_GetValue(&hadc1);
	// double Vcell_CV = (double)(1.65-Vcell_ADC_CV)*2;
	double Vcell_CV = calculateVrefVoltage(Vcell_ADC_CV);
	// double Vcell_CV = calculateDacOutputVoltage(Vcell_ADC_CV);

	// double Icell_CV = (double)((Vcell_ADC_CV - 1.65)*2)/RTIA;
	HAL_ADC_Start(&hadc1); //iniciamos conversion
	HAL_ADC_PollForConversion(&hadc1, 100);
	Icell_ADC_CV = HAL_ADC_GetValue(&hadc1);

	double Icell_CV = calculateIcellCurrent(Icell_ADC_CV);

	uint32_t counter_CV = 0;
	uint32_t point_CV = 0;
	uint8_t cycles = 0;

	while (cycles < cvConfiguration.cycles){
		// si els cicles son < que els cicles de la configuracio seguim dins el bucle
		//abans hauriem de fer una variable de cicles = 0
		while (counter_CV < ts_CV){ //canviar counter per cicles // counter_CV < ts_CV
			if (tomarPunto){
				tomarPunto = FALSE;
				HAL_ADC_Start(&hadc1); //iniciamos conversion
				Vcell_ADC_CV = HAL_ADC_GetValue(&hadc1);
				Icell_ADC_CV = HAL_ADC_GetValue(&hadc1);

				Vcell_CV = calculateDacOutputVoltage(Vcell_ADC_CV);
				Icell_CV = calculateIcellCurrent(Icell_ADC_CV);
				// Vcell_CV = (double)(1.65-Vcell_ADC_CV)*2;
				// Icell_CV = (double)((Vcell_ADC_CV - 1.65)*2)/RTIA;

				point_CV = point_CV + 1;

				struct Data_S data;
				data.current = Icell_CV;
				data.point = point_CV;
				data.timeMs = counter_CV;
				data.voltage = Vcell_CV;

				MASB_COMM_S_sendData(data);


				counter_CV = counter_CV + eStep;

				if (Vcell_CV == vObjetivo) {

					if (vObjetivo == cvConfiguration.eVertex1){
						vObjetivo = cvConfiguration.eVertex2;

					} else if (vObjetivo == cvConfiguration.eVertex2){
							vObjetivo = cvConfiguration.eBegin;

					} else if ((cycles -1 ) == cvConfiguration.cycles) {
							break;

					} else {
						vObjetivo = cvConfiguration.eVertex1;
						cycles = cycles + 1;
					}

				} else {
					if (vObjetivo == cvConfiguration.eVertex1){

						if (Vcell_CV + eStep > vObjetivo){
								Vcell_CV = vObjetivo;
						} else {
							Vcell_CV = Vcell_CV + eStep;
					}

					MCP4725_SetOutputVoltage(hdac, V_DAC_CV);

				} else if (vObjetivo == cvConfiguration.eVertex2) {
					if (Vcell_CV + eStep < vObjetivo){
						Vcell_CV = vObjetivo;
					} else {
						Vcell_CV = Vcell_CV + eStep;
					}

				MCP4725_SetOutputVoltage(hdac, V_DAC_CV);

				} else if (vObjetivo == cvConfiguration.eBegin){
					if (Vcell_CV + eStep > vObjetivo){
						Vcell_CV = vObjetivo;
					} else {
						Vcell_CV = Vcell_CV + eStep;
					}
				MCP4725_SetOutputVoltage(hdac, V_DAC_CV);
				}
			}
		}


	}
	}



	HAL_TIM_Base_Stop_IT(&htim3); // paramos el timer

	// Abrir relé
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
}




