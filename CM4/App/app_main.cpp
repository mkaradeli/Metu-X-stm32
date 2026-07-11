/*
 * app_main.cpp
 *
 *  Created on: Jun 13, 2026
 *      Author: karadeli
 */

#include "app_main.hpp"

#define FRACTIONAL(x) int(floor(int((x)*100)))%100

#include "stm32h7xx_hal.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "Profiler.hpp"
#include "globals.hpp"
extern "C" {
#include "task_timer.h"
#include "shared_memory.h"
}

extern UART_HandleTypeDef huart6;
//#include "usart.h"

volatile uint64_t cpuTicks_overflow = 0;

FATFS FatFs;
FRESULT FR_Status;
FATFS *FS_Ptr;
UINT RWC, WWC; // Read/Write Word Counter
DWORD FreeClusters;
uint32_t TotalSize, FreeSpace;
int a;

int left_filename_index = 0;
int right_filename_index = 0;
//	FATFS FatFs;
FRESULT sd_create_log_file(char *filename, uint16_t *log_index);
FRESULT sd_recreate_log_file(char *filename, uint16_t log_index,
		uint16_t *secondary_log_index);
char filename[32] = { 0 };
uint16_t log_index = 1000;
uint16_t secondary_log_index = 0;
char initial_filename[32] = { 0 };
uint8_t file_created = 0;
uint8_t file_name_selected = 0;
uint8_t initial_file_name_selected = 0;
char RW_Buffer[200];
//	static int sd_mount();
//common_print_buffer_t isolated_print_buffer;

FRESULT disk_mounted = FR_DISK_ERR;
FRESULT file_open = FR_DISK_ERR;

//SensorData_t sensorData = { 0 };

FIL logFile;
FIL Fil;

const size_t scratch_buffer_max_size = ((128*1024) / PACKET_SIZE -1);
__attribute__((section(".sram2"), aligned(4)))
SensorData_t sensorDataScratch[scratch_buffer_max_size];

#include "ff_gen_drv.h"
char sd_path[4];
int sd_mount(void);
FRESULT sd_reopen_log_append(uint16_t log_index);
extern Disk_drvTypeDef disk;
extern "C" {
#include "ring_buffer.h"

extern common_print_buffer_t isolated_print_buffer;
extern volatile common_print_buffer_t common_print_buffer;
}
task_timer_t uart_task = { 50, 0 };
task_timer_t sd_card_task = { 1000, 0 };
task_timer_t printf_task = {1000, 0};

Profiler free_profiler;

Profiler sd_card_write_profiler;
Profiler lidar_profiler ;
Profiler printf_profiler ;

static uint8_t lidar_rx[64];

void app_init() {
	//	HAL_TIM_Base_Start_IT(&htim5);
	HAL_TIM_Base_Start_IT(&htim7); /* _IT = interrupt ile */

	lidar.Reset();

    HAL_UARTEx_ReceiveToIdle_DMA(&huart6, lidar.getBuffer(), 128);
	sd_card_write_profiler.reset();
	lidar_profiler.reset();
	free_profiler.reset();
	printf_profiler.reset();
	free_profiler.start();
	printf_task.last_trigger = uwTick;


}
void sd_card_task_function();
void sd_card_prep();
void app_loop() {
	//	printf(CLR_SCREEN);
	if (task_ready(&sd_card_task)) {
		sd_card_prep();
//		sd_card_task_function();
		sd_card_write_profiler.metrics();
		lidar_profiler.metrics();
		free_profiler.metrics();
		printf_profiler.metrics();
		printf("gdb debugging rocks!\n\r");
		printf("free_profiler cpu usage = %%%d.%d\n\r", (int)free_profiler.cpu_usage, FRACTIONAL(free_profiler.cpu_usage));
		printf("sd_card_write_profiler cpu usage = %%%d.%d\n\r", (int)sd_card_write_profiler.cpu_usage, FRACTIONAL(sd_card_write_profiler.cpu_usage));
		printf("lidar_profiler cpu usage = %%%d.%d\n\r", (int)lidar_profiler.cpu_usage, FRACTIONAL(lidar_profiler.cpu_usage));
		printf("printf_profiler cpu usage = %%%d.%d\n\r", (int)printf_profiler.cpu_usage, FRACTIONAL(printf_profiler.cpu_usage));

		printf("free_profiler task mean time = %d.%d\n\r",
					(int) free_profiler.mean_time,
					FRACTIONAL(free_profiler.mean_time));
		printf("sd_card task mean time = %d.%d\n\r",
					(int) sd_card_write_profiler.mean_time,
					FRACTIONAL(sd_card_write_profiler.mean_time));
		printf("lidar_profiler task mean time = %d.%d\n\r",
			(int) lidar_profiler.mean_time,
			FRACTIONAL(lidar_profiler.mean_time));
		printf("printf_profiler task mean time = %d.%d\n\r",
				(int) printf_profiler.mean_time,
				FRACTIONAL(printf_profiler.mean_time));

		float total_usage = free_profiler.cpu_usage
		+ sd_card_write_profiler.cpu_usage
		+ lidar_profiler.cpu_usage
		+ printf_profiler.cpu_usage;
		printf("total cpu usage accounted %%%d.%d \n\r", (int)total_usage, FRACTIONAL(total_usage));
	}


	sd_card_task_function();

	if (task_ready(&uart_task)) {
//		sensorData.timestamp = uwTick;
		// TODO: convert telemetry to DMA usage

		HAL_UART_Transmit(&huart6, (uint8_t*) &logData.sensorData[0], sizeof(SensorData_t),
				HAL_MAX_DELAY);

	}

	if (task_ready(&printf_task)) {
		printf_profiler.start();
//			printf(CLR_SCREEN);
//			HAL_Delay(100);
			printf("%d %ld , %ld\n\r", rb_count(&common_print_buffer), common_print_buffer.head, common_print_buffer.tail);
			printf("uwTick = %ld \n\r",uwTick);
			printf("filename = %s\n\r", filename);


			rb_flush(&common_print_buffer);

			rb_flush(&isolated_print_buffer);
			printf_profiler.end();


	  }

}











size_t scratch_buffer_size;
void sd_card_task_function() {
	if (file_created) { // file open already.
		sd_card_write_profiler.start();
		scratch_buffer_size = SensorData_Buffer_PopAll(&logData, sensorDataScratch, scratch_buffer_max_size);
		if (scratch_buffer_size) {// remove

		f_write(&Fil, &sensorDataScratch, sizeof(SensorData_t)*scratch_buffer_size, &WWC);
		// TODO: Circular buffer implement edilecek.
		// TODO: fonksiyona cevrilecek.

		if (sizeof(SensorData_t)*scratch_buffer_size == WWC) {
			FR_Status = f_sync(&Fil);
			if (FR_Status == FR_OK) {
				BSP_LED_Off(LED_RED);
				printf("\t\tw.e.c.=%d\n\r", scratch_buffer_size);
//				printf("sync successfull\n\r");
			} else {
				printf("sync FAILED!!!!!!!\n\r");
				BSP_LED_On(LED_RED);
				disk_mounted = FR_Status;
				file_created = 0;
				f_close(&Fil);
				f_mount(NULL, "", 1);
			}
		} else {
			printf("line addition failed.\n\r");
			BSP_LED_On(LED_RED);
			printf("RW Buffer len%d, wwc %d", strlen(RW_Buffer), WWC);
			disk_mounted = FR_INT_ERR;
			file_created = 0;
			f_close(&Fil);
			f_mount(NULL, "", 1);
		}
		}
		sd_card_write_profiler.end();
	}
}

void sd_card_prep() {

	if (disk_mounted != FR_OK) {
		BSP_LED_On(LED_RED);
		logData.ready=false;
		disk.is_initialized[0] = 0;
		f_mount(NULL, "", 1);
		//		FR_Status = (FRESULT)sd_mount();
		FR_Status = f_mount(&FatFs, "", 1);

		if (FR_Status != FR_OK) {
			//		if (FR_Status != FR_OK){
			printf("Error! While Mounting SD Card, Error Code: (%i)\r\n",
					FR_Status);
			BSP_LED_On(LED_RED);
			logData.ready=false;
			//						f_mount(NULL, "", 1);
			//						f_mount(NULL, "", 1);
		} else
			printf("SD Card Mounted Successfully! \r\n\n");
		disk_mounted = FR_Status;
	} else { // Disk mounted
//		BSP_LED_Off(LED_RED);
		if (initial_file_name_selected == 0) { // first file creation
			printf("creating file\n\r");
			file_open = sd_create_log_file(&filename[0], &log_index);
			if (file_open == FR_OK) {
				f_write(&Fil, (uint8_t*)&logFormatId, sizeof(uint16_t), &WWC);
				f_write(&Fil, &logHeaderSize, sizeof(uint8_t), &WWC);
				f_write(&Fil, (uint8_t*)&logHeader, logHeaderSize, &WWC);
				f_write(&Fil, (uint8_t*)&sensorDataLength, sizeof(uint16_t), &WWC);



				//							initial_file_name_selected = 0;
				//						else
				initial_file_name_selected = 1;
				file_created = 1;
				logData.ready=true;
				BSP_LED_Off(LED_RED);
				SensorData_Buffer_Reset_Dropped(&logData);
			}

		} else if (initial_file_name_selected == 1 && file_created == 0) { // recreation

			file_open = sd_reopen_log_append(log_index);
			if (file_open == FR_OK) {
				file_created = 1;
				FSIZE_t sz = f_size(&Fil);
				FSIZE_t aligned = (sz / sizeof(SensorData_t))
								* sizeof(SensorData_t);
				if (aligned != sz) {
					f_lseek(&Fil, aligned);
					f_truncate(&Fil);    // drop the partial trailing record
					f_sync(&Fil);
					logData.ready=true;
				}
			} else {
				initial_file_name_selected = 0; // give up, scan for a fresh name next loop

			}
		}

	}


};





FRESULT sd_reopen_log_append(uint16_t log_index) {
	char name[32];
	sprintf(name, "log%04u.bin", log_index);
	// TODO: sd fonksiyonlari cilalanacak.
	// FA_OPEN_APPEND (0x30) = open-or-create + seek to end of file
	return f_open(&Fil, name, FA_OPEN_APPEND | FA_WRITE);
}

FRESULT sd_create_log_file(char *filename, uint16_t *log_index) {
	volatile uint16_t index = 1000;
	//    char filename[32];
	FRESULT res;

	volatile uint16_t left_index = 1000;
	uint16_t right_index = 10000;
	while (left_index < right_index) {
		int mid = left_index + ((right_index - left_index) >> 1);
		sprintf(filename, "log%04u.bin", mid);  // log0000.bin, log0001.bin ...
		res = f_stat(filename, NULL);             // check if file exists
		if (res == FR_OK) {
			left_index = mid + 1;
			left_filename_index = left_index;
		}
		else if (res == FR_DISK_ERR) {
			return  res;
		}
		else {
			right_index = mid;
			right_filename_index = right_index;
		}
	}
	index = left_index;
	*log_index = index;

	do {
		sprintf(filename, "log%04u.bin", index); // log0000.bin, log0001.bin ...
		res = f_stat(filename, NULL);             // check if file exists
		index++;
	} while (res == FR_OK && index < 10000);     // stop if too many files
	res = f_open(&Fil, filename, FA_CREATE_NEW | FA_WRITE);
	if (res == FR_OK) {
//		*log_index = index;
		printf("Created new log file: %s\n", filename);
		//    f_expand(&Fil, 16*1024*1024, 1);
		//        file_creation_ok = 1;
	} else {
		//    	file_creation_ok = 0;
		printf("Failed to create log file, error: %d\n", res);
	}
	//    f_close(&logFile);
	return res;
}
FRESULT sd_recreate_log_file(char *filename, uint16_t log_index,
		uint16_t *secondary_log_index) {
	//    uint16_t index = 0;
	FRESULT res;
	//    index = left_index ;

	//    do {
	sprintf(filename, "log%04u_%04u.bin", log_index, *secondary_log_index); // log0000.bin, log0001.bin ...
	res = f_stat(filename, NULL);             // check if file exists
	//        index++;
	//    } while (res == FR_OK && index < 10000);     // stop if too many files
	res = f_open(&Fil, filename, FA_CREATE_NEW | FA_WRITE);
	if (res == FR_OK) {
		printf("Created new log file: %s\n", filename);
		*secondary_log_index = *secondary_log_index + 1;
		//    f_expand(&Fil, 16*1024*1024, 1);
		//        file_creation_ok = 1;
	} else {
		//    	file_creation_ok = 0;
		printf("Failed to create log file, error: %d\n", res);
	}
	//    f_close(&logFile);
	return res;
}

void tim7_trigger() {
	//	tim2_profiler.start();
	cpuTicks_overflow++;
	//	printf("HERE\n\r");
	//	tim2_profiler.end();
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM7) {
		tim7_trigger();
	}

}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
	if (huart == &huart6){
		lidar_profiler.start();
//		printf("aaaa lidar frame captured!\n\r");
		lidar.FrameHandler(Size);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart6, lidar.getBuffer(), 128);  // re-arm!
		lidar_profiler.end();
	}
}
void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart) {
    if (huart == &huart6)
        HAL_UARTEx_ReceiveToIdle_DMA(&huart6, lidar.getBuffer(), 128);  // recover
}


uint64_t cpuTicks() {
	return (cpuTicks_overflow << 16) + __HAL_TIM_GET_COUNTER(&htim7);
}
uint64_t micros() {
	return cpuTicks() / 200;
}
