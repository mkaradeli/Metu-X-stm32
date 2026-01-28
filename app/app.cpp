  /*
 * app.cpp
 *
 *  Created on: Aug 7, 2025
 *      Author: alica
 */

#include "globals.hpp"
#include "globals.h"
#include "app.h"
//#include "encoder.h"
#include "profiler.hpp"

#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "spi.h"

#include "string.h"
#include "stdio.h"
#include <stdint.h>



#define PACKET_SIZE sizeof(SensorData_t)
#define BUFFER_PACKETS (65536 / PACKET_SIZE )
#define PI 3.1415926536

#define BUFFER_SIZE (PACKET_SIZE * BUFFER_PACKETS)
//uint32_t timclk;
//float pwm_freq;
//HAL_RCC_ClkInitTypeDef clk;
//uint32_t flashLatency;



//#define ADC2_BUF_LEN     256
//#define ADC2_CH_COUNT    4


//static_assert(BUFFER_SIZE==910*72);

uint32_t adc_clk =0;
uint32_t adc_updatePeriod=0;


uint8_t bufferA[BUFFER_SIZE];
uint8_t bufferB[BUFFER_SIZE];
uint8_t *activeBuffer = bufferA;
uint8_t *writeBuffer = NULL;
uint32_t activeIndex = 0;


float MOTORDUTY_OVERRIDE = 0.0f;
bool mount_ok;
// float adc_updateFreq = 0.0f;
// uint32_t micros_old[30]={0};


uint32_t bufferIndex = 0;
bool lastWrite = false;
int lastWriteDone = false;
SemaphoreHandle_t writeSemaphore;

uint8_t myrxBuffer[5];

typedef struct {
    float x;
    float y;
    float z;
} Force_t;

Force_t force_measurement = {0, 0, 0};

uint8_t counter = 0;

uint32_t log_buffer_delay = 0;
// uint32_t controller_delay = 0;
 uint32_t accel_delay = 0;
 uint32_t rotation_delay = 0;

TaskHandle_t dischargeTaskHandle;
TaskHandle_t controlTaskHandle;
TaskHandle_t bufferDataTaskHandle;
TaskHandle_t safetyConnectorTaskHandle;

Profiler profiler_hallEffect;
Profiler profiler_hallEffect_new;
Profiler profiler_current_controller;

void lidarTask(void *pvParameters){
	uint8_t Size = 0;
	for(;;) {
		xTaskNotifyWait(0x00, 0x00, (uint32_t *)&Size, portMAX_DELAY);
		lidar.FrameHandler(Size);
	}
}

// void motorTask(void *pvParameters){
// 	for(;;){
// 		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
// 		;
// //		for (int i =0 ; i<4; i++)
// 		motors[0].updatePosition(EncoderValues[0]);
// 		motors[0].updateCurrent(&EncoderValues[3]);
// 	}
// }

void psTask(void *pvParameters){
	// hallEffect[0].update_subBuffer();
	for(;;){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		for (uint8_t i = 0; i < MAX_PS_COUNT; i++)
			psSensors[i].updatePS(PSValues[i]);
			// psSensors[i].updatePS(PSValues[i]);
		force_measurement.x = PSValues[3]/4096.0*3300.0/5000.0*750.0;
		force_measurement.y = EncoderValues[2]/4096.0*10.0*360.0;
	}
}

void sdCardTask(void *pvParameters){
	mount_ok = (sd_mount() == FR_OK);
	sd_create_log_file();

	sd_write_log_file((uint8_t*)logFormatID_ptr,sizeof(uint16_t));
	sd_write_log_file(&logHeaderSize, sizeof(uint8_t));
	sd_write_log_file((uint8_t*)logHeader_ptr, logHeaderSize);
	sd_write_log_file((uint8_t*)&sensorDataLength, sizeof(uint16_t));



	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	for(;;){
        if (xSemaphoreTake(writeSemaphore, portMAX_DELAY) == pdTRUE) {
            // Write full buffer to SD

    		sd_write_log_file(writeBuffer, bufferIndex > 0 ? bufferIndex : BUFFER_SIZE);
            writeBuffer = NULL;
            if (lastWrite){
            	sd_close_log_file();
            	lastWriteDone = sd_unmount();
//            	lastWriteDone = ;
            	osThreadTerminate(NULL);

            }
        }
	}
}

void bufferDataTask(void *pvParameters){
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	uint32_t start_flag = micros();
	uint32_t delay_flag = micros();
	for(;;){
		osDelay(1);
		if ((micros()-start_flag) < 10 * 1e6){

			txData.data.timestamp = micros();
//			txData.data.motor_duty = metux_controller_Y.MotorDuty;
			txData.data.current_measured = motors[0].getCurrent();
			txData.data.current_demand = current_controller.rtU.current_ref;
			txData.data.encoderButt = hallEffect[0].lastReading;
			txData.data.vel_measured = 0;
			txData.data.motor_pos_kalman = 0;
			txData.data.angleRaw = hallEffect[0].angleRaw;
			txData.data.current_raw = EncoderValues[3];

			txData.data.speedDemand = position_controller.rtY.speedDemand;
			txData.data.pos_ref = position_controller.rtU.pos_ref;



			// txData.data.valveAngle = hallEffect[0].valveAngle;
			// txData.data.valveAngleKalman = hallEffect[0].valveAngleKalman;
			// txData.data.valveVelocity = hallEffect[0].valveVelocity;
			// txData.data.current_subsample = motors[0].getCurrent();




		    memcpy(&activeBuffer[bufferIndex], txData.bytes, PACKET_SIZE);
		    bufferIndex += PACKET_SIZE;

		    if (bufferIndex >= BUFFER_SIZE) {
		        // Prepare buffer for writing
		        writeBuffer = activeBuffer;

		        // Swap active buffer
		        activeBuffer = (activeBuffer == bufferA) ? bufferB : bufferA;
		        bufferIndex = 0;
		        xSemaphoreGive(writeSemaphore);
		    }
		}
		else {
	        writeBuffer = activeBuffer;
	        lastWrite = true;
	        xSemaphoreGive(writeSemaphore);
		}
		log_buffer_delay = micros()-delay_flag;
		delay_flag = micros();
	}
}

void controllerTask(void *pvParameters){
	float time_sec = 0;
//	timclk = HAL_RCC_GetPCLK1Freq();
//
//	if (HAL_RCC_GetPCLK1Freq() != HAL_RCC_GetHCLKFreq())
//	    timclk *= 2;
//
//	pwm_freq = timclk / (4.0f * 4096.0f);
//	HAL_RCC_GetClockConfig(&clk, &flashLatency);


	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	uint32_t start_flag = micros();
//	uint32_t delay_flag = micros();

	position_controller.initialize();

	position_controller.rtU.speed_enable = true;
	position_controller.rtU.position_enable = true;
	position_controller.rtU.SpeedFeedback = hallEffect[0].valveVelocity;
	position_controller.rtU.pos_ref = hallEffect[0].valveAngleKalman;
	position_controller.rtU.pos_ref = 0;
	current_controller.rtU.enabled = true;
	float freq = 5;
	for(;;){
		osDelay(1);
		time_sec = (micros() - start_flag) / 1e6;

		if (time_sec<0.1)
			position_controller.rtU.pos_ref = 360*5;
		else if (time_sec>1.5 and time_sec<1.51)
			position_controller.rtU.pos_ref = 0;
		else if (time_sec > 3 and time_sec < 10)
			position_controller.rtU.pos_ref = 100 * sin(time_sec * 2*PI* freq);

		position_controller.rtU.SpeedFeedback = hallEffect[0].valveVelocity;
		position_controller.rtU.pos_feedback = hallEffect[0].valveAngleKalman;
		position_controller.step();
		current_controller.rtU.current_ref = position_controller.rtY.currentDemand;



		if (time_sec <= 12 && time_sec > 11){
			current_controller.rtU.enabled = false;
			position_controller.rtU.speed_enable = false;
			position_controller.rtU.position_enable = false;
		};

	}
}


void safetyConnectorTask(void *pvParameters){
	osDelay(1000);
	for(;;){
		osDelay(1);
		while (HAL_GPIO_ReadPin(SAFETY_GPIO_Port, SAFETY_Pin)){
			counter++;
			if (counter > 50)
				break;
		}
		if (counter > 50){
			xTaskNotifyGive(bufferDataTaskHandle);
			xTaskNotifyGive(sdCardTaskHandle);
			xTaskNotifyGive(controlTaskHandle);
			osThreadTerminate(NULL);
		}
	}
}

void dischargeTask(void *pvParameters){
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	osThreadTerminate(controlTaskHandle);
	//osThreadTerminate(bufferDataTaskHandle);
	//osThreadTerminate(sdCardTaskHandle);
	for(;;){
		printf("Tahliye Başlatılıyor...\n");
		osDelay(1000);
		uint32_t flag = micros();
		motors[0].setSpeed(0.65);
		motors[1].setSpeed(0.65);
		motors[2].setSpeed(0.65);
		motors[3].setSpeed(0.65);
		while((micros()-flag) < 50000);
		motors[0].setSpeed(0);
		motors[1].setSpeed(0);
		motors[2].setSpeed(0);
		motors[3].setSpeed(0);
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	}
}

void app_start(){
	HAL_UART_Receive_DMA(&huart1, myrxBuffer, 5);

	IMU_Init();

    writeSemaphore = xSemaphoreCreateBinary();

	xTaskCreate(lidarTask, "Lidar Task", 128, NULL, 6, &lidarTaskHandle);
	// xTaskCreate(motorTask, "Motor Task", 128, NULL, 5, &motorTaskHandle);
	xTaskCreate(psTask, "PS Task", 128, NULL, 5, &psTaskHandle);
	xTaskCreate(dischargeTask, "Discharge Task", 128, NULL, 6, &dischargeTaskHandle);


	lidar.Reset();

	HAL_UARTEx_ReceiveToIdle_DMA(&huart2, lidar.getBuffer(), 128);


	HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
	//HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);

	HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
	//HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);



	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)PSValues, 5);
	HAL_ADC_Start_DMA(&hadc2, (uint32_t *)EncoderValues, ADC2_CH_COUNT*ADC2_BUF_LEN);



	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
	HAL_TIM_Base_Start_IT(&htim13);

	xTaskCreate(controllerTask, "Control Task", 128*30, NULL, 5, &controlTaskHandle);
	xTaskCreate(safetyConnectorTask, "Safety Connector", 128, NULL, 6, &safetyConnectorTaskHandle);
	xTaskCreate(bufferDataTask, "Buffer Data", 128, NULL, 5, &bufferDataTaskHandle);
	xTaskCreate(sdCardTask, "PS Task", 128*5, NULL, 5, &sdCardTaskHandle);

//	for(uint8_t i = 0; i < 4; i++){
//		motors[i].setPositionDegree(0*360.0);
	// }
	current_controller.initialize();

	for(uint8_t i=0; i<4; i++){
//		encoderReaderInit(&encoder[i],&EncoderValues[i]);
		actuator[i].motor->initCurrent(&EncoderValues[3]);
		actuator[i].hallEffect->calibrate();
	}
	profiler_hallEffect = Profiler();
	profiler_hallEffect_new = Profiler();
	profiler_current_controller = Profiler();





}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
    static BaseType_t xHigherPriorityTaskWoken;
	if (huart->Instance == USART2){
		HAL_UARTEx_ReceiveToIdle_DMA(&huart2, lidar.getBuffer(), 128);
		xHigherPriorityTaskWoken = pdFALSE;
		xTaskGenericNotifyFromISR(lidarTaskHandle, Size, eSetValueWithOverwrite, NULL, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    static BaseType_t xHigherPriorityTaskWoken;
    if (huart->Instance == USART1)
    {
		xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(dischargeTaskHandle, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    	HAL_UART_Receive_DMA(&huart1, myrxBuffer, 5);
    }
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc){
	if (hadc->Instance == ADC2){
//			encoderReader(&encoder[0], &EncoderValues[0]);


			motors[0].updateCurrent(&EncoderValues[3]);
			for (int i=0; i<7; i++)
				txData.data.current_subsample[i] = txData.data.current_subsample[i+1];



			current_controller.rtU.current_feedback = motors[0].getCurrent();
//			current_controller.rtU.current_ref = MOTORDUTY_OVERRIDE;
			current_controller.step();
			motors[0].setSpeed(current_controller.rtY.Duty);
			for (int i=0; i<7; i++)
				txData.data.duty_subsample[i] = txData.data.duty_subsample[i+1];
			txData.data.duty_subsample[7] = current_controller.rtY.Duty;


	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	static BaseType_t xHigherPriorityTaskWoken;
	if (hadc->Instance == ADC1){
		// TODO: HAL_ADC_Start_DMA function should only run in app start once.
		HAL_ADC_Start_DMA(&hadc1, (uint32_t *)PSValues, MAX_PS_COUNT);
		xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(psTaskHandle, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
	else if (hadc->Instance == ADC2) {
		// profiler_hallEffect.start();
		// for (int index = 0; index < 4; index++){
		// 	hallEffect[index].update();
		// }
		// profiler_hallEffect.end();


		profiler_hallEffect_new.start();
		for (int index = 0; index <4; index++){
			hallEffect[index].update_subBuffer();
		}
		for (int i=0;i<3; i++){
			txData.data.valveAngle[i] = txData.data.valveAngle[i+1];
			txData.data.valveAngleKalman[i] = txData.data.valveAngleKalman[i+1];
			txData.data.valveVelocity[i] = txData.data.valveVelocity[i+1];
		}

		txData.data.valveAngle[3] = hallEffect[0].valveAngle;
		txData.data.valveAngleKalman[3] = hallEffect[0].valveAngleKalman;
		txData.data.valveVelocity[3] = hallEffect[0].valveVelocity;
		profiler_hallEffect_new.end();




		// adc_updatePeriod = (micros() - micros_old[29])/30.0f;
		// adc_updateFreq =  1.0f/ adc_updatePeriod* 1.0e6;

		// adc_clk /= HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_ADC);


		profiler_current_controller.start();
		motors[0].updateCurrent(&EncoderValues[3] + 128);
		for (int i=0; i<7; i++)
			txData.data.current_subsample[i] = txData.data.current_subsample[i+1];
		txData.data.current_subsample[7] = motors[0].getCurrent();

		current_controller.rtU.current_feedback = motors[0].getCurrent();
//		current_controller.rtU.current_ref = MOTORDUTY_OVERRIDE;
		current_controller.step();
		motors[0].setSpeed(current_controller.rtY.Duty);

		for (int i=0; i<7; i++)
			txData.data.duty_subsample[i] = txData.data.duty_subsample[i+1];
		txData.data.duty_subsample[7] = current_controller.rtY.Duty;

		profiler_current_controller.end();



		xHigherPriorityTaskWoken = pdFALSE;
		// vTaskNotifyGiveFromISR(motorTaskHandle, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

	}
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
	static BaseType_t xHigherPriorityTaskWoken;
	if (hspi == &SD_SPI_HANDLE){
		xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(sdCardTaskHandle, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
	if (hspi == &hspi1){
		xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(imuTaskHandle, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
	static BaseType_t xHigherPriorityTaskWoken;
	if (hspi == &SD_SPI_HANDLE) {
		xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(sdCardTaskHandle, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM17)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}
