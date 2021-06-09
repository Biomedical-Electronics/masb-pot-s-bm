/**
  ******************************************************************************
  * @file		chronoamperometry.c
  * @brief		Gestión de la cronoamperometría.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

#include "components/chronoamperometry.h"
#include "components/stm32main.h"
#include "components/masb_comm_s.h"
#include "main.h"   // tenemos que incluir main.h para que nos reconozca el pin RELAY
#include "components/ad5280_driver.h"
#include "components/mcp4725_driver.h"
#include "components/i2c_lib.h"
#include "components/formulas.h"

// workflow crono:
// iniciem --> fixem tensió Vcell = eDC --> tanquem relé
// true: mesurem Vcell i Icell i enviem dades
// tanquem rele
// false: esperem sampling period

// potenciostat: polaritza amb una tensio Vcell i llegeix el corrent Icell
// per a establir Vcell, usem DAC (MCP4725 amb direcció i2c 1100000)
// el DAC pot generar de 0 a 4V --> es converteix en senyal -4/4V.
// Vdac = 1.65 - Vcell/2


extern MCP4725_Handle_T hdac;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim3;


_Bool tomarPunto = FALSE;


static double RTIA = 50000; // definimos R_tia de 10 kohm

void CA_Start_Meas(struct CA_Configuration_S caConfiguration){
// el DAC genera una tensio

	double Vcell_preADC = caConfiguration.eDC; // necesitamos leer Vcell para poder establecer Vdac
	double V_DAC = calculateDacOutputVoltage(Vcell_preADC);
	MCP4725_SetOutputVoltage(hdac, V_DAC);
	// no conocemos Vcell, tenemos que pasarla por el ADC del microcontroladors
	// double Vcell = (float)(1.65-Vcell_preADC)*2;

	// cerrar relé: como un pin HAL_GPIO
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET); // set = 1 = apagar

	// entramos en condiciones:
	uint32_t sampling_period = caConfiguration.samplingPeriodMs;
	uint32_t measurement_time = caConfiguration.measurementTime * 1000;

	// transcurrido sampling period se toma una muestra durante un tiempo (measurement time)
	// el tiempo lo calculamos con el timer?

	__HAL_TIM_SET_AUTORELOAD(&htim3, sampling_period * 10); // Fijamos el periodo.
	// estamos trabajando a 10 kHz --> TIM3 preescaler 8399 y counter period de 10000

	__HAL_TIM_SET_COUNTER(&htim3, 0); // Reiniciamos el contador del timer a 0.

	HAL_TIM_Base_Start_IT(&htim3);

	uint32_t Vcell_ADC = 0;
	uint32_t Icell_ADC = 0;

	HAL_ADC_Start(&hadc1); //iniciamos conversion
	HAL_ADC_PollForConversion(&hadc1, 100);
	Vcell_ADC = HAL_ADC_GetValue(&hadc1);
	double Vcell = calculateVrefVoltage(Vcell_ADC);

	HAL_ADC_Start(&hadc1); //iniciamos conversion
	HAL_ADC_PollForConversion(&hadc1, 100);
	Icell_ADC = HAL_ADC_GetValue(&hadc1);

	double Icell = calculateIcellCurrent(Icell_ADC);

	struct Data_S data;

	uint32_t counter = 0;
	uint32_t point = 1;

	data.current = Icell;
	data.point = point;
	data.timeMs = counter;
	data.voltage = Vcell;

	MASB_COMM_S_sendData(data);

	counter = counter + sampling_period;
	point = point + 1;


	while (counter <= measurement_time){

		if (tomarPunto == TRUE){
			tomarPunto = FALSE;

			HAL_ADC_Start(&hadc1); //iniciamos conversion
			HAL_ADC_PollForConversion(&hadc1, 100);
			Vcell_ADC = HAL_ADC_GetValue(&hadc1);
			Vcell = calculateVrefVoltage(Vcell_ADC);

			HAL_ADC_Start(&hadc1); //iniciamos conversion
			HAL_ADC_PollForConversion(&hadc1, 100);
			Icell_ADC = HAL_ADC_GetValue(&hadc1);

			Icell = calculateIcellCurrent(Icell_ADC);

			data.current = Icell;
			data.point = point;
			data.timeMs = counter;
			data.voltage = Vcell;

			MASB_COMM_S_sendData(data);

			counter = counter + sampling_period;
			point = point + 1;
		}

	}

	HAL_TIM_Base_Stop_IT(&htim3);

	// abrir relé: como un pin HAL_GPIO
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET); // reset = 0 = abrir

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim3) {

	tomarPunto = TRUE;

}


