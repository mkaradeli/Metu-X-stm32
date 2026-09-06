/*
 * app_main.cpp
 *
 *  Created on: May 14, 2026
 *      Author: karadeli
 */

#include "globals.hpp"   // HWIL_ENABLED lives here now, shared with UserTask.cpp
#define DISABLE_CRC false
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
uint32_t           nrf24_uplink_rx_count   = 0u;   /* ACK-payload uplink packets drained, see tim7_trigger() */
uint32_t           nrf24_uplink_bad_frames = 0u;   /* wrong length or CRC-16 mismatch, dropped                */
uint32_t           nrf24_uplink_dup_drops  = 0u;   /* repeat of an already-processed seq, ignored              */

/* Go/no-go: bit set = subsystem currently healthy. Recomputed every 100 ms in
 * LED_Counter_Tick(); MissionControl::Start() ANDs this against
 * missionControl.go_no_go_enabled to decide whether arming is allowed. */
uint8_t  go_no_go_status = 0u;
#define IMU_STALE_MS    200u   /* IMU reports at 200-400 Hz; way past one period if stale */
#define LIDAR_STALE_MS  300u   /* lidar repeats ~100/s per MissionUart.cpp's own estimate  */
static uint32_t s_last_imu_ok_tick   = 0u;
static uint32_t s_last_lidar_ok_tick = 0u;

/* Battery: 6S LiPo, sensed on adc_dma_buf_pressure[5] through a 1/21 divider.
 * BATTERY_ADC_VREF_V assumes VREF+ = VDDA = 3.3 V (no VREFBUF override found
 * in this project) -- if the board's analog reference is actually something
 * else, this threshold is silently wrong by that same ratio. Verify against
 * a real multimeter reading before trusting this gate. */
#define BATTERY_DIVIDER_RATIO   21.0f
#define BATTERY_ADC_VREF_V      3.3f
#define BATTERY_MIN_VOLTS       23.0f   /* ~25% SoC cutoff for a 6S pack */
#define BATTERY_RAW_MIN         ((uint16_t)((BATTERY_MIN_VOLTS / BATTERY_DIVIDER_RATIO \
                                  / BATTERY_ADC_VREF_V) * 65535.0f + 0.5f))

/* Ground uplink command wire format -- MUST match rf24_gateway.py's CMD_*
 * opcodes / VERB table and CMD_STRUCT ("<BBIH" + crc16, little-endian).
 * Delivered as a raw nRF24 ACK payload (nrf24_rx_read() below), NOT our own
 * nrf24_link fragment format:
 *
 *   byte 0    seq       wraps at 256; gateway repeats a command 3x on the
 *                       same seq so one lost ACK payload doesn't lose it
 *   byte 1    cmd       opcode, see UPLINK_CMD_* below
 *   byte 2..5 echo_ts   last telemetry timestamp the gateway had seen (u32 LE)
 *   byte 6..7 arg       opcode-specific argument (u16 LE)
 *   byte 8..9 crc16     CRC-16/CCITT-FALSE seed 0xFFFF over bytes 0..7, LE
 */
#define UPLINK_CMD_LEN        10u
#define UPLINK_CMD_NOP        0x00u
#define UPLINK_CMD_SEL        0x10u
#define UPLINK_CMD_ARM        0x11u
#define UPLINK_CMD_START      0x12u
#define UPLINK_CMD_STOP       0x20u
#define UPLINK_CMD_ABORT      0x21u
#define UPLINK_CMD_DISCHARGE  0x30u
#define UPLINK_CMD_SHUTDOWN   0x40u
#define UPLINK_CMD_SET_GONOGO 0x50u   /* arg low byte = new go_no_go_enabled mask */

static bool    s_have_last_cmd_seq = false;
static uint8_t s_last_cmd_seq = 0;

/* Dispatch one validated uplink command directly against MissionControl's
 * API -- no ASCII round-trip through HandleCommand(). The one safety property
 * that mattered there (ARM/START only fires if the operator's confirmation
 * names the mission currently SELECTed) is preserved as a plain index
 * compare: `arg == SelectedIndex()` is the binary-native equivalent of
 * HandleCommand()'s ieq(arg, SelectedName()) check, and unlike the old
 * missionTable[arg].name lookup it needs no bounds check at all -- arg is
 * never used to index anything here, so an out-of-range value just fails the
 * compare instead of ever risking an out-of-bounds read.
 *
 * Each case calls exactly what HandleCommand() itself calls underneath
 * (direct Start()/End()/Select() vs. the ISR-safe Request* variants for
 * discharge/shutdown), so this is not a change in ISR-safety -- only in how
 * the command gets there. */
static void uplink_cmd_dispatch(uint8_t cmd, uint16_t arg)
{
	switch (cmd) {
	case UPLINK_CMD_NOP:
		return;

	case UPLINK_CMD_SEL: {
		bool ok = missionControl.Select((uint8_t)arg);
		if (ok) printf("SEL %u OK\r\n", (unsigned)arg);
		else    printf("SEL %u FAIL: %s\r\n", (unsigned)arg,
		                MissionControl::ErrorText(missionControl.last_error));
		return;
	}

	case UPLINK_CMD_ARM:
	case UPLINK_CMD_START: {
		if (arg != missionControl.SelectedIndex()) {
			printf("ARM FAIL: arg %u != selected %u\r\n",
			       (unsigned)arg, (unsigned)missionControl.SelectedIndex());
			return;
		}
		bool ok = missionControl.Start();
		if (ok) printf("ARM %s OK\r\n", missionControl.SelectedName());
		else    printf("ARM %s FAIL: %s\r\n", missionControl.SelectedName(),
		                MissionControl::ErrorText(missionControl.last_error));
		return;
	}

	case UPLINK_CMD_STOP:
	case UPLINK_CMD_ABORT:
		missionControl.End();
		printf("STOP/ABORT: state=%s\r\n", missionControl.StateName());
		return;

	case UPLINK_CMD_DISCHARGE:
		missionControl.RequestSafeDischarge();
		printf("DISCHARGE requested\r\n");
		return;

	case UPLINK_CMD_SHUTDOWN:
		missionControl.RequestShutdown();
		printf("SHUTDOWN requested\r\n");
		return;

	case UPLINK_CMD_SET_GONOGO:
		/* Doesn't move hardware, just changes what's allowed to gate a
		 * later ARM. */
		missionControl.go_no_go_enabled = (uint8_t)(arg & 0xFFu);
		printf("GONOGO enabled=0x%02X\r\n", missionControl.go_no_go_enabled);
		return;

	default:
		return;
	}
}


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
DebouncedButton SafetyConnector{false,SAFETY_CONNECTOR_GPIO_Port,SAFETY_CONNECTOR_Pin};
volatile float load_filtered;
uint16_t crc16_calc(const uint8_t *p, size_t n);
bool mount_ok;
bool file_creation_ok;
float time_sec = 0;
int load_cell_counter = 0;

SensorData_t local_sensor_data{'K','D'};
SensorData_t * last_valid_sensor_data = 0;

void onImuReport(const BNO085& r);
void onLidarFrame(uint16_t distMm, uint16_t strength);

LowPass load_lpf{1.0f, 1000,1};  // 30 Hz cutoff @ 1 kHz sample rate
LowPass accel_interval{1.0f, 1000,1};  // 30 Hz cutoff @ 1 kHz sample rate
LowPass quatIntRV_interval{1.0f,1000,1};

Profiler free_profiler{"free"};
Profiler load_cell_profiler{"load"};
Profiler main_loop_profiler{"main"};
Profiler printf_profiler{"prin"};
Profiler adc1_profiler{"enco"};
Profiler adc2_profiler{"curr"};
Profiler adc3_profiler{"pres"};
Profiler tim2_profiler{"tim2"};
Profiler tim3_profiler{"tim3"};
Profiler tim4_profiler{"tim4"};
Profiler button_profiler{"butt"};
Profiler crc_profiler{"crc_"};
Profiler IMU_profiler{"IMU_"};
Profiler nrf24_profiler{"nrf2"};
Profiler sd_card_profiler{"sd_c"};
Profiler tim7_profiler{"tim7"};
Profiler kf_profiler{"kf_p"};
Profiler tim12_profiler{"tim1"};
Profiler hwil_profiler{"hwil"};
Profiler lidar_profiler{"lidr"};

Profiler *profilers[] = {
		&free_profiler,
		&load_cell_profiler,
		&main_loop_profiler,
		&printf_profiler,
		&adc1_profiler,
		&adc2_profiler,
		&adc3_profiler,
		&tim2_profiler,
		&tim3_profiler,
		&tim4_profiler,
		&button_profiler,
		&crc_profiler,
		&IMU_profiler,
		&nrf24_profiler,
		&sd_card_profiler,
		&tim7_profiler,
		&kf_profiler,
		&tim12_profiler,
		&hwil_profiler,
		&lidar_profiler,

};


float battery_voltage = 0;
bool True = true;
MissionControl missionControl(&True,
			&logData.record);
//MissionC/ontrol missionControl(&logData.ready, &logData.record);

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
		HAL_Delay(1000);  /* TF02-Pro needs ~1s after a system reset before it responds again (Benewake protocol manual) */
		uint8_t lidarFwVersion[3] = {0};
		bool lidarHealthy = lidar.HealthCheck(lidarFwVersion);
		printf("TF02-Pro lidar: %s\r\n",
		       lidarHealthy ? "OK" : "NOT RESPONDING (check USART6 wiring/power/baud)");
		if (lidarHealthy) {
			printf("TF02-Pro lidar firmware v%u.%u.%u\r\n",
			       lidarFwVersion[2], lidarFwVersion[1], lidarFwVersion[0]);
		}
//		altEstimator.begin();
		HAL_UARTEx_ReceiveToIdle_DMA(&huart6, lidar.getBuffer(), 128);
//	  dummy_init();
//	  printf(CLR_SCREEN);

//	    local_sensor_data.current_demand = 2;
//	    local_sensor_data.timestamp = 0;
//	    ready_to_write_a=0;
		for (int i=0; i<(sizeof(profilers)/sizeof(profilers[0])); i++)
			profilers[i]->reset();

	    HAL_Delay(10);
	    for (int i=0; i<4; i++)
	    	actuator[i].calibrate();
	    platform_controller.initialize();
	    hwil.initialize();
#if HWIL_ENABLED
	    /* Initial conditions, set once at boot -- not touched again per-tick.
	     * NOTE: since hwil.initialize() only runs once per power cycle, a
	     * second mission armed in the same session continues the simulated
	     * rocket from wherever the first mission's flight left off (position/
	     * velocity aren't reset back to X0/V0), rather than starting fresh.
	     * If that's not what you want, this needs to move to wherever a
	     * mission's Start() actually fires instead of here. */
	     hwil.rtU.V0 = 0.0f;   // rocket starts at rest
	     hwil.rtU.X0 = 2.0f;   // rocket starts at ground level
	     hwil.rtY.position = hwil.rtU.X0;
	     hwil.rtY.velocity = hwil.rtU.V0;

#endif
		for (int i= 0; i< 4; i++) {
			actuator[i].actuatorController.rtU.pos_feedback = actuator[i].hallEffect.valveAngle;
			actuator[i].actuatorController.rtU.SpeedFeedback = actuator[i].hallEffect.valveVelocity;
			actuator[i].actuatorController.rtU.P_manifold = Actuator::manifold->getPsi();
			actuator[i].actuatorController.rtU.P_nozzle_demand = 0.0F;

		}



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
float total_cpu_usage = 0;
bool altitudeEstimatorDone = false;
extern Profiler HWIL_STEP_profiler;
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
		printf("timestamp = %ld\n\r", uwTick);
		total_cpu_usage = 0;
		for (int i=0; i<(sizeof(profilers)/sizeof(profilers[0])); i++){
			profilers[i]->metrics();
			printf("%.4s cpu=%f, freq=%f\n\r",profilers[i]->name, profilers[i]->cpu_usage, profilers[i]->call_frequency);
			total_cpu_usage += profilers[i]->cpu_usage;
		}
		HWIL_STEP_profiler.metrics();
		printf("\ttotal cpu usage = %f\n\r", total_cpu_usage);
		printf("battery voltage = %f\n\r", battery_voltage);

	}
		main_loop_profiler.end();
	if (task_ready(&printf_task)) { // 1000 ms
		printf_profiler.start();
		rb_flush();
		printf_profiler.end();
	  }

	if (task_ready(&sd_card_task)) { // 500 ms
		sd_card_profiler.start();
		sd_card_prep();
		sd_card_profiler.end();
	}
	sd_card_task_function(); // every iter

	if (task_ready(&nrf24_tx_task)) { // 20 ms, 50 Hz downlink
		if (nrf24_link_tx_idle()) {
			(void)nrf24_link_send(&local_sensor_data, sizeof(local_sensor_data));
		}
	}
//	battery_voltage = static_cast<float>(adc_dma_buf_pressure[5])/65536.0f *3.3f *21.0f;

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
            for (int i=0; i<(sizeof(profilers)/sizeof(profilers[0])); i++)
            			profilers[i]->reset();
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
	        uint8_t n = nrf24_rx_read(cmd_buf, sizeof(cmd_buf));
	        nrf24_uplink_rx_count++;
	        (void)pipe;

	        if (n != UPLINK_CMD_LEN) {
	            nrf24_uplink_bad_frames++;
	            continue;
	        }
	        uint16_t crc_calc = nrf24_crc16_ccitt_false(cmd_buf, 8, 0xFFFFu);
	        uint16_t crc_rx   = (uint16_t)cmd_buf[8] | ((uint16_t)cmd_buf[9] << 8);
	        if (crc_calc != crc_rx) {
	            nrf24_uplink_bad_frames++;
	            continue;
	        }

	        uint8_t  seq = cmd_buf[0];
	        uint8_t  cmd = cmd_buf[1];
	        uint16_t arg = (uint16_t)cmd_buf[6] | ((uint16_t)cmd_buf[7] << 8);

	        /* rf24_gateway.py repeats every command 3x on the same seq so one
	         * lost ACK payload doesn't lose the command; dedup here so e.g.
	         * STOP/ABORT doesn't get processed three times over. */
	        if (s_have_last_cmd_seq && seq == s_last_cmd_seq) {
	            nrf24_uplink_dup_drops++;
	            continue;
	        }
	        s_have_last_cmd_seq = true;
	        s_last_cmd_seq = seq;

	        uplink_cmd_dispatch(cmd, arg);
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
        	s_last_imu_ok_tick = uwTick;
        }

    }
//    float R[3][3]; quatToR(q, R);
	if(lidar.hasNewReading()) {

		onLidarFrame(lidar.getDistance(), lidar.getStrength());
		s_last_lidar_ok_tick = uwTick;
	}
//    float aw[3];
//    for (int i = 0; i < 3; ++i)
//        aw[i] = R[i][0]*a_b[0] + R[i][1]*a_b[1] + R[i][2]*a_b[2];
    kf_profiler.end();


    if (task_ready(&heartbeat_task))
    	  		LED_Counter_Tick();
//    missionControl.HandleCommand(rx_line, reply, sizeof(reply))
    // TODO: recive handling

#if HWIL_ENABLED
    /* No "active" flag on the model itself, so gate it externally: only
     * step (and only bother feeding it inputs) while a mission is actually
     * running, so the simulated rocket doesn't integrate motion while idle
     * on the bench pre-arm. */
    hwil_profiler.start();
    if (missionControl.firing) {
        hwil.rtU.F_nozzle_1 = actuator[0].actuatorController.rtY.ThrustEstimate;
        hwil.rtU.F_nozzle_2 = actuator[1].actuatorController.rtY.ThrustEstimate;
        hwil.rtU.F_nozzle_3 = actuator[2].actuatorController.rtY.ThrustEstimate;
        hwil.rtU.F_nozzle_4 = actuator[3].actuatorController.rtY.ThrustEstimate;
        hwil.rtU.P_nozzle_manifold = Actuator::manifold->getPsi();
        hwil.rtU.P_nozzle_1 = actuator[0].getPressurePsi();
        hwil.rtU.P_nozzle_2 = actuator[1].getPressurePsi();
        hwil.rtU.P_nozzle_3 = actuator[2].getPressurePsi();
        hwil.rtU.P_nozzle_4 = actuator[3].getPressurePsi();
        hwil.rtU.quaternion_true[0] = imu.gyroIntegratedRV.i;
        hwil.rtU.quaternion_true[1] = imu.gyroIntegratedRV.j;
        hwil.rtU.quaternion_true[2] = imu.gyroIntegratedRV.k;
        hwil.rtU.quaternion_true[3] = imu.gyroIntegratedRV.real;
        hwil.step();
    }
    hwil_profiler.end();
#endif

	tim7_profiler.end();
}

void tim12_trigger(){ // mid priority 1000hz platform control task
	tim12_profiler.start();
//	platform_controller.rtU.Height = altEstimator.altitude(); // m
//	platform_controller.rtU.Velocity = altEstimator.velocity(); // m/s
	platform_controller.rtU.ManifoldPressure = Actuator::manifold->getBar(); // bar -- real either way, HWIL has no manifold output
#if HWIL_ENABLED
	platform_controller.rtU.quaternion[0] = hwil.rtY.quaternion_sim[0];
	platform_controller.rtU.quaternion[1] = hwil.rtY.quaternion_sim[1];
	platform_controller.rtU.quaternion[2] = hwil.rtY.quaternion_sim[2];
	platform_controller.rtU.quaternion[3] = hwil.rtY.quaternion_sim[3];
	platform_controller.rtU.Height = hwil.rtY.position;
	platform_controller.rtU.Velocity = hwil.rtY.velocity;
	platform_controller.rtU.angularVelocity[0] = hwil.rtY.angular_velocity[0];
	platform_controller.rtU.angularVelocity[1] = hwil.rtY.angular_velocity[1];
	platform_controller.rtU.angularVelocity[2] = hwil.rtY.angular_velocity[2];

#else
	platform_controller.rtU.quaternion[0] = imu.gyroIntegratedRV.i;
	platform_controller.rtU.quaternion[1] = imu.gyroIntegratedRV.j;
	platform_controller.rtU.quaternion[2] = imu.gyroIntegratedRV.k;
	platform_controller.rtU.quaternion[3] = imu.gyroIntegratedRV.real;
	platform_controller.rtU.Height = g_altEst.height();
	platform_controller.rtU.Velocity = g_altEst.velocity();
	platform_controller.rtU.angularVelocity[0] = imu.gyroIntegratedRV.angVelX;
	platform_controller.rtU.angularVelocity[1] = imu.gyroIntegratedRV.angVelY;
	platform_controller.rtU.angularVelocity[2] = imu.gyroIntegratedRV.angVelZ;
#endif
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
bool accept_log_entry=false;
static uint32_t log_tick = 0;
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
	battery_voltage = static_cast<float>(adc_dma_buf_pressure[5])/65536.0f *3.3f *21.0f;
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
	log_tick++;
	accept_log_entry = false;(logData.write_idx) < (LOG_HALF_RECORDS/2);
	accept_log_entry |= !(logData.half_full[0] || logData.half_full[1]);
	accept_log_entry |= ((log_tick%2) == 0);
	if (accept_log_entry) {

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
		local_sensor_data.actuatorData[j].thrust_demand = actuator[j].actuatorController.rtU.F_demand;
		local_sensor_data.actuatorData[j].thrust_estimated = actuator[j].actuatorController.rtY.ThrustEstimate;

	}
		local_sensor_data.manifold_pressure = Actuator::manifold->getPsi();
#if HWIL_ENABLED
//		for (int i = 0; i <3; i++)
		local_sensor_data.linearAccel.x = hwil.rtY.acceleration[0];   // real either way: HWIL has no 3-axis accel output to substitute
		local_sensor_data.linearAccel.y = hwil.rtY.acceleration[1];   // real either way: HWIL has no 3-axis accel output to substitute
		local_sensor_data.linearAccel.z = hwil.rtY.acceleration[2];   // real either way: HWIL has no 3-axis accel output to substitute
		local_sensor_data.quaternion.i = hwil.rtY.quaternion_sim[0];
		local_sensor_data.quaternion.j = hwil.rtY.quaternion_sim[1];
		local_sensor_data.quaternion.k = hwil.rtY.quaternion_sim[2];
		local_sensor_data.quaternion.real = hwil.rtY.quaternion_sim[3];
		local_sensor_data.angularVelocity.x = hwil.rtY.angular_velocity[0];
		local_sensor_data.angularVelocity.y = hwil.rtY.angular_velocity[1];
		local_sensor_data.angularVelocity.z = hwil.rtY.angular_velocity[2];
#else
		local_sensor_data.angularVelocity.x = imu.gyroIntegratedRV.angVelX;
		local_sensor_data.angularVelocity.y = imu.gyroIntegratedRV.angVelY;
		local_sensor_data.angularVelocity.z = imu.gyroIntegratedRV.angVelZ;
//				local_sensor_data.angularVelocity.y = hwil.rtY.angular_velocity[1];
//				local_sensor_data.angularVelocity.z = hwil.rtY.angular_velocity[2];
		local_sensor_data.linearAccel = imu.accel;   // real either way: HWIL has no 3-axis accel output to substitute
		local_sensor_data.quaternion.i = imu.gyroIntegratedRV.i;
		local_sensor_data.quaternion.j = imu.gyroIntegratedRV.j;
		local_sensor_data.quaternion.k = imu.gyroIntegratedRV.k;
		local_sensor_data.quaternion.real = imu.gyroIntegratedRV.real;
#endif
		local_sensor_data.angularVelocityDemand.x = platform_controller.rtY.omega_demand[0];
		local_sensor_data.angularVelocityDemand.y = platform_controller.rtY.omega_demand[1];
		local_sensor_data.angularVelocityDemand.z = platform_controller.rtY.omega_demand[2];
		local_sensor_data.lidarDistance = lidar.getDistance();
		local_sensor_data.lidarStrength = lidar.getStrength();

#if HWIL_ENABLED
		local_sensor_data.kf_altitude = hwil.rtY.position;
		local_sensor_data.kf_velocity = hwil.rtY.velocity;
#else
		local_sensor_data.kf_altitude = g_altEst.height();
		local_sensor_data.kf_velocity = g_altEst.velocity();
#endif

        local_sensor_data.actuator_mode = static_cast<uint8_t>(controller_mode);
        local_sensor_data.mission_modes = static_cast<uint8_t>(mission_mode);
        local_sensor_data.system_modes = static_cast<uint8_t>(missionControl.system_mode);
        local_sensor_data.last_error   = static_cast<uint8_t>(missionControl.last_error);
        local_sensor_data.go_no_go_status  = go_no_go_status;
        local_sensor_data.go_no_go_enabled = missionControl.go_no_go_enabled;
        local_sensor_data.battery_voltage = battery_voltage;


		if(task_ready(&uart_logging)){
#if not ENABLE_PRINT
			rb_write(&common_print_buffer, &local_sensor_data, (size_t)sizeof(SensorData_t));
#endif
		}

		if (missionControl.running and logData.ready) {
		    SensorData_t *slot = SensorData_Buffer_Reserve(&logData);
		if (slot){
			*slot = local_sensor_data;
			crc_profiler.start();
			slot->crc = crc16_calc((uint8_t*)slot, sizeof(SensorData_t) - 2);
			crc_profiler.end();
			last_valid_sensor_data = slot;
			SensorData_Buffer_Commit(&logData);
		}
	}
	}

	adc3_profiler.end();
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
	if (huart == &huart6){
		lidar_profiler.start();
//		printf("aaaa lidar frame captured!\n\r");
		lidar.FrameHandler(Size);
//		if (lidar.hasNewReading()) altEstimator.onLidarFrame(lidar.getDistance(), lidar.getStrength());

		HAL_UARTEx_ReceiveToIdle_DMA(&huart6, lidar.getBuffer(), 128);  // re-arm!
		lidar_profiler.end();
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

	/* Yellow: on whenever the downlink isn't demonstrably working right now --
	 * either it just dropped a frame (frames_dropped ticking up) or nothing
	 * went out at all since the last check (frames_sent stalled -- covers a
	 * dead/hung radio that never even gets far enough to hit a retry-exhausted
	 * drop). Off only while frames are actually leaving clean. */
	static uint32_t lastFramesSent = 0;
	static uint32_t lastFramesDropped = 0;
	bool sentProgress = (nrf24_frame_stats.frames_sent != lastFramesSent);
	bool droppingNow  = (nrf24_frame_stats.frames_dropped != lastFramesDropped);
	lastFramesSent    = nrf24_frame_stats.frames_sent;
	lastFramesDropped = nrf24_frame_stats.frames_dropped;

	if (sentProgress && !droppingNow)
		BSP_LED_Off(LED_YELLOW);
	else
		BSP_LED_On(LED_YELLOW);

	/* Go/no-go: recompute which of the 7 gated subsystems are currently
	 * healthy. See MissionControl::Start(), which ANDs this against
	 * missionControl.go_no_go_enabled before allowing an arm. */
	{
		uint8_t status = 0;

		if ((uwTick - s_last_imu_ok_tick) < IMU_STALE_MS)
			status |= GoNoGo::IMU;
		if ((uwTick - s_last_lidar_ok_tick) < LIDAR_STALE_MS)
			status |= GoNoGo::LIDAR;

		bool pressure_ok = true;
		for (int i = 0; i < 5; i++)
			if (adc_dma_buf_pressure[i] < 4000) pressure_ok = false;
		if (pressure_ok) status |= GoNoGo::PRESSURE;

		if (logData.ready) status |= GoNoGo::SD_CARD;

		if (sentProgress && !droppingNow) status |= GoNoGo::TELEMETRY;

		bool current_ok = true;
		for (int i = 0; i < 4; i++)
			if (adc_dma_buf_current[i] < 4000) current_ok = false;
		if (current_ok) status |= GoNoGo::CURRENT;

		if (adc_dma_buf_pressure[5] >= BATTERY_RAW_MIN) status |= GoNoGo::BATTERY;

		go_no_go_status = status;
	}

	index ++;
	index %= 7;
}

uint16_t crc16_calc(const uint8_t *p, size_t n)
{
#if DISABLE_CRC
	return LOG_TERMINATOR;
//	return (uint16_t) '\n\r';
#else
    CRC->CR |= CRC_CR_RESET;               // reload INIT (0xFFFF), self-clears
    while (n--)
        *(volatile uint8_t *)&CRC->DR = *p++;   // STRB: byte-wide feed
    return (uint16_t)CRC->DR;
#endif
}
