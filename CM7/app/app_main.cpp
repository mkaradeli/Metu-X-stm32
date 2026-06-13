/*
 * app_main.cpp
 *
 *  Created on: May 14, 2026
 *      Author: karadeli
 */

#include "globals.hpp"

#define CHECK_TIMER_FREQUENCIES false

#include "shared_memory.h"
#include "app_main.hpp"
#include "main.h"
#include "ring_buffer.h"
#include <string.h>
#include "Hx711.hpp"


#include "PressureSensor.hpp"
//extern "C" {
//#include "hx711.h"
//}
#include "globals.hpp"

#define FRACTIONAL(x) int(floor(int((x)*100)))%100

#define PI 3.1415926536f





Profiler load_cell_profiler;
Profiler main_loop_profiler;
volatile uint64_t micros_overflow=0;
float load;


void current_adc_complete();
void encoder_adc_complete();
void pressure_adc_complete();


__attribute__((section(".sram3"), used))
volatile uint16_t adc_dma_buf_current[4];
__attribute__((section(".sram3"), used))
volatile uint16_t adc_dma_buf_encoder[8*4];
__attribute__((section(".sram3"), used))
volatile uint16_t adc_dma_buf_pressure[5];

uint32_t timeOfLastPrint=uwTick;


volatile float load_filtered;

//HX711_Handle loadcell = {
//	        .dout_port = HX711_DOUT_GPIO_Port,
//	        .dout_pin  = HX711_DOUT_Pin,
//	        .sck_port  = HX711_SCK_GPIO_Port,
//	        .sck_pin   = HX711_SCK_Pin,
//
//	    };


bool mount_ok;
bool file_creation_ok;
float time_sec = 0;
int load_cell_counter = 0;
uint64_t start_flag;

//controller::current currentController;

SensorData_t local_sensor_data;

//Motor motor1(1, true, LEFT_EN_1_GPIO_Port, LEFT_EN_1_Pin, RIGHT_EN_1_GPIO_Port, RIGHT_EN_1_Pin, &htim1, TIM_CHANNEL_1);
LowPass load_lpf{0.1f, 66.6};  // 30 Hz cutoff @ 1 kHz sample rate

Profiler printf_profiler;
Profiler adc1_profiler;
Profiler adc2_profiler;
Profiler adc3_profiler;
Profiler tim2_profiler;
Profiler tim3_profiler;
Profiler tim4_profiler;


void app_init() {
	HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
	  HAL_ADC_Start_DMA(&hadc2, (uint32_t*)adc_dma_buf_current, 4);

	  HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
	  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_dma_buf_encoder, 32);

	  HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
	  HAL_ADC_Start_DMA(&hadc3, (uint32_t*)adc_dma_buf_pressure, 5);


	//  HAL_TIM_Base_Start(&htim3);
	  __HAL_TIM_SET_COUNTER(&htim2, htim2.Instance->ARR - 6);    // ~15 µs to first fire
	  __HAL_TIM_SET_COUNTER(&htim3, htim3.Instance->ARR - 36);   // ~45 µs

#if CHECK_TIMER_FREQUENCIES
	  HAL_TIM_Base_Start_IT(&htim2);  /* pressure */
	  HAL_TIM_Base_Start_IT(&htim3);  /* current */
	  HAL_TIM_Base_Start_IT(&htim4);  /* encoder  */
#else
	  HAL_TIM_Base_Start(&htim2);  /* pressure */
	  HAL_TIM_Base_Start(&htim3);  /* current */
	  HAL_TIM_Base_Start(&htim4);  /* encoder  */
#endif
	  HAL_TIM_Base_Start_IT(&htim5);  /* _IT = interrupt ile */
	  HAL_TIM_Base_Start(&htim1);  /* _IT = interrupt ile */

	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

	  actuator[0].setDuty(1.0f);
//	  motors[0].setDuty(1.0f);

//	  dummy_init();
//	  printf(CLR_SCREEN);

	    local_sensor_data.current_demand = 2;
	    local_sensor_data.timestamp = 0;
	    ready_to_write_a=0;

	    main_loop_profiler.reset();
	    printf_profiler.reset();
	    adc1_profiler.reset();
	    adc2_profiler.reset();
	    adc3_profiler.reset();
	    tim2_profiler.reset();
	    tim3_profiler.reset();
	    tim4_profiler.reset();

	    rb_init(&common_print_buffer);
	    setvbuf(stdout, NULL, _IONBF, 0);   /* important: disable stdio line buffering */

//	    SCB_InvalidateDCache_by_Addr((uint32_t *)adc_dma_buf_encoder, sizeof(adc_dma_buf_encoder));
	    HAL_Delay(10);
	    for (int i=0; i<4; i++)
	    	actuator[i].calibrate();
//	    currentController.initialize();
	    Actuator::manifold->calibrate();

	    if (mount_ok and file_creation_ok)
			controller_mode = controller_modes::PRESSURE;

		for (int i= 0; i< 4; i++) {
			actuator[i].actuatorController.rtU.pos_feedback = actuator[i].hallEffect.valveAngle;
			actuator[i].actuatorController.rtU.SpeedFeedback = actuator[i].hallEffect.valveVelocity;
			actuator[i].actuatorController.rtU.P_manifold = Actuator::manifold->getPsi();
			actuator[i].actuatorController.rtU.P_nozzle_demand = 0.0F;

		}

		start_flag = micros();
//		actuator[0].static_manifold = &psSensors[4];

}

void app_loop() {
	if (uwTick - timeOfLastPrint >= 1000){
		main_loop_profiler.start();

		timeOfLastPrint+= 1000;

//		printf(CLR_SCREEN);
		local_sensor_data.timestamp+= 1;
		printf("timestamp = %ld, %ld \n\r", uwTick, micros());

		printf("\n\rWelcome to STM32 world ! counter=%d\n\r", (int16_t)(uwTick/1e3));

		printf("adc 2 - CS counter = %d, should be 8k\n\r", (int)adc2_profiler.call_frequency);

		printf("current controller mean time = %d.%d us, cpu usage = %%%d.%d\n\r", (int)adc2_profiler.mean_time, FRACTIONAL(adc2_profiler.mean_time), (int)adc2_profiler.cpu_usage, FRACTIONAL(adc2_profiler.cpu_usage));
		printf("adc 1 - HE counter = %d, should be 4k\n\r", (int)adc1_profiler.call_frequency);
		printf("encoder decode mean time = %d.%d us, cpu usage = %%%d.%d\n\r", (int)adc1_profiler.mean_time, FRACTIONAL(adc1_profiler.mean_time), (int)adc1_profiler.cpu_usage, FRACTIONAL(adc1_profiler.cpu_usage));
		printf("adc 3 - PT counter = %d, should be 1k\n\r", (int)adc3_profiler.call_frequency);
		printf("pressure decode mean time = %d.%d us, cpu usage = %%%d.%d\n\r", (int)adc3_profiler.mean_time, FRACTIONAL(adc3_profiler.mean_time), (int)adc3_profiler.cpu_usage, FRACTIONAL(adc3_profiler.cpu_usage));
//		uint32_t adc_src = (RCC->D3CCIPR & RCC_D3CCIPR_ADCSEL_Msk) >> RCC_D3CCIPR_ADCSEL_Pos;

		printf("main loop mean time = %d us\n\r", (int) main_loop_profiler.mean_time);
		printf("printf mean time = %d us\n\r", (int) printf_profiler.mean_time);
//		printf("current controller mean time = %d us\n\r", (int) adc2_profiler.mean_time);
		printf("%d\n\r",BUFFER_PACKET_COUNT);
//		ready_to_write_a++;
//		actuator[0].setDuty(actuator[0].getDutyCycle() * -1.0f);
		printf("%%%d\n\r", (int)(actuator[0].getDutyCycle()*100));
		printf("timestamp = %ld, %ld \n\r", uwTick, micros());

		if (adc1_profiler.get_start_click() < adc2_profiler.get_start_click())
			printf("encoder is before current controller\n\r");
		else
			printf("current is before encoder \n\r");

		if (adc2_profiler.get_start_click() < adc3_profiler.get_start_click())
			printf("current is before pressure controller\n\r");
		else
			printf("pressure is before current \n\r");
		printf("pressure start %ld\n\r", adc3_profiler.get_start_click());
		printf("encoder start %ld\n\r", adc1_profiler.get_start_click());
		printf("current start %ld\n\r", adc2_profiler.get_start_click());




		main_loop_profiler.metrics();
		printf_profiler.metrics();
		adc1_profiler.metrics();
		adc2_profiler.metrics();
		adc3_profiler.metrics();
		tim2_profiler.metrics();
		tim3_profiler.metrics();
		tim4_profiler.metrics();


		main_loop_profiler.end();
	}
}


void tim3_trigger(){
	tim3_profiler.start();
//	timer_counter_tim3+=1; /* Nucleo yeşil LED → PB0 */
	tim3_profiler.end();

}
void tim4_trigger(){
	tim4_profiler.start();
//	timer_counter_tim4 += 1;
	tim4_profiler.end();
}
void tim2_trigger(){
	tim2_profiler.start();
//	timer_counter_tim2 += 1;
	tim2_profiler.end();
}

void tim5_trigger(){
//	tim2_profiler.start();
	micros_overflow++;
//	tim2_profiler.end();
}

uint64_t micros(){
	return (micros_overflow<<32) + __HAL_TIM_GET_COUNTER(&htim5);
}

void current_adc_complete(){
	adc2_profiler.start();
//    SCB_InvalidateDCache_by_Addr((uint32_t *)adc_dma_buf_current, sizeof(adc_dma_buf_current));

//	uint16_t current_meas;
	for (int i=0; i<4; i++){
		actuator[i].updateCurrent(adc_dma_buf_current[i]);

		actuator[i].current_controller_step();
	}
	adc2_profiler.end();
}

void encoder_adc_complete(){
	adc1_profiler.start();
//	SCB_InvalidateDCache_by_Addr((uint32_t *)adc_dma_buf_encoder, sizeof(adc_dma_buf_encoder));
	for (int i=0; i<4; i++){
		actuator[i].updateHallEffect();
	}
	adc1_profiler.end();
}

void pressure_adc_complete(){
	adc3_profiler.start();
	for (int i=0; i<4; i++){
		actuator[i].readPressure();
	}
	Actuator::manifold->updatePS();


	time_sec = (micros() - start_flag) / 200e6;

	// Sinüs dalgası hareketi
	if (time_sec<120)
		if (fmod(time_sec,3)<0.5)
		  actuator[0].actuatorController.rtU.P_nozzle_demand = 1000;
		else if (fmod(time_sec,3)<1)
			actuator[0].actuatorController.rtU.P_nozzle_demand = 500;
		else if (fmod(time_sec,3)<2.5)
			actuator[0].actuatorController.rtU.P_nozzle_demand = sin(5*PI*2*time_sec)*150 + 600;
		else
			actuator[0].actuatorController.rtU.P_nozzle_demand = 0;
	else if (time_sec<121){
		actuator[0].actuatorController.rtU.P_nozzle_demand = 0;
		if (controller_mode == controller_modes::PRESSURE)
			controller_mode = controller_modes::POSITION;
		actuator[0].actuatorController.rtU.pos_ref_ext = 0;
	}


	for (int i=0; i<4; i++) {
		actuator[i].actuator_controller_step();
	}

	if (time_sec <= 122 && time_sec > 121){

		controller_mode = controller_modes::DISABLE;
	};

	adc3_profiler.end();
}
