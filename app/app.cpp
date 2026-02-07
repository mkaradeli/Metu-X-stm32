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



const char logHeader[] = "600 derece ofsetli 25 derece magnitude 15Hz sinus fonksiyonu"
		"basincli test";


const char* logHeader_ptr = &logHeader[0];
const uint8_t logHeaderSize = sizeof(logHeader);

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
Profiler profiler_position_controller;

void lidarTask(void *pvParameters){
	uint8_t Size = 0;
	for(;;) {
		xTaskNotifyWait(0x00, 0x00, (uint32_t *)&Size, portMAX_DELAY);
		lidar.FrameHandler(Size);
	}
}


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
            	lastWriteDone = sd_unmount() == FR_OK;
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
		if ((micros()-start_flag) < 60 * 1e6){    // Log alma suresi 10 saniyeden 60 saniyeye arttirildi

			txData.data.timestamp = micros();
			txData.data.current_measured = motors[0].getCurrent();
			txData.data.current_demand = current_controller[0].rtU.current_ref;
			txData.data.speedDemand = position_controller[0].rtY.speedDemand;
			txData.data.pos_ref = position_controller[0].rtU.pos_ref;
			txData.data.pos_ref_rate_limited = position_controller[0].rtY.pos_ref_rate_limited;
			txData.data.speed_ref_rate_limited = position_controller[0].rtY.speedDebug.ref_rate_limited;
			txData.data.manifold_pressure = psSensors[0].getBar();
			txData.data.nozzle_pressure = psSensors[1].getBar();








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
	for (int i = 0; i<4; i++)
		position_controller[i].initialize();

	controller_mode = controller_modes::POSITION;

	for (int i= 0; i< 4; i++) {
		position_controller[i].rtU.SpeedFeedback = hallEffect[i].valveVelocity;
		position_controller[i].rtU.pos_ref = 0;
	}

float sayac = 1.0; // Rampa araliklarini ayarlamak icin degisken
float period = 1.0; // 1 saniyelik periyot

					// Sinüs dalgası parametreleri
float amplitude = 25.0;  // Genlik (±90 pozisyon)
float frequency = 15.0;     // Frekans (Hz) - 1 Hz = saniyede 1 tam dalga
float offset = 600.0;        // DC offset (ortalama pozisyon)

	for(;;){
	    osDelay(1);
	    profiler_position_controller.start();
	    time_sec = (micros() - start_flag) / 1e6;

	    // USER CODE START

	    // Sinüs dalgası hareketi
	    position_controller[0].rtU.pos_ref = offset + amplitude * sin(2 * M_PI * frequency * time_sec);

	    // USER CODE END


		for (int i=0; i<4; i++) {
			position_controller[i].rtU.SpeedFeedback = hallEffect[i].valveVelocity;
			position_controller[i].rtU.pos_feedback = hallEffect[i].valveAngleKalman;
			position_controller[i].step();
			current_controller[i].rtU.current_ref = position_controller[i].rtY.currentDemand;

		}
//		if (time_sec< 0.5)
//			current_controller[0].rtU.current_ref = 1;
//		else if (time_sec>0.5 and time_sec < 1)
//			current_controller[0].rtU.current_ref = 0;
//		else if (time_sec< 5)
//			current_controller[0].rtU.current_ref = 0.7*sin(time_sec*2*PI*50);
//		else
//			current_controller[0].rtU.current_ref = 0.7*sin(time_sec*2*PI*800);


		if (time_sec <= 61 && time_sec > 60){

			controller_mode = controller_modes::DISABLE;
		};

		profiler_position_controller.end();

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
	for (int i = 0; i<4; i++)
		current_controller[i].initialize();

	for(uint8_t i=0; i<4; i++){
//		encoderReaderInit(&encoder[i],&EncoderValues[i]);
		actuator[i].motor->initCurrent(&EncoderValues[3]);
		actuator[i].hallEffect->calibrate();
	}
	profiler_hallEffect = Profiler();
	profiler_hallEffect_new = Profiler();
	profiler_current_controller = Profiler();
	profiler_position_controller = Profiler();





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
		profiler_current_controller.start();

		motors[0].updateCurrent(&EncoderValues[3]);
		for (int i=0; i<7; i++)
			txData.data.current_subsample[i] = txData.data.current_subsample[i+1];


//			for (int i=0; i<4; i++) {
//				current_controller[i].rtU.current_feedback = motors[i].getCurrent();
//	//			current_controller.rtU.current_ref = MOTORDUTY_OVERRIDE;
//				current_controller[i].step();
//				motors[i].setSpeed(current_controller[i].rtY.Duty);
//			}

		current_controller[0].rtU.current_feedback = motors[0].getCurrent();
		current_controller[0].step();
		motors[0].setSpeed(current_controller[0].rtY.Duty);

		for (int i=0; i<7; i++)
			txData.data.duty_subsample[i] = txData.data.duty_subsample[i+1];
		txData.data.duty_subsample[7] = current_controller[0].rtY.Duty;
		profiler_current_controller.end();


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


		profiler_current_controller.start();
		motors[0].updateCurrent(&EncoderValues[3] + 128);
		for (int i=0; i<7; i++)
			txData.data.current_subsample[i] = txData.data.current_subsample[i+1];
		txData.data.current_subsample[7] = motors[0].getCurrent();

//		for (int i=0; i<4; i++) {
//			current_controller[i].rtU.current_feedback = motors[i].getCurrent();
//	//		current_controller.rtU.current_ref = MOTORDUTY_OVERRIDE;
//			current_controller[i].step();
//			motors[i].setSpeed(current_controller[i].rtY.Duty);
//		}

		current_controller[0].rtU.current_feedback = motors[0].getCurrent();
		current_controller[0].step();
		motors[0].setSpeed(current_controller[0].rtY.Duty);

		for (int i=0; i<7; i++)
			txData.data.duty_subsample[i] = txData.data.duty_subsample[i+1];
		txData.data.duty_subsample[7] = current_controller[0].rtY.Duty;

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
