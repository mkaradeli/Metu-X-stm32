  /*
 * app.cpp
 *
 *  Created on: Aug 7, 2025
 *      Author: alica
 */

#include "globals.hpp"
#include "globals.h"
#include "app.h"

#include "adc.h"
#include "usart.h"
#include "spi.h"

#include "string.h"
#include "stdio.h"

#define PACKET_SIZE sizeof(SensorData_t)
#define BUFFER_PACKETS (65536 / PACKET_SIZE )

#define BUFFER_SIZE (PACKET_SIZE * BUFFER_PACKETS)

//#define ADC2_BUF_LEN     256
//#define ADC2_CH_COUNT    4


//static_assert(BUFFER_SIZE==910*72);


uint8_t bufferA[BUFFER_SIZE];
uint8_t bufferB[BUFFER_SIZE];
uint8_t *activeBuffer = bufferA;
uint8_t *writeBuffer = NULL;
uint32_t activeIndex = 0;


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
uint32_t controller_delay = 0;
uint32_t accel_delay = 0;
uint32_t rotation_delay = 0;

TaskHandle_t dischargeTaskHandle;
TaskHandle_t controlTaskHandle;
TaskHandle_t bufferDataTaskHandle;
TaskHandle_t safetyConnectorTaskHandle;

void lidarTask(void *pvParameters){
	uint8_t Size = 0;
	for(;;) {
		xTaskNotifyWait(0x00, 0x00, (uint32_t *)&Size, portMAX_DELAY);
		lidar.FrameHandler(Size);
	}
}

void motorTask(void *pvParameters){
	for(;;){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		;
		for (int i =0 ; i<4; i++)
			motors[i].updatePosition(EncoderValues[i]);
		motors[0].updateCurrent(EncoderValues[3]);
	}
}

void psTask(void *pvParameters){
	for(;;){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		for (uint8_t i = 0; i < MAX_PS_COUNT; i++)
			psSensors[i].updatePS(PSValues[i]);
		force_measurement.x = PSValues[3]/4096.0*3300.0/5000.0*750.0;
		force_measurement.y = EncoderValues[2]/4096.0*10.0*360.0;
	}
}

void sdCardTask(void *pvParameters){
	sd_mount();
	sd_create_log_file();

	sd_write_log_file((uint8_t*)logFormatID_ptr,sizeof(uint16_t));
	sd_write_log_file(&logHeaderSize, sizeof(uint8_t));
	sd_write_log_file((uint8_t*)logHeader_ptr, logHeaderSize);




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

//		    uint32_t timestamp;
//			float motor_duty;
//		    float current_measured;
//		    float current_demand;
//		    float encoderFront;
//		    float encoderButt;
//		    float encoderGeared;
//		    float vel_measured;
//		    float vel_demand;
//		    float motor_pos_kalman;
//		    float motor_pos_demand;
//		    float mass_estimation;
//			float pressure_manifold;
//		    float pressure_nozzle;
//		    float pressure_demand;
//		    float force_feedback;
//		    float force_measured;
//		    float thrust_demand;


			txData.data.timestamp = micros();
			txData.data.motor_duty = metux_controller_Y.MotorDuty;
			txData.data.current_measured = motors[0].getCurrent();
			txData.data.current_demand = metux_controller_Y.current_demand;
			txData.data.encoderFront = motors[1].getPositionDegree();
			txData.data.encoderButt = EncoderValues[0];
			txData.data.encoderGeared = motors[2].getPositionDegree();
			txData.data.vel_measured = metux_controller_Y.motor_speed;
			txData.data.vel_demand = metux_controller_Y.motor_speed_demand;
			txData.data.motor_pos_kalman = metux_controller_Y.motor_position_kalman;
			txData.data.motor_pos_demand = metux_controller_Y.motor_position_demand;
			txData.data.mass_estimation = metux_controller_Y.mass_estimation;
			txData.data.pressure_manifold = psSensors[4].getBar();
			txData.data.pressure_nozzle = psSensors[0].getBar();
			txData.data.pressure_demand = metux_controller_Y.pressure_demand;
			txData.data.force_feedback = metux_controller_Y.force_feedback;
			txData.data.force_measured = 0;
			txData.data.thrust_demand = metux_controller_Y.thrust_demand;

			activeIndex = __HAL_DMA_GET_COUNTER(&hdma_adc2);

			uint32_t written = ADC2_BUF_LEN*ADC2_CH_COUNT - activeIndex + activeIndex%4;
			uint32_t head = written % (ADC2_BUF_LEN*ADC2_CH_COUNT);

			for (uint16_t i=0; i<ADC2_BUF_LEN; i++){
//				(start + i) % ADC2_BUF_LEN
				txData.data.encoder_readings[i] = EncoderValues[(head + i*ADC2_CH_COUNT) % (ADC2_BUF_LEN*ADC2_CH_COUNT)];
				txData.data.current_readings[i] = EncoderValues[(head + i*ADC2_CH_COUNT) % (ADC2_BUF_LEN*ADC2_CH_COUNT) + 3];
			}



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
	metux_controller_initialize();
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	for (uint8_t i = 0; i < 4; i++){
		metux_controller_U.MotorAngle = motors[i].getPositionDegree();
		metux_controller_U.pressure_nozzle = psSensors[i].getBar();
		metux_controller_U.MotorsEnabled = 1;
	}
	metux_controller_U.ESTOP = 0;
	metux_controller_U.ControllerMode = 1;
	metux_controller_U.Activate = 0;

	for (uint8_t i = 0; i< 20; i++)
		metux_controller_step();

	metux_controller_U.Activate = 1;
	uint32_t start_flag = micros();
//	uint32_t delay_flag = micros();
	for(;;){
		osDelay(1);
		time_sec = (micros() - start_flag) / 1e6;
		metux_controller_U.current_measured = motors[0].getCurrent();
		metux_controller_U.MotorAngle = motors[0].getPositionDegree();
		metux_controller_U.pressure_manifold = psSensors[4].getBar();
		metux_controller_U.pressure_nozzle = psSensors[0].getBar();
		metux_controller_U.current_demand_ext = 0;
		metux_controller_U.motor_speed_demand_ext = 0;
		metux_controller_U.motor_pos_demand_ext = 0;
		metux_controller_U.pressure_demand_ext = 0;
		metux_controller_U.thrust_demand_ext = 0;


			//		current_measured = motors[0].getCurrent();
			//	current_demand = metux_controller_Y.current_demand;
			//	encoderFront = motors[0].getPositionDegree();
			//	encoderButt = motors[1].getPositionDegree();
			//	encoderGeared = motors[2].getPositionDegree();
			//	vel_measured = metux_controller_Y.motor_speed;
			//	vel_demand = metux_controller_Y.motor_speed_demand;
			//	motor_pos_kalman = metux_controller_Y.motor_position_kalman;
			//	motor_pos_demand = metux_controller_Y.motor_position_demand;
			//	mass_estimation = metux_controller_Y.mass_estimation;
			//	pressure_manifold = psSensors[4].getBar();
			//	pressure_nozzle = psSensors[0].getBar();
			//	pressure_demand = metux_controller_Y.pressure_demand;
			//	force_feedback = metux_controller_Y.force_feedback;
			//	force_measured = 0;
			//	thrust_demand = metux_controller_Y.thrust_demand;

		if (time_sec < 10){
			metux_controller_U.current_demand_ext = 1*sin(time_sec * 10 * PI * 2);

//			if (int(time_sec/0.5)%2)
//				metux_controller_U.current_demand_ext = 1;
//			else
//				metux_controller_U.current_demand_ext = -1;
		}
		else
			metux_controller_U.Activate =  false;
//		if ((micros() - start_flag) < 1 * 1e6)
//			metux_controller_U.current_demand_ext = 0;
//		else if ((micros()-start_flag) < (2 * 1e6))
//			metux_controller_U.current_demand_ext = 0.1;
//		else if ((micros()-start_flag) < (3 * 1e6))
//			metux_controller_U.current_demand_ext = 0.2;
//		else if ((micros()-start_flag) < (4 * 1e6))
//			metux_controller_U.current_demand_ext = 0.3;
//		else if ((micros()-start_flag) < (5 * 1e6))
//			metux_controller_U.current_demand_ext = 0.4;
//		else if ((micros()-start_flag) < (6 * 1e6))
//			metux_controller_U.current_demand_ext = 0.5;
//		else if ((micros()-start_flag) < (7 * 1e6))
//			metux_controller_U.current_demand_ext = 0.6;
//		else if ((micros()-start_flag) < (8 * 1e6))
//			metux_controller_U.current_demand_ext = 0.7;
//		else if ((micros()-start_flag) < (9 * 1e6))
//			metux_controller_U.current_demand_ext = 0.8;
//		else {
//			metux_controller_U.current_demand_ext = 0;
//			metux_controller_U.Activate = 0;
//		}

		metux_controller_step();
		motors[0].setSpeed(metux_controller_Y.MotorDuty);
//		if ((micros()-start_flag) > (5.5 * 1e6)){
//			for (uint8_t i = 0; i < 4; i++)
//						motors[i].setSpeed(0);
		//}
//					metux_controller_U.ExternalSetpoint[0] = 0;
//		controller_delay = micros()-delay_flag;
//		delay_flag = micros();
	}
}


void safetyConnectorTask(void *pvParameters){
	osDelay(10000);
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
	xTaskCreate(motorTask, "Motor Task", 128, NULL, 5, &motorTaskHandle);
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

	for(uint8_t i = 0; i < 4; i++){
		motors[i].setPositionDegree(0*360.0);
	}

	for(uint8_t i=0; i<4; i++){
//	encoderReaderInit(encoder[i],)
	}

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

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	static BaseType_t xHigherPriorityTaskWoken;
	if (hadc->Instance == ADC1){
		// TODO: HAL_ADC_Start_DMA function should only run in app start once.
		// HAL_ADC_Start_DMA(&hadc1, (uint32_t *)PSValues, MAX_PS_COUNT);
		xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(psTaskHandle, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
	else if (hadc->Instance == ADC2) {
		// HAL_ADC_Start_DMA(&hadc2, (uint32_t *)EncoderValues, ADC2_CH_COUNT*ADC2_BUF_LEN);
		xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(motorTaskHandle, &xHigherPriorityTaskWoken);
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
