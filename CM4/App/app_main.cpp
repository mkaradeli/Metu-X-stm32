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
extern "C" {
#include "task_timer.h"
}
#include "shared_memory.h"

extern UART_HandleTypeDef huart6;
//#include "usart.h"

volatile uint64_t micros_overflow = 0;

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
uint16_t log_index = 0;
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

SensorData_t sensorData = { 0 };

FIL logFile;
FIL Fil;

#include "ff_gen_drv.h"
char sd_path[4];
int sd_mount(void);
FRESULT sd_reopen_log_append(uint16_t log_index);
extern Disk_drvTypeDef disk;
task_timer_t uart_task = { 50, 0 };
task_timer_t sd_card_task = { 1000, 0 };

Profiler sd_card_write_profiler;

void app_init() {
	//	HAL_TIM_Base_Start_IT(&htim5);
	HAL_TIM_Base_Start_IT(&htim7); /* _IT = interrupt ile */

	sd_card_write_profiler.reset();

}

void app_loop() {
	//	printf(CLR_SCREEN);
	if (task_ready(&sd_card_task)) {
		sd_card_write_profiler.start();
		if (disk_mounted != FR_OK) {
			BSP_LED_On(LED_RED);
			disk.is_initialized[0] = 0;
			f_mount(NULL, "", 1);
			//		FR_Status = (FRESULT)sd_mount();
			FR_Status = f_mount(&FatFs, "", 1);

			if (FR_Status != FR_OK) {
				//		if (FR_Status != FR_OK){
				printf("Error! While Mounting SD Card, Error Code: (%i)\r\n",
						FR_Status);
				//						f_mount(NULL, "", 1);
				//						f_mount(NULL, "", 1);
			} else
				printf("SD Card Mounted Successfully! \r\n\n");
			disk_mounted = FR_Status;
		} else { // Disk mounted
			BSP_LED_Off(LED_RED);
			if (initial_file_name_selected == 0) { // first file creation
				printf("creating file\n\r");
				file_open = sd_create_log_file(&filename[0], &log_index);
				if (file_open == FR_OK) {
					//							initial_file_name_selected = 0;
					//						else
					initial_file_name_selected = 1;
					file_created = 1;
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
					}
				} else {
					initial_file_name_selected = 0; // give up, scan for a fresh name next loop

				}
			} else { // file open already.
				//					if (file_open == FR_OK) {
				//							printf("file open %s\n\r", filename);
				//						snprintf(RW_Buffer, sizeof(RW_Buffer), "timestamp = %ld\n\r", uwTick);
				FR_Status = f_write(&Fil, &sensorData, sizeof(sensorData),
						&WWC);
				// TODO: Circular buffer implement edilecek.
				// TODO: fonksiyona cevrilecek.

				if (sizeof(sensorData) == WWC) {
					//								printf("line written\n\r");
					FR_Status = f_sync(&Fil);
					if (FR_Status == FR_OK) {
						printf("sync successfull\n\r");
					} else {
						printf("sync FAILED!!!!!!!\n\r");
						disk_mounted = FR_Status;
						file_created = 0;
						f_close(&Fil);
						f_mount(NULL, "", 1);

					}

				} else {
					printf("line addition failed.\n\r");
					printf("RW Buffer len%d, wwc %d", strlen(RW_Buffer), WWC);
					disk_mounted = FR_INT_ERR;
					file_created = 0;
					f_close(&Fil);
					//								DESELECT();
					f_mount(NULL, "", 1);
					//							file_open = 1;
				}

				//
				//					}
				//					else {
				//
				//					}

			}
		}
		//				printf("DISK STATUS = %d\n\r", disk_mounted);

		//			printf("FILE STATUS = %d\n\r\n\r", file_open);
		a = sizeof(sensorData);
		//					int b = a;
		//					HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout)

		sd_card_write_profiler.end();
		sd_card_write_profiler.metrics();
		printf("sd_card task mean time = %d.%d, cpu usage = %d.%d \n\r",
				(int) sd_card_write_profiler.mean_time,
				FRACTIONAL(sd_card_write_profiler.mean_time),
				(int) sd_card_write_profiler.cpu_usage,
				FRACTIONAL(sd_card_write_profiler.cpu_usage));
		//	printf("%lu\n\r", __HAL_TIM_GET_COUNTER(&htim7));
		printf("%d, %p, %d, %d\n\r", disk.is_initialized[0], disk.drv[0],
				disk.lun[0], disk.nbr);
		//	printf("%lu\n\r", (uint32_t)micros());
		//	printf("%lu\n\r", (uint32_t)micros_overflow);
		printf("%d\n\r", a);
	}

	if (task_ready(&uart_task)) {
		sensorData.timestamp = uwTick;
		// TODO: convert telemetry to DMA usage

		HAL_UART_Transmit(&huart6, (uint8_t*) &sensorData, sizeof(sensorData),
				HAL_MAX_DELAY);

	}

}

FRESULT sd_reopen_log_append(uint16_t log_index) {
	char name[32];
	sprintf(name, "log%04u.bin", log_index);
	// TODO: sd fonksiyonlari cilalanacak.
	// FA_OPEN_APPEND (0x30) = open-or-create + seek to end of file
	return f_open(&Fil, name, FA_OPEN_APPEND | FA_WRITE);
}

FRESULT sd_create_log_file(char *filename, uint16_t *log_index) {
	uint16_t index = 0;
	//    char filename[32];
	FRESULT res;

	uint16_t left_index = 0;
	uint16_t right_index = 10000;
	while (left_index < right_index) {
		int mid = left_index + ((right_index - left_index) >> 1);
		sprintf(filename, "log%04u.bin", mid);  // log0000.bin, log0001.bin ...
		res = f_stat(filename, NULL);             // check if file exists
		if (res == FR_OK) {
			left_index = mid + 1;
			left_filename_index = left_index;
		} else {
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
	micros_overflow++;
	//	printf("HERE\n\r");
	//	tim2_profiler.end();
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM7) {
		tim7_trigger();
	}

}

//
//int sd_mount(void) {
//	FRESULT res;
////	extern uint8_t sd_is_sdhc(void);
//
//	printf("Linking SD driver...\r\n");
//	if (FATFS_LinkDriver(disk.drv[0], &sd_path[0]) != 0) {
//		printf("FATFS_LinkDriver failed\n");
//		return FR_DISK_ERR;
//	}
//	printf("%s\n\r",sd_path);
////
//	printf("Initializing disk...\r\n");
//	DSTATUS stat = disk_initialize(0);
//	if (stat != 0) {
//		printf("disk_initialize failed: 0x%02X\n", stat);
//		printf("FR_NOT_READY\tTry Hard Reset or Check Connection/Power\r\n");
//		return FR_NOT_READY;
//	}
////
//	printf("Attempting mount at %s...\r\n", sd_path);
//	res = f_mount(&FatFs, sd_path, 1);
//	if (res == FR_OK)
//	{
//		printf("SD card mounted successfully at %s\r\n", sd_path);
////		printf("Card Type: %s\r\n", sd_is_sdhc() ? "SDHC/SDXC" : "SDSC");
////
//		return FR_OK;
//	}
////
////
////	// Any other mount error
//	printf("Mount failed with code: %d\r\n", res);
//	return res;
//}

uint64_t micros() {
	return (micros_overflow << 16) + __HAL_TIM_GET_COUNTER(&htim7);
}
