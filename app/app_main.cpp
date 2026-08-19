/*
 * app_main.cpp
 *
 *  Created on: May 14, 2026
 *      Author: karadeli
 */

#include "globals.hpp"
#define DISABLE_CRC true
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
#include "MissionUart.hpp"

#include "MissionControl.hpp"
//#include "platformController.h"

#include "BNO085.hpp"
#include "sd_task.hpp"
BNO085 imu;


#define FRACTIONAL(x) int(floor(int((x)*100)))%100
#define LOG_TERMINATOR  0x0A0Du   /* LF,CR packed MSB-first */

//#define PI 3.1415926536f

// TODO: PS sensor 3 encodere yakin olan
// TODO: PS sensor 0 alttaki
void LED_Counter_Tick(void);


// TODO: islemci acilma sirasinda akim sensorlerini sifirlayacak.


volatile uint64_t micros_overflow=0;
float load;
//uint32_t local_sensor_data_dropped=0;



void current_adc_complete();
void encoder_adc_complete();
void pressure_adc_complete();

task_timer_t test_point_gpio = {5000,0};
task_timer_t button_task = {1,0};
task_timer_t IMU_task = {1,0};
task_timer_t sd_card_task = {500,0};
task_timer_t printf_task = {1000, 0};
task_timer_t heartbeat_task = {100, 0}; // period ms, start ms

task_timer_t uart_logging = { 2, 0};

__attribute__((section(".RAM_D2_Section"), used))
volatile uint16_t adc_dma_buf_current[4];
__attribute__((section(".RAM_D2_Section"), used))
volatile uint16_t adc_dma_buf_encoder[8*4];
__attribute__((section(".RAM_D2_Section"), used))
volatile uint16_t adc_dma_buf_pressure[5];

uint32_t timeOfLastPrint=uwTick;


DebouncedButton Button{1,BUTTON_USER_GPIO_PORT,BUTTON_USER_PIN};
DebouncedButton SafetyConnector{1,SAFETY_CONNECTOR_GPIO_Port,SAFETY_CONNECTOR_Pin};
volatile float load_filtered;
uint16_t crc16_calc(const uint8_t *p, size_t n);
bool mount_ok;
bool file_creation_ok;
float time_sec = 0;
int load_cell_counter = 0;

SensorData_t local_sensor_data{'K','D'};

void onImuReport(const BNO085& r);
void onLidarFrame(uint16_t distMm, uint16_t strength);

LowPass load_lpf{1.0f, 1000,1};  // 30 Hz cutoff @ 1 kHz sample rate
LowPass accel_interval{1.0f, 1000,1};  // 30 Hz cutoff @ 1 kHz sample rate
LowPass quatIntRV_interval{1.0f,1000,1};

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
Profiler sd_card_profiler;
Profiler tim7_profiler;
Profiler kf_profiler;
Profiler tim12_profiler;


//bool True = true;
//MissionControl missionControl(&True,
//			&logData.record);
MissionControl missionControl(&logData.ready, &logData.record);

//platformController platform_controller;



/* Called by MissionControl the moment a mission is triggered, before
 * log_recording goes true. Wire it to your f_expand pre-allocation. */
//static void logPrepareHook(uint32_t estimated_duration_ms, const char *header)
//{
//    const uint32_t bytes_per_s = 1000u * sizeof(SensorData_t);   // ~1 kHz logging
//    const uint32_t bytes = (estimated_duration_ms / 1000u) * bytes_per_s;
////    sd_log_prepare(bytes, header);      // your sd_task API
//
//}
static uint32_t calStartTick = 0;   // set right after beginCalibration()

void app_init() {
	SensorData_Buffer_Init(&logData);

	for (int i = 0; i<4; i++){
		for (int j=0; j<11; j++){
			actuator[i].actuatorController.rtU.ValveFitPressureRatios[j] = ValveFitPressureRatios[i][j];
		}
		actuator[i].actuatorController.rtU.nozzle_gain = nozzle_gain[i];
	}
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
//	  actuator[0].actuatorController

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
	  HAL_TIM_Base_Start_IT(&htim12);  /* _IT = interrupt ile */

	  HAL_TIM_Base_Start(&htim1);  /* _IT = interrupt ile */

	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
      BSP_LED_On(LED_RED);

//	  actuator[0].setDuty(1.0f);
//	  motors[0].setDuty(1.0f);
      p.lever[2]   = 0.06f;
      p.sigmaAccel = 0.30f;
      g_altEst.configure(p);
	  if (!imu.begin(&hi2c1)) {          // 0x4A default, pass 0x4B if SA0 high
//	          Error_Handler();
	      }
	  imu.enableReport(SH2_ACCELEROMETER,  2500);
	  imu.enableReport(SH2_GYRO_INTEGRATED_RV, 2500);   // 400 Hz
//	  imu.enableReport(SH2_GAME_ROTATION_VECTOR,2500);

		lidar.Reset();
//		altEstimator.begin();
		HAL_UARTEx_ReceiveToIdle_DMA(&huart6, lidar.getBuffer(), 128);
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
	    sd_card_profiler.reset();
	    tim7_profiler.reset();
	    tim12_profiler.reset();
	    kf_profiler.reset();



//	    SCB_InvalidateDCache_by_Addr((uint32_t *)adc_dma_buf_encoder, sizeof(adc_dma_buf_encoder));
	    HAL_Delay(10);
	  HAL_TIM_Base_Start_IT(&htim7);  /* _IT = interrupt ile */
	    for (int i=0; i<4; i++)
	    	actuator[i].calibrate();
//	    currentController.initialize();
//	    Actuator::manifold->calibrate();
	    platform_controller.initialize();

//	    if (mount_ok and file_creation_ok)
//			controller_mode = controller_modes::PRESSURE;

		for (int i= 0; i< 4; i++) {
			actuator[i].actuatorController.rtU.pos_feedback = actuator[i].hallEffect.valveAngle;
			actuator[i].actuatorController.rtU.SpeedFeedback = actuator[i].hallEffect.valveVelocity;
			actuator[i].actuatorController.rtU.P_manifold = Actuator::manifold->getPsi();
			actuator[i].actuatorController.rtU.P_nozzle_demand = 0.0F;

		}



//		start_flag = micros();
//		actuator[0].static_manifold = &psSensors[4];

	free_profiler.start();
//	missionControl.Init(taskFunction, shutdownFunction);
//	missionControl.actuator_mode_desired = actuator_mode_desired;
//	missionControl.system_mode_desired = system_modes::TESTFIRE;
//	missionControl.ops_duration_ms = ops_duration_ms;
//	missionControl.shutdown_duration_ms = shutdown_duration_ms;
//	missionControl.postShutdownWait_ms = postShutdownWait_ms;

    missionControl.Init();        // or Init();
    missionControl.Select(defaultMissionIndex);
    mission_uart_init(&huart3);       // whichever UART your ground link is on
    g_altEst.beginCalibration();
    calStartTick = uwTick;

//	const uint8_t tv[] = "123456789";
//		uint16_t crc = crc16_calc(tv, 9);
//	controller_mode = controller_modes::POSITION;
}
extern "C" {
extern bool pc8_active;

}
bool selfTrigger = false;

bool altitudeEstimatorDone = false;
void app_loop() {

	if (!altitudeEstimatorDone && (uwTick - calStartTick) >= 1500) {
	    if (g_altEst.finishCalibration()) {
	        altitudeEstimatorDone = true;
	    } else {
	        calStartTick = uwTick;      // not enough samples yet, keep collecting
	    }
	}
//	if (task_ready(&button_task)) { // 1ms
//		Button.update();
//
//		if (Button.pressed()) {
//			button_profiler.start();
//			missionControl.Toggle();
//		} else {
//			if (selfTrigger and uwTick==10000) {
//				selfTrigger = false;
//				missionControl.Toggle();
//			}
//		button_profiler.end();
//		}
//	}


	if (uwTick - timeOfLastPrint >= 1000){
		main_loop_profiler.start();

		timeOfLastPrint+= 1000;
		printf(CLR_SCREEN);
		printf("timestamp = %ld \n\r", uwTick);

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
		printf("sd_card_profiler cpu usage = %%%d.%d\n\r",(int)sd_card_profiler.cpu_usage,FRACTIONAL(sd_card_profiler.cpu_usage));
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
		+ IMU_profiler.cpu_usage +sd_card_profiler.cpu_usage;
		printf("angle = %d, cur = %d\n\r", int(actuator[0].hallEffect.valveAngle), int(actuator[0].get_current()));
		printf("total cpu usage accounted %%%d.%d \n\r", (int)total_usage, FRACTIONAL(total_usage));
#endif
		printf("ACTUATOR 1\n\r");
		printf("current = %d.%d A\n\r",(int)actuator[0].get_current(), FRACTIONAL(actuator[0].get_current()));
		printf("encoder = %d.%d deg\n\r",(int)actuator[0].hallEffect.valveAngleKalman, FRACTIONAL(actuator[0].hallEffect.valveAngleKalman));
		printf("Nozzle Pressure = %d.%d\n\r", (int)actuator[0].psSensor->getBar(), FRACTIONAL(actuator[0].psSensor->getBar()));
		printf("Manifold Pressure = %d.%d\n\r", (int)Actuator::manifold->getBar(), FRACTIONAL(Actuator::manifold->getBar()) );
		printf("measured weight = %d.%d\n\r", (int)loadCell.weight_kg_filtered, FRACTIONAL(loadCell.weight_kg_filtered));

		printf("logData wr=%lu half=[%u,%u] written=%lu dropped=%lu\n\r",
		       logData.write_idx, logData.half_full[0], logData.half_full[1],
		       logData.written, logData.dropped);		printf("\n\r\n\r");
//		printf("%d, %d, %d, \n\r", imu.quaternion.i, imu.quaternion.j, imu.quaternion.k);

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
		sd_card_profiler.metrics();
		kf_profiler.metrics();
		tim7_profiler.metrics();
		tim12_profiler.metrics();
		main_loop_profiler.end();


	}
//	if (task_ready(&printf_task)) { // 1000 ms
//		printf_profiler.start();
//		printf("%ld %ld , %ld\n\r", rb_count(&common_print_buffer), common_print_buffer.head, common_print_buffer.tail);
//		printf("uwTick = %ld \n\r",uwTick);
//		rb_flush();
//		printf_profiler.end();
//	  }

//	if (task_ready(&IMU_task)) { // 1 ms
//		IMU_profiler.start();
//		imu.service();                 // poll at least every ~1 ms
//		IMU_profiler.end();
//	}
	if (task_ready(&sd_card_task)) { // 500 ms
			sd_card_prep();
		}
	sd_card_task_function(); // every iter
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

void tim7_trigger() { // 1 khz low priority
	tim7_profiler.start();
	if (true /*task_ready(&button_task)*/) { // 1ms
		Button.update();

		if (Button.pressed()) {
			button_profiler.start();
            missionControl.RequestToggle();     // ISR-safe, serviced in Iter()
		} else {
			if (selfTrigger and uwTick==10000) {
				selfTrigger = false;
	            missionControl.RequestToggle();     // ISR-safe, serviced in Iter()
			}
			button_profiler.end();
		}
	}
//	{ // safety connector
//		if (SafetyConnector.pressed()) {
//			missionControl.Toggle();
//		}
//	}
    mission_uart_poll();              // one DMA counter read when idle



	rb_flush();
	if (task_ready(&IMU_task)) { // 1 ms
		IMU_profiler.start();
		imu.service();                 // poll at least every ~1 ms
		IMU_profiler.end();
	}
	accel_interval.update(imu.accelInterval_us);
	quatIntRV_interval.update(imu.gyroIntegratedRVInterval_us);

    kf_profiler.start();
    {
        // Evaluate both flags separately: && would short-circuit and leave one
        // uncleared. Both are consume-on-read.
        const bool newQuat  = imu.hasNewQuaternion();
        const bool newAccel = imu.hasNewAccel();
        (void)newQuat;
        if (newAccel) {
        	onImuReport(imu);
        }

    }
//    float R[3][3]; quatToR(q, R);
	if(lidar.hasNewReading()) {

		onLidarFrame(lidar.getDistance(), lidar.getStrength());
	}
//    float aw[3];
//    for (int i = 0; i < 3; ++i)
//        aw[i] = R[i][0]*a_b[0] + R[i][1]*a_b[1] + R[i][2]*a_b[2];
    kf_profiler.end();


    if (task_ready(&heartbeat_task))
    	  		LED_Counter_Tick();
//    missionControl.HandleCommand(rx_line, reply, sizeof(reply))
    // TODO: recive handling

	tim7_profiler.end();
}
struct ExtY {
//  real32_T currentDemand;          // '<Root>/currentDemand'
//  real32_T speedDemand;            // '<Root>/speedDemand'
//  real32_T position_demand;        // '<Root>/position_demand'
//  real32_T pos_ref_rate_limited;   // '<Root>/pos_ref_rate_limited'
  real32_T P_nozzle_demand;        // '<Root>/P_nozzle_demand'
  real32_T ThrustMax;              // '<Root>/ThrustMax'
  real32_T ThrustEstimate;         // '<Root>/ThrustEstimate'
};
void tim12_trigger(){ // mid priority 50hz platform control task
	tim12_profiler.start();
//	platform_controller.rtU.Height = altEstimator.altitude(); // m
//	platform_controller.rtU.Velocity = altEstimator.velocity(); // m/s
	platform_controller.rtU.ManifoldPressure = Actuator::manifold->getBar(); // bar
	platform_controller.rtU.quaternion[0] = imu.gyroIntegratedRV.i;
	platform_controller.rtU.quaternion[1] = imu.gyroIntegratedRV.j;
	platform_controller.rtU.quaternion[2] = imu.gyroIntegratedRV.k;
	platform_controller.rtU.quaternion[3] = imu.gyroIntegratedRV.real;
	platform_controller.rtU.Height = g_altEst.height();
	platform_controller.rtU.Velocity = g_altEst.velocity();
//	platform_controller.rtU.T_alloc_total = T_alloc_total;


	float T_alloc_total  = 0;
	for (int i=0; i<4; i++) T_alloc_total += actuator[i].actuatorController.rtY.ThrustEstimate;
	float T_max_allowed  = 0;
	for (int i=0; i<4; i++) T_max_allowed += actuator[i].actuatorController.rtY.ThrustMax;

	platform_controller.rtU.T_alloc_total = T_alloc_total;
	platform_controller.rtU.T_max_allowed = T_max_allowed;

	// TODO: add drop, and force relationships.
//	platform_controller.rtU.Dropped = ;
	platform_controller.step();
	tim12_profiler.end();
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
//	loadCell.update();

    missionControl.Iter();

//    altEstimator.setClamped(!missionControl.firing);   // was: !missionControl.running

//	altEstimator.setClamped(!missionControl.running);

	for (int i=0; i<4; i++) {
		actuator[i].actuator_controller_step();
	}
	// External inputs (root inport signals with default storage)
//	struct ExtU {
	//  real32_T P_manifold;             // '<Root>/P_manifold'
//	  real32_T P_nozzle_demand_ext;    // '<Root>/P_nozzle_demand_ext'
	//  real32_T P_nozzle;               // '<Root>/P_nozzle'
	//  real32_T SpeedFeedback;          // '<Root>/speed_feedback'
//	  real32_T pos_ref_ext;            // '<Root>/pos_ref_ext'
	//  real32_T pos_feedback;           // '<Root>/pos_feedback'
//	  real32_T speedDemand_ext;        // '<Root>/speedDemand_ext'
	//  real32_T ValveFitPressureRatios[11];// '<Root>/ValveFitPressureRatios'
//	  real32_T F_demand;               // '<Root>/F_demand'
	//  real32_T nozzle_gain;            // '<Root>/nozzle_gain'
//	};/

	// External outputs (root outports fed by signals with default storage)


	local_sensor_data.timestamp = micros(); //microseconds

	for (int j=0;j<4;j++){
		local_sensor_data.actuatorData[j].current_measured = actuator[j].get_current();
		local_sensor_data.actuatorData[j].current_demand = actuator[j].actuatorController.rtY.currentDemand;
		local_sensor_data.actuatorData[j].speedDemand = actuator[j].actuatorController.rtY.speedDemand;
		local_sensor_data.actuatorData[j].pos_ref = actuator[j].actuatorController.rtY.position_demand;
		local_sensor_data.actuatorData[j].pos_ref_rate_limited = actuator[j].actuatorController.rtY.pos_ref_rate_limited;
		local_sensor_data.actuatorData[j].speed_ref_rate_limited = actuator[j].actuatorController.rtY.speedDemand;
		local_sensor_data.actuatorData[j].pressure_demand = actuator[j].actuatorController.rtY.P_nozzle_demand1;
		local_sensor_data.actuatorData[j].valveAngle = actuator[j].hallEffect.valveAngle;
		local_sensor_data.actuatorData[j].valveAngleKalman = actuator[j].hallEffect.valveAngleKalman;
		local_sensor_data.actuatorData[j].valveVelocity = actuator[j].hallEffect.valveVelocity;
		local_sensor_data.actuatorData[j].duty = actuator[j].getDutyCycle();

		local_sensor_data.actuatorData[j].nozzle_pressure = actuator[j].getPressurePsi();
		local_sensor_data.actuatorData[j].nozzle_raw = *actuator[j].psSensor->raw_value;
		local_sensor_data.actuatorData[j].thrust_demand = actuator[j].actuatorController.rtU.F_demand;
		local_sensor_data.actuatorData[j].thrust_estimated = actuator[j].actuatorController.rtY.ThrustEstimate;

	}
//		local_sensor_data.thrust_measured = loadCell.getForce();
//		local_sensor_data.thrust_raw = *loadCell.raw_value;
		local_sensor_data.manifold_pressure = Actuator::manifold->getPsi();
		local_sensor_data.manifold_raw = *Actuator::manifold->raw_value;
		local_sensor_data.linearAccel = imu.accel;
		local_sensor_data.quaternion.i = imu.gyroIntegratedRV.i;
		local_sensor_data.quaternion.j = imu.gyroIntegratedRV.j;
		local_sensor_data.quaternion.k = imu.gyroIntegratedRV.k;
		local_sensor_data.quaternion.real = imu.gyroIntegratedRV.real;
		local_sensor_data.lidarDistance = lidar.getDistance();
		local_sensor_data.lidarStrength = lidar.getStrength();

		local_sensor_data.kf_altitude = g_altEst.height();
		local_sensor_data.kf_velocity = g_altEst.velocity();
//        float    innovation          = 0.0f;  /* last y = z - h        [m] */
//        float    nis                 = 0.0f;  /* last y^2 / S          [-] */
//        float    cosTilt             = 1.0f;  /* vertical fraction     [-] */
//        uint32_t lidarAccepted       = 0;
//        uint32_t lidarRejected       = 0;     /* gate or tilt failures     */
//        uint32_t lidarBlocksDropped  = 0;     /* too few valid raw frames  */
//        uint32_t consecutiveRejects  = 0;
//        bool     lastUpdateAccepted  = false;

		AltitudeEstimator::Status s = g_altEst.status();
//        local_sensor_data.kf_altitude   = altEstimator.altitude();
//        local_sensor_data.kf_velocity   = altEstimator.velocity();
//        local_sensor_data.kf_accelBias  = altEstimator.accelBias();
        local_sensor_data.kf_sigmaH     = g_altEst.heightSigma();
        local_sensor_data.kf_sigmaV     = g_altEst.velocitySigma();
        local_sensor_data.kf_meanNis    = s.nis;
        local_sensor_data.kf_tiltCos    = s.cosTilt;
        local_sensor_data.kf_rejects    = s.consecutiveRejects;
//        local_sensor_data.kf_flags      = (uint8_t)altEstimator.phase()
//                                        | (uint8_t)(altEstimator.lastResult() << 4);

		if(task_ready(&uart_logging)){
			rb_write(&common_print_buffer, &local_sensor_data, (size_t)sizeof(SensorData_t));
		}

		if (missionControl.running and logData.ready) {
		    SensorData_t *slot = SensorData_Buffer_Reserve(&logData);
		if (slot){
			*slot = local_sensor_data;
			crc_profiler.start();
			slot->crc = crc16_calc((uint8_t*)slot, sizeof(SensorData_t) - 2);
			crc_profiler.end();
			SensorData_Buffer_Commit(&logData);
		}
	}

	adc3_profiler.end();
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
	if (huart == &huart6){
//		lidar_profiler.start();
//		printf("aaaa lidar frame captured!\n\r");
		lidar.FrameHandler(Size);
//		if (lidar.hasNewReading()) altEstimator.onLidarFrame(lidar.getDistance(), lidar.getStrength());

		HAL_UARTEx_ReceiveToIdle_DMA(&huart6, lidar.getBuffer(), 128);  // re-arm!
//		lidar_profiler.end();
	}
}
void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart) {
    if (huart == &huart6){
        HAL_UARTEx_ReceiveToIdle_DMA(&huart6, lidar.getBuffer(), 128);  // recover
    }
    if (huart == s_huart) {
        rb_tx_error_isr();
    }
    mission_uart_error(huart);        // no-op unless it is the command UART
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == s_huart) {
		rb_tx_complete_isr();
	}
//	imu.quaternionInterval_us
}

static uint32_t lastUs = 0;
void onImuReport(const BNO085& r) {
    const uint32_t now = imu.accelInterval_us;
    const float dt = (lastUs == 0) ? 0.0f : (now - lastUs) * 1e-6f;
    lastUs = now;

    const float q[4] = { r.gyroIntegratedRV.real, r.gyroIntegratedRV.i, r.gyroIntegratedRV.j, r.gyroIntegratedRV.k };
    const float a[3] = { r.accel.x, r.accel.y, r.accel.z };
//    const float dt   = ( - lastTs_) * 1e-6f;
//    lastTs_ = r.quaternionInterval_us;
    g_altEst.pushImu(a, q, dt);
}
void onLidarFrame(uint16_t distMm, uint16_t strength) {
    const float q[4] = { imu.gyroIntegratedRV.real, imu.gyroIntegratedRV.i,
                         imu.gyroIntegratedRV.j,    imu.gyroIntegratedRV.k };
//    g_altEst.pushLidarFrame(distMm, strength, q);
    g_altEst.pushLidarFrame(distMm, strength, q);
}
void LED_Counter_Tick(void)
{
	static const uint8_t timing[] = {1, 0, 1, 0, 0, 0, 0};
	static const uint8_t timing_logging[]= {1, 0, 1, 0, 1, 0, 0};
	static uint8_t index = 0;
	if (logData.record){
		if (timing_logging[index])
			BSP_LED_On(LED_GREEN);
		else
			BSP_LED_Off(LED_GREEN);
		}
	else {
		if (timing[index])
			BSP_LED_On(LED_GREEN);
		else
			BSP_LED_Off(LED_GREEN);
	}



	index ++;
	index %= 7;
}

uint16_t crc16_calc(const uint8_t *p, size_t n)
{
#ifdef DISABLE_CRC
	return LOG_TERMINATOR;
//	return (uint16_t) '\n\r';
#else
    CRC->CR |= CRC_CR_RESET;               // reload INIT (0xFFFF), self-clears
    while (n--)
        *(volatile uint8_t *)&CRC->DR = *p++;   // STRB: byte-wide feed
    return (uint16_t)CRC->DR;
#endif
}
