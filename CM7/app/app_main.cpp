/*
 * app_main.cpp
 *
 *  Created on: May 14, 2026
 *      Author: karadeli
 */

#include "globals.hpp"
#define DISABLE_CRC true
#define CHECK_TIMER_FREQUENCIES false


#include <shared_memory.h>
#include "app_main.hpp"
#include "main.h"
#include "i2c.h"
#include "spi.h"
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
#include "SdCard/sd_task.hpp"
//#include "SdCardRaw.hpp"
#include "nrf24.h"
#include "nrf24_link.h"
#include "nrf24_port.h"
BNO085 imu;

/* Plain (non-static) mirrors of the driver's internal counters, refreshed
 * every tick in tim7_trigger() - watch these in Live Expressions while
 * running. nrf24.c and nrf24_link.c each keep their own stats in a static
 * s_stats, same name in both files, which Live Expressions can't address
 * unambiguously by name; these globals sidestep that. */
nrf24_stats_t      nrf24_radio_stats;   /* per-fragment: tx_ok, tx_dropped, tx_timeout, rx_ok, retransmits, lost_packets */
nrf24_link_stats_t nrf24_frame_stats;   /* per-frame: frames_sent, frames_dropped, frags_sent, frames_rx, crc_errors, reassembly_drops */
uint32_t           nrf24_uplink_rx_count = 0u;   /* ACK-payload uplink packets drained, see tim7_trigger() */


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
task_timer_t printf_task = {1, 0};
task_timer_t heartbeat_task = {100, 0}; // period ms, start ms

task_timer_t uart_logging = { 2, 0};
task_timer_t nrf24_tx_task = {20, 0};   // 50 Hz downlink

__attribute__((section(".sram3"), used))
volatile uint16_t adc_dma_buf_current[5];
__attribute__((section(".sram3"), used))
volatile uint16_t adc_dma_buf_encoder[8*4];
__attribute__((section(".sram3"), used))
volatile uint16_t adc_dma_buf_pressure[6];

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
Profiler nrf24_profiler;
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
	  HAL_ADC_Start_DMA(&hadc2, (uint32_t*)adc_dma_buf_current, 5);

	  HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
	  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_dma_buf_encoder, 32);

	  HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
	  HAL_ADC_Start_DMA(&hadc3, (uint32_t*)adc_dma_buf_pressure, 6);


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
	  if (!imu.begin(&hspi1)) {
//	          Error_Handler();
	      }
//	  imu.enableReport(SH2_RAW_ACCELEROMETER,  2500);
	  imu.enableReport(SH2_GYRO_INTEGRATED_RV, 2500);   // 400 Hz
	  imu.enableReport(SH2_ACCELEROMETER, 5000);   // 200 Hz

//	  imu.enableReport(SH2_GAME_ROTATION_VECTOR,2500);

	  /* nRF24 telemetry: downlinks local_sensor_data at 50 Hz, see
	   * nrf24_tx_task (app_loop) and nrf24_link_service() (tim7_trigger,
	   * 1 kHz). Auto-ack stays on so ground -> rocket commands can ride
	   * back on the ACK payload; ch/addr/data-rate must match the ground
	   * station config (see conversation history on that sync). */
	  {
	      nrf24_cfg_t nrf24_cfg;
	      nrf24_default_cfg(&nrf24_cfg);
	      nrf24_cfg.data_rate = NRF24_DR_2M;
	      nrf24_status_t nrf24_st = nrf24_init(&nrf24_cfg);
	      printf("nRF24: %s (status=%d)\r\n",
	             (nrf24_st == NRF24_OK) ? "OK, radio present" : "NOT FOUND",
	             (int)nrf24_st);
	      if (nrf24_st == NRF24_OK) {
	          nrf24_link_init(false);   /* auto-ack downlink */
	          nrf24_start_tx();
	      }
		  HAL_TIM_Base_Start_IT(&htim7);  /* micros() timebase -- must be running before imu.begin() so its internal sh2 timeouts can actually elapse */
	  }

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
	    nrf24_profiler.reset();
	    sd_card_profiler.reset();
	    tim7_profiler.reset();
	    tim12_profiler.reset();
	    kf_profiler.reset();



//	    SCB_InvalidateDCache_by_Addr((uint32_t *)adc_dma_buf_encoder, sizeof(adc_dma_buf_encoder));
	    HAL_Delay(10);
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
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);

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
		main_loop_profiler.start();

	if (uwTick - timeOfLastPrint >= 1000){

		timeOfLastPrint+= 1000;

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
		nrf24_profiler.metrics();
		sd_card_profiler.metrics();
		kf_profiler.metrics();
		tim7_profiler.metrics();
		tim12_profiler.metrics();


	}
		main_loop_profiler.end();
	if (task_ready(&printf_task)) { // 1000 ms
		printf_profiler.start();
		rb_flush();
		printf_profiler.end();
	  }

	if (task_ready(&sd_card_task)) { // 500 ms
			sd_card_prep();
	}
	sd_card_task_function(); // every iter

	if (task_ready(&nrf24_tx_task)) { // 20 ms, 50 Hz downlink
		if (nrf24_link_tx_idle()) {
			(void)nrf24_link_send(&local_sensor_data, sizeof(local_sensor_data));
		}
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
int tim7_counter = 0;
void tim7_trigger() { // 1 khz low priority
	tim7_counter++;
	tim7_profiler.start();
	if (true /*task_ready(&button_task)*/) { // 1ms
		Button.update();

		if (Button.pressed()) {
			button_profiler.start();
            missionControl.RequestToggle();     // ISR-safe, serviced in Iter()
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
            	    nrf24_profiler.reset();
            	    sd_card_profiler.reset();
            	    tim7_profiler.reset();
            	    tim12_profiler.reset();
            	    kf_profiler.reset();
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



//	rb_flush();
	if (task_ready(&IMU_task)) { // 1 ms
		IMU_profiler.start();
		imu.service();                 // poll at least every ~1 ms
		IMU_profiler.end();
	}
	nrf24_profiler.start();
	(void)nrf24_link_service();        // pumps the TX fragment FIFO, never blocks longer than one SPI burst
	nrf24_get_stats(&nrf24_radio_stats);
	nrf24_link_get_stats(&nrf24_frame_stats);
	{
	    /* Ground-station commands ride in as raw ACK payloads (rf24_gateway.py's
	     * write_ack_payload() / <BBIH>+crc16 - NOT our own nrf24_link fragment
	     * format, so nrf24_rx_read() is the right API here, not nrf24_link_poll()).
	     * This MUST run every tick: with nothing draining it, 3 queued repeats of
	     * one command exactly fill the 3-deep RX FIFO, and once full it stays
	     * full until the radio is re-initialized - that's what silently killed
	     * the whole downlink the first time a command was sent. */
	    uint8_t pipe;
	    while (nrf24_rx_available(&pipe)) {
	        uint8_t cmd_buf[NRF24_MAX_PAYLOAD];
	        (void)nrf24_rx_read(cmd_buf, sizeof(cmd_buf));
	        nrf24_uplink_rx_count++;
	        (void)pipe;
	        // TODO: parse <BBIH>+crc16 (see rf24_gateway.py build_cmd/CMD_STRUCT)
	        // and route to MissionControl - drain-only for now.
	    }
	}
	nrf24_profiler.end();
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
//			rb_write(&common_print_buffer, &local_sensor_data, (size_t)sizeof(SensorData_t));
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
