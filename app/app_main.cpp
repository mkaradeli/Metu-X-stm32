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
#include "i2c.h"
#include "ring_buffer.h"
#include <string.h>
#include "task_timer.h"
#include "PressureSensor.hpp"
#include "globals.hpp"
#include "Button.hpp"
#include "string.h"
#include "UserTask.hpp"
#include "MissionControl.hpp"

#include "BNO085.hpp"

BNO085 imu;


#define FRACTIONAL(x) int(floor(int((x)*100)))%100

#define PI 3.1415926536f

// TODO: PS sensor 3 encodere yakin olan
// TODO: PS sensor 0 alttaki


// TODO: islemci acilma sirasinda akim sensorlerini sifirlayacak.


volatile uint64_t cpuTicks_overflow=0;
float load;
uint32_t local_sensor_data_dropped=0;


void current_adc_complete();
void encoder_adc_complete();
void pressure_adc_complete();

task_timer_t test_point_gpio = {5000,0};
task_timer_t button_task = {1,0};
task_timer_t IMU_task = {1,0};

__attribute__((section(".ramd2"), used))
volatile uint16_t adc_dma_buf_current[4];
__attribute__((section(".ramd2"), used))
volatile uint16_t adc_dma_buf_encoder[8*4];
__attribute__((section(".ramd2"), used))
volatile uint16_t adc_dma_buf_pressure[5];

uint32_t timeOfLastPrint=uwTick;


DebouncedButton Button{1};
volatile float load_filtered;
uint16_t crc16_calc(const uint8_t *p, size_t n);
bool mount_ok;
bool file_creation_ok;
float time_sec = 0;
int load_cell_counter = 0;
uint64_t start_flag;

//controller::current currentController;

SensorData_t local_sensor_data{'K','D'};
SensorData_t local_sensor_data_crc{};
volatile bool local_sensor_data_ready = false; // data state for crc checksum calculation.


//Motor motor1(1, true, LEFT_EN_1_GPIO_Port, LEFT_EN_1_Pin, RIGHT_EN_1_GPIO_Port, RIGHT_EN_1_Pin, &htim1, TIM_CHANNEL_1);
LowPass load_lpf{1.0f, 1000,1};  // 30 Hz cutoff @ 1 kHz sample rate


Profiler free_profiler;
Profiler load_cell_profiler;
Profiler main_loop_profiler;
Profiler printf_profiler;
Profiler adc1_profiler;
Profiler adc2_profiler;
Profiler adc3_profiler;
Profiler tim2_profiler;
Profiler tim3_profiler;
Profiler tim4_profiler;
Profiler button_profiler;
Profiler crc_profiler;
Profiler IMU_profiler;

bool True = true;
MissionControl missionControl(&True,
			&logData.record);



void app_init() {
	SensorData_Buffer_Init(&logData);
//	Button.DebouncedButton(1);
	logData.ready=false;
//	logData.record = false;
//	SensorData_Buffer_Init(&logData_axiram);
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

//	  actuator[0].setDuty(1.0f);
//	  motors[0].setDuty(1.0f);

	  if (!imu.begin(&hi2c1)) {          // 0x4A default, pass 0x4B if SA0 high
//	          Error_Handler();
	      }
	  imu.enableReport(SH2_GAME_ROTATION_VECTOR, 2500);   // 400 Hz
	  imu.enableReport(SH2_LINEAR_ACCELERATION,  2500);
//	  dummy_init();
//	  printf(CLR_SCREEN);

//	    local_sensor_data.current_demand = 2;
//	    local_sensor_data.timestamp = 0;
//	    ready_to_write_a=0;

	    main_loop_profiler.reset();
	    printf_profiler.reset();
	    free_profiler.reset();
	    adc1_profiler.reset();
	    adc2_profiler.reset();
	    adc3_profiler.reset();
	    tim2_profiler.reset();
	    tim3_profiler.reset();
	    tim4_profiler.reset();
	    button_profiler.reset();
	    crc_profiler.reset();
	    IMU_profiler.reset();

	    rb_init(&common_print_buffer);
	    setvbuf(stdout, NULL, _IONBF, 0);   /* important: disable stdio line buffering */

//	    SCB_InvalidateDCache_by_Addr((uint32_t *)adc_dma_buf_encoder, sizeof(adc_dma_buf_encoder));
	    HAL_Delay(10);
	    for (int i=0; i<4; i++)
	    	actuator[i].calibrate();
//	    currentController.initialize();
	    Actuator::manifold->calibrate();

//	    if (mount_ok and file_creation_ok)
//			controller_mode = controller_modes::PRESSURE;

		for (int i= 0; i< 4; i++) {
			actuator[i].actuatorController.rtU.pos_feedback = actuator[i].hallEffect.valveAngle;
			actuator[i].actuatorController.rtU.SpeedFeedback = actuator[i].hallEffect.valveVelocity;
			actuator[i].actuatorController.rtU.P_manifold = Actuator::manifold->getPsi();
			actuator[i].actuatorController.rtU.P_nozzle_demand = 0.0F;

		}

		start_flag = cpuTicks();
//		actuator[0].static_manifold = &psSensors[4];
		for (int i=0; i<4; i++)
			actuator[i].setDuty(1);
	free_profiler.start();
	missionControl.Init(taskFunction, shutdownFunction);
	missionControl.actuator_mode_desired = actuator_mode_desired;
	missionControl.system_mode_desired = system_modes::TESTFIRE;
	missionControl.ops_duration_ms = ops_duration_ms;
	missionControl.shutdown_duration_ms = shutdown_duration_ms;
	missionControl.postShutdownWait_ms = postShutdownWait_ms;
//	const uint8_t tv[] = "123456789";
//		uint16_t crc = crc16_calc(tv, 9);
//	controller_mode = controller_modes::POSITION;
}
extern "C" {
extern bool pc8_active;

}
bool selfTrigger = false;
void app_loop() {
	if (task_ready(&button_task)) {
		Button.update();

		if (Button.pressed()) {
			button_profiler.start();
			missionControl.Toggle();
		} else {
			if (selfTrigger and uwTick==10000) {
				selfTrigger = false;
				missionControl.Toggle();
			}
		button_profiler.end();
		}
	}


	if (local_sensor_data_ready){
		crc_profiler.start();
		local_sensor_data_crc.crc = crc16_calc((uint8_t *)&local_sensor_data_crc, sizeof(SensorData_t)-2);
		crc_profiler.end();
		// sensor data paketlendi ve crc hesaplamasi yapilip cm4 e gondeilecek

		SensorData_Buffer_Push(&logData, &local_sensor_data_crc);
		local_sensor_data_ready = false;
	}


	if (uwTick - timeOfLastPrint >= 1000){
		main_loop_profiler.start();

		timeOfLastPrint+= 1000;
		printf(CLR_SCREEN);
		printf("timestamp = %ld, %ld \n\r", uwTick, cpuTicks());

#ifdef SHOW_INTERRUPT_TIMER_COUNTERS
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
		printf("timestamp = %ld, %ld \n\r", uwTick, cpuTicks());

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

#endif
#ifndef PROFILER_RESULTS
		printf("free_profiler cpu usage = %%%d.%d\n\r",(int)free_profiler.cpu_usage,FRACTIONAL(free_profiler.cpu_usage));
		printf("load_cell_profiler cpu usage = %%%d.%d\n\r",(int)load_cell_profiler.cpu_usage,FRACTIONAL(load_cell_profiler.cpu_usage));
		printf("main_loop_profiler cpu usage = %%%d.%d\n\r",(int)main_loop_profiler.cpu_usage,FRACTIONAL(main_loop_profiler.cpu_usage));
		printf("printf_profiler cpu usage = %%%d.%d\n\r",(int)printf_profiler.cpu_usage,FRACTIONAL(printf_profiler.cpu_usage));
		printf("adc1_profiler cpu usage = %%%d.%d\n\r",(int)adc1_profiler.cpu_usage,FRACTIONAL(adc1_profiler.cpu_usage));
		printf("adc2_profiler cpu usage = %%%d.%d\n\r",(int)adc2_profiler.cpu_usage,FRACTIONAL(adc2_profiler.cpu_usage));
		printf("adc3_profiler cpu usage = %%%d.%d\n\r",(int)adc3_profiler.cpu_usage,FRACTIONAL(adc3_profiler.cpu_usage));
		printf("tim2_profiler cpu usage = %%%d.%d\n\r",(int)tim2_profiler.cpu_usage,FRACTIONAL(tim2_profiler.cpu_usage));
		printf("tim3_profiler cpu usage = %%%d.%d\n\r",(int)tim3_profiler.cpu_usage,FRACTIONAL(tim3_profiler.cpu_usage));
		printf("tim4_profiler cpu usage = %%%d.%d\n\r",(int)tim4_profiler.cpu_usage,FRACTIONAL(tim4_profiler.cpu_usage));
		printf("button_profiler cpu usage = %%%d.%d\n\r",(int)button_profiler.cpu_usage,FRACTIONAL(button_profiler.cpu_usage));
		printf("crc_profiler cpu usage = %%%d.%d\n\r",(int)crc_profiler.cpu_usage,FRACTIONAL(crc_profiler.cpu_usage));
		printf("IMU_profiler cpu usage = %%%d.%d\n\r",(int)IMU_profiler.cpu_usage,FRACTIONAL(IMU_profiler.cpu_usage));
		float total_usage = main_loop_profiler.cpu_usage
		+ free_profiler.cpu_usage
		+ printf_profiler.cpu_usage
		+ adc1_profiler.cpu_usage
		+ adc2_profiler.cpu_usage
		+ adc3_profiler.cpu_usage
		+ tim2_profiler.cpu_usage
		+ tim3_profiler.cpu_usage
		+ tim4_profiler.cpu_usage
		+ button_profiler.cpu_usage
		+ load_cell_profiler.cpu_usage
		+ crc_profiler.cpu_usage
		+ IMU_profiler.cpu_usage;
		printf("angle = %d, cur = %d\n\r", int(actuator[0].hallEffect.valveAngle), int(actuator[0].get_current()));
		printf("total cpu usage accounted %%%d.%d \n\r", (int)total_usage, FRACTIONAL(total_usage));
#endif
		printf("ACTUATOR 1\n\r");
		printf("current = %d.%d A\n\r",(int)actuator[0].get_current(), FRACTIONAL(actuator[0].get_current()));
		printf("encoder = %d.%d deg\n\r",(int)actuator[0].hallEffect.valveAngleKalman, FRACTIONAL(actuator[0].hallEffect.valveAngleKalman));
		printf("Nozzle Pressure = %d.%d\n\r", (int)actuator[0].psSensor->getBar(), FRACTIONAL(actuator[0].psSensor->getBar()));
		printf("Manifold Pressure = %d.%d\n\r", (int)Actuator::manifold->getBar(), FRACTIONAL(Actuator::manifold->getBar()) );
		printf("measured weight = %d.%d\n\r", (int)loadCell.weight_kg_filtered, FRACTIONAL(loadCell.weight_kg_filtered));

		printf("logData head = %ld, tail = %ld, written =%ld, dropped = %ld\n\r", logData.head, logData.tail, logData.written, logData.dropped);
		printf("\n\r\n\r");


		main_loop_profiler.metrics();
		printf_profiler.metrics();
		adc1_profiler.metrics();
		adc2_profiler.metrics();
		adc3_profiler.metrics();
		tim2_profiler.metrics();
		tim3_profiler.metrics();
		tim4_profiler.metrics();
		free_profiler.metrics();
		button_profiler.metrics();
		crc_profiler.metrics();
		IMU_profiler.metrics();
		main_loop_profiler.end();


	}

	if (task_ready(&IMU_task)) {
		IMU_profiler.start();
		imu.service();                 // poll at least every ~1 ms

		if (imu.hasNewQuaternion()) {
			float w = imu.quaternion.real;   // i, j, k likewise
		}
		if (imu.hasNewAccel()) {
			float ax = imu.linearAccel.x;
		}
		IMU_profiler.end();
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
	cpuTicks_overflow++;
//	tim2_profiler.end();
}


void current_adc_complete(){
	adc2_profiler.start();
	for (int i=0; i<4; i++){
		actuator[i].updateCurrent(adc_dma_buf_current[i]);
	}
	for (int i=0; i<4; i++)
		actuator[i].current_controller_step();
	adc2_profiler.end();
}

void encoder_adc_complete(){
	adc1_profiler.start();
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
	loadCell.update();

	missionControl.Iter();

	for (int i=0; i<4; i++) {
		actuator[i].actuator_controller_step();
	}

	local_sensor_data.timestamp = cpuTicks()/200; //microseconds

	for (int j=0;j<4;j++){
		local_sensor_data.actuatorData[j].current_measured = actuator[j].get_current();
		local_sensor_data.actuatorData[j].current_demand = actuator[j].actuatorController.rtY.currentDemand;
		local_sensor_data.actuatorData[j].speedDemand = actuator[j].actuatorController.rtY.speedDemand;
		local_sensor_data.actuatorData[j].pos_ref = actuator[j].actuatorController.rtY.position_demand;
		local_sensor_data.actuatorData[j].pos_ref_rate_limited = actuator[j].actuatorController.rtY.pos_ref_rate_limited;
		local_sensor_data.actuatorData[j].speed_ref_rate_limited = actuator[j].actuatorController.rtY.speedDemand;
		local_sensor_data.actuatorData[j].pressure_demand = actuator[j].actuatorController.rtU.P_nozzle_demand;
		local_sensor_data.actuatorData[j].valveAngle = actuator[j].hallEffect.valveAngle;
		local_sensor_data.actuatorData[j].valveAngleKalman = actuator[j].hallEffect.valveAngleKalman;
		local_sensor_data.actuatorData[j].valveVelocity = actuator[j].hallEffect.valveVelocity;
		local_sensor_data.actuatorData[j].duty = actuator[j].getDutyCycle();

		local_sensor_data.actuatorData[j].nozzle_pressure = actuator[j].getPressurePsi();
		local_sensor_data.actuatorData[j].nozzle_raw = *actuator[j].psSensor->raw_value;
		local_sensor_data.actuatorData[j].thrust_demand = 0;
		local_sensor_data.actuatorData[j].thrust_estimated = 0;

	}
		local_sensor_data.thrust_measured = loadCell.getForce();
		local_sensor_data.thrust_raw = *loadCell.raw_value;
		local_sensor_data.manifold_pressure = Actuator::manifold->getPsi();
		local_sensor_data.manifold_raw = *Actuator::manifold->raw_value;

		if (missionControl.running and logData.ready) {
		if (!local_sensor_data_ready){
			memcpy(&local_sensor_data_crc, &local_sensor_data, sizeof(local_sensor_data));
			local_sensor_data_ready = true;
		}
		else {
			local_sensor_data_dropped++;
		}
	}

	adc3_profiler.end();
}

uint16_t crc16_calc(const uint8_t *p, size_t n)
{
    CRC->CR |= CRC_CR_RESET;               // reload INIT (0xFFFF), self-clears
    while (n--)
        *(volatile uint8_t *)&CRC->DR = *p++;   // STRB: byte-wide feed
    return (uint16_t)CRC->DR;
}
