/*
 * app_main.cpp
 *
 *  Created on: May 14, 2026
 *      Author: karadeli
 */

#include "shared_memory.h"
#include "app_main.hpp"
#include "main.h"
#include <string.h>

#include "Motor.hpp"
#include "PressureSensor.hpp"
extern "C" {
#include "hx711.h"
}
#include "dummy.hpp"
#include <cstdio>
#include <cmath>


extern "C" {
    extern ADC_HandleTypeDef hadc1;
    extern ADC_HandleTypeDef hadc2;
    extern ADC_HandleTypeDef hadc3;
    extern TIM_HandleTypeDef htim1;
    extern TIM_HandleTypeDef htim2;
    extern TIM_HandleTypeDef htim3;
    extern TIM_HandleTypeDef htim4;
    extern TIM_HandleTypeDef htim6;
}


volatile uint16_t adc_dma_buf_current[16];
volatile uint16_t adc_dma_buf_encoder[4*4];
volatile uint16_t adc_dma_buf_pressure[4*5];
volatile uint8_t adc_data_ready_current = 0;
volatile uint8_t adc_data_ready_enc = 0;
volatile uint8_t adc_data_ready_pres = 0;
volatile uint32_t adc_buffer_full_counter_current = 0;
volatile uint32_t adc_buffer_full_counter_encoder = 0;
volatile uint32_t adc_buffer_full_counter_pressure = 0;

volatile uint32_t timer_counter_tim3=0;
volatile uint32_t timer_counter_tim4=0;
volatile uint32_t timer_counter_tim2=0;

volatile uint32_t timer_counter_tim4_delta=0;
volatile uint32_t timer_counter_tim2_delta=0;
volatile uint32_t timer_counter_tim3_delta=0;


uint32_t timeOfLastPrint=uwTick;
volatile uint32_t timer_counter_last_print_tim3=0;
volatile uint32_t timer_counter_last_print_tim4=0;
volatile uint32_t timer_counter_last_print_tim2=0;

HX711_Handle loadcell = {
	        .dout_port = HX711_DOUT_GPIO_Port,
	        .dout_pin  = HX711_DOUT_Pin,
	        .sck_port  = HX711_SCK_GPIO_Port,
	        .sck_pin   = HX711_SCK_Pin,
	    };



//SensorData_t sensor_data;
SensorData_t local_sensor_data;

Motor motor1(1, true, LEFT_EN_1_GPIO_Port, LEFT_EN_1_Pin, RIGHT_EN_1_GPIO_Port, RIGHT_EN_1_Pin, &htim1, TIM_CHANNEL_1);




void app_init() {
	HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
	  HAL_ADC_Start_DMA(&hadc2, (uint32_t*)adc_dma_buf_current, 16);

	  HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
	  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_dma_buf_encoder, 16);

	  HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
	  HAL_ADC_Start_DMA(&hadc3, (uint32_t*)adc_dma_buf_pressure, 4*5);
	//  HAL_TIM_Base_Start(&htim3);

	  HAL_TIM_Base_Start_IT(&htim2);  /* _IT = interrupt ile */
	  HAL_TIM_Base_Start_IT(&htim3);  /* _IT = interrupt ile */
	  HAL_TIM_Base_Start_IT(&htim4);  /* _IT = interrupt ile */
//	  HAL_TIM_Base_Start(&htim1);  /* _IT = interrupt ile */

	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);


//	  motor(1, true, LEFT_EN_1_GPIO_Port, LEFT_EN_1_Pin, RIGHT_EN_1_GPIO_Port, RIGHT_EN_1_Pin, &htim4, TIM_CHANNEL_1),
//	  Motor_Init(&motor1, /*id=*/0, /*magnetPolarity=*/false,
//			  LEFT_EN_1_GPIO_Port, LEFT_EN_1_Pin, RIGHT_EN_1_GPIO_Port, RIGHT_EN_1_Pin,
//			             &htim1, TIM_CHANNEL_1);
//	  Motor_SetSpeed(&motor1, 0.5f);
	  motor1.setDuty(1.0f);

	  dummy_init();
	  printf(CLR_SCREEN);

//	  printf(CLR_SCREEN);
	  //  uint32_t hclk = HAL_RCC_GetHCLKFreq();
//	    volatile uint32_t dummy=0;

	    HX711_Init(&loadcell, HX711_GAIN_128);

	    local_sensor_data.current_demand = 2;
	    local_sensor_data.timestamp = 0;
	    ready_to_write_a=0;
//	    sensor_data = local_sensor_data;
//	    memcpy(&sensor_data, &local_sensor_data,sizeof(SensorData_t));
//	    memset(&sensor_data, 1, sizeof(sensor_data));

}

void app_loop() {
	if (uwTick >= timeOfLastPrint+1000){
		timer_counter_tim3_delta=timer_counter_tim3 - timer_counter_last_print_tim3;
		timer_counter_last_print_tim3 = timer_counter_tim3;
		timer_counter_tim4_delta=timer_counter_tim4 - timer_counter_last_print_tim4;
		timer_counter_last_print_tim4 = timer_counter_tim4;

		timer_counter_tim2_delta=timer_counter_tim2 - timer_counter_last_print_tim2;
		timer_counter_last_print_tim2 = timer_counter_tim2;

		volatile uint16_t encoder_buffer_full = adc_buffer_full_counter_encoder;
		adc_buffer_full_counter_encoder=0;
		printf("\n\rWelcome to STM32 world ! counter=%d\n\r", (int16_t)(uwTick/1e3));
		printf("%ld, %ld\n\r",HAL_RCC_GetSysClockFreq()/1000000, adc_buffer_full_counter_current);
		printf("timer 3 - adc 2 - CS counter = %ld, should be 8k\n\r",timer_counter_tim3_delta);
		printf("timer 4 - adc 1 - HE counter = %ld, should be 64k\n\r", timer_counter_tim4_delta);
		printf("timer 2 - adc 3 - PT counter = %ld, should be 4k\n\r", timer_counter_tim2_delta);
		uint32_t adc_src = (RCC->D3CCIPR & RCC_D3CCIPR_ADCSEL_Msk) >> RCC_D3CCIPR_ADCSEL_Pos;

		printf("ADC src: %lu Hz\n\r", adc_src);

		printf("adc1 - encoder fill rate = %d\n\r", encoder_buffer_full);
		uint32_t presc = (ADC12_COMMON->CCR & ADC_CCR_PRESC_Msk) >> ADC_CCR_PRESC_Pos;
		printf("adc prescaler = %ld\n\r",presc);
		// Is PLL2 itself running?
		uint32_t pll2_on  = (RCC->CR & RCC_CR_PLL2ON) ? 1 : 0;
		uint32_t pll2_rdy = (RCC->CR & RCC_CR_PLL2RDY) ? 1 : 0;

		// Is PLL2P output enabled?
		uint32_t pll2p_en = (RCC->PLLCFGR & RCC_PLLCFGR_DIVP2EN) ? 1 : 0;

		printf("PLL2 ON=%lu RDY=%lu  PLL2P_EN=%lu\n", pll2_on, pll2_rdy, pll2p_en);
		uint32_t hclk = HAL_RCC_GetHCLKFreq();
		printf("HCLK: %lu Hz\n\r", hclk);

		// And the AHB prescaler that produces it from sysclk
		uint32_t sysclk = HAL_RCC_GetSysClockFreq();
		printf("SYSCLK: %lu Hz\n\r", sysclk);
		//    	adc_buffer_full_counter_encoder=0;
		//    	timer_counter_last_print = timer_counter;

		timeOfLastPrint+= 1000;

		if (adc_data_ready_current)
				{
					adc_data_ready_current = 0;
					printf("current - ");
		//    	            for (int i=0;i<16;i++){
					printf("%d , %d.%2d", adc_dma_buf_current[0], (int)((float)adc_dma_buf_current[0] /65536*5),(int)fmod((int)((float)adc_dma_buf_current[0] /65536*5*100),100));
		//    	            }
					printf("\n\r");
				}
		if (adc_data_ready_enc)
						{
							adc_data_ready_enc = 0;
							printf("enc - ");
							for (int j= 0;j<4;j++){
							for (int i=0;i<4;i++){
								printf("%d, %d,  ", adc_dma_buf_encoder[j+i*4], adc_dma_buf_encoder[j+i*4]*360/65536);
							}
							printf("\n\r");}
							printf("\n\r");
						}
		if (adc_data_ready_pres)
						{
							adc_data_ready_pres = 0;
							printf("pres 0 mA - ");
							for (int j=0;j<4;j++){
								float mA= ((float)(adc_dma_buf_pressure[j*5])/65536*5/220*1000);
								float Bar = (mA-4.0f)*315/16.0f;
								printf("%d.%03d \t", (int)Bar, (int)(fmod(Bar,1)*1000));
							}
							printf("\n\r");
							printf("pres 3 mA - ");
							for (int j=0;j<4;j++){
								float mA= ((float)(adc_dma_buf_pressure[3+j*5])/65536*5/220*1000);
								float Bar = (mA-4.0f)*315/16.0f;
								printf("%d.%03d \t", (int)Bar, (int)(fmod(Bar,1)*1000));
							}
							printf("\n\r");

						}

//		int32_t load = HX711_ReadRawAvg(&loadcell, 2);
//		printf("load = %ld\n\r", load);
		//    	Motor_SetSpeed(&motor1, 0.5f);

		local_sensor_data.timestamp+= 1;
		printf("timestamp = %ld, %p, %p, %p, %p \n\r", local_sensor_data.timestamp, &sensor_data_buffer_a, &sensor_data_buffer_b, &ready_to_write_a, &ready_to_write_b);
		printf("%d\n\r",BUFFER_PACKET_COUNT);
		ready_to_write_a++;
		motor1.setDuty(motor1.getDutyCycle() * -1.0f);
		printf("%%%d\n\r", (int)(motor1.getDutyCycle()*100));
	}
}

//void CurrentSensor


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    	timer_counter_tim3+=1; /* Nucleo yeşil LED → PB0 */
    else if (htim->Instance == TIM4)
    	timer_counter_tim4 += 1;
    else if (htim->Instance == TIM2)
        	timer_counter_tim2 += 1;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC2){
    	// current
		adc_data_ready_current = 1;
		adc_buffer_full_counter_current += 1;
    }
    else if (hadc->Instance == ADC1){
    	// encoder
    	adc_data_ready_enc = 1;
    	adc_buffer_full_counter_encoder +=1;
    }
    else if (hadc->Instance == ADC3){
    	// pressure
    	adc_data_ready_pres = 1;
    	adc_buffer_full_counter_pressure +=1;
    }

}

