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
//#include "shared_memory.h"
}

//extern UART_HandleTypeDef huart6;
//#include "usart.h"

//volatile uint64_t cpuTicks_overflow = 0;

//FATFS FatFs;
//FRESULT FR_Status;
//FATFS *FS_Ptr;
//UINT RWC, WWC; // Read/Write Word Counter
//DWORD FreeClusters;
//uint32_t TotalSize, FreeSpace;
//int a;
//
//int left_filename_index = 0;
//int right_filename_index = 0;
////	FATFS FatFs;
//FRESULT sd_create_log_file(char *filename, uint16_t *log_index);
//FRESULT sd_recreate_log_file(char *filename, uint16_t log_index,
//		uint16_t *secondary_log_index);
//char filename[32] = { 0 };
//uint16_t log_index = 1000;
//uint16_t secondary_log_index = 0;
//char initial_filename[32] = { 0 };
//uint8_t file_created = 0;
//uint8_t file_name_selected = 0;
//uint8_t initial_file_name_selected = 0;
//char RW_Buffer[200];
////	static int sd_mount();
////common_print_buffer_t isolated_print_buffer;
//
//FRESULT disk_mounted = FR_DISK_ERR;
//FRESULT file_open = FR_DISK_ERR;
//
////SensorData_t sensorData = { 0 };
//
//FIL logFile;
//FIL Fil;
//
//const size_t scratch_buffer_max_size = ((128*1024) / PACKET_SIZE -1);
//__attribute__((section(".sram2"), aligned(4)))
//SensorData_t sensorDataScratch[scratch_buffer_max_size];
//
//#include "ff_gen_drv.h"
//char sd_path[4];
//int sd_mount(void);
//FRESULT sd_reopen_log_append(uint16_t log_index);
//extern Disk_drvTypeDef disk;
//extern "C" {
//#include "ring_buffer.h"
//
//extern common_print_buffer_t isolated_print_buffer;
//extern volatile common_print_buffer_t common_print_buffer;
//}
task_timer_t uart_task = { 50, 0 };
task_timer_t sd_card_task = { 1000, 0 };
task_timer_t printf_task = {1000, 0};

Profiler free_profiler;

Profiler sd_card_write_profiler;
//Profiler lidar_profiler ;
Profiler printf_profiler ;

//static uint8_t lidar_rx[64];

void app_init() {


}
//void sd_card_task_function();
//void sd_card_prep();
void app_loop() {
}
