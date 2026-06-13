/*
 * app_main.cpp
 *
 *  Created on: Jun 13, 2026
 *      Author: karadeli
 */



#include "app_main.hpp"



#include <stdbool.h>
#include <stdio.h>
#include <string.h>

volatile uint64_t micros_overflow=0;


FATFS FatFs;
FRESULT FR_Status;
FATFS *FS_Ptr;
UINT RWC, WWC; // Read/Write Word Counter
DWORD FreeClusters;
uint32_t TotalSize, FreeSpace;


int left_filename_index = 0;
int right_filename_index = 0;
//	FATFS FatFs;
FRESULT sd_create_log_file(char* filename);
char filename[32] = {0};
char RW_Buffer[200];
//	static int sd_mount();


FRESULT disk_mounted= FR_DISK_ERR;
FRESULT file_open = FR_DISK_ERR;

FIL logFile;
FIL Fil;


void app_init() {


}

void app_loop() {
	if (disk_mounted != FR_OK){
					f_mount(NULL, "", 1);
					FR_Status = f_mount(&FatFs, "", 1);
						if (FR_Status != FR_OK){
						  printf("Error! While Mounting SD Card, Error Code: (%i)\r\n", FR_Status);
						f_mount(NULL, "", 1);}
						else
							printf("SD Card Mounted Successfully! \r\n\n");
						disk_mounted = FR_Status;
				}
				else { // Disk mounted
					if (filename[0]==0){
						printf("creating file\n\r");
						file_open = sd_create_log_file(&filename[0]);
						if (file_open != FR_OK)
							filename[0] = 0;
					}
					else {
	//					if (file_open == FR_OK) {
							printf("file open %s\n\r", filename);
							strcpy(RW_Buffer, "Hello! From STM32 To SD Card Over SPI, Using f_write()\r\n");
							FR_Status = f_write(&Fil, RW_Buffer, strlen(RW_Buffer), &WWC);
							if (strlen(RW_Buffer) == WWC){
								printf("line written\n\r");
								FR_Status = f_sync(&Fil);
								if (FR_Status == FR_OK)
									printf("sync successfull\n\r");
								else{
									printf("sync FAILED!!!!!!!\n\r");
									disk_mounted = FR_Status;

								}

							}
							else{
								printf("line addition failed.\n\r");
								printf("RW Buffer len%d, wwc %d", strlen(RW_Buffer), WWC);
								disk_mounted = FR_INT_ERR;
	//							file_open = 1;
							}

	//
	//					}
	//					else {
	//
	//					}

					}
				printf("DISK STATUS = %d\n\r", disk_mounted);
	//			printf("FILE STATUS = %d\n\r\n\r", file_open);

				}

}



FRESULT sd_create_log_file(char* filename){
    uint16_t index = 0;
//    char filename[32];
    FRESULT res;

    uint16_t left_index = 0;
    uint16_t right_index = 10000;
    while(left_index < right_index) {
    	int mid = left_index + ((right_index - left_index) >> 1);
    	sprintf(filename, "log%04u.bin", mid);  // log0000.bin, log0001.bin ...
    	res = f_stat(filename, NULL);             // check if file exists
    	if (res == FR_OK){
    		left_index = mid + 1;
    		left_filename_index = left_index;
    	}
    	else {
    		right_index = mid;
    		right_filename_index = right_index;
    	}
    }
    index = left_index ;


    do {
        sprintf(filename, "log%04u.bin", index);  // log0000.bin, log0001.bin ...
        res = f_stat(filename, NULL);             // check if file exists
        index++;
    } while (res == FR_OK && index < 10000);     // stop if too many files
    res = f_open(&Fil, filename, FA_CREATE_NEW | FA_WRITE);
    if (res == FR_OK) {
        printf("Created new log file: %s\n", filename);
    f_expand(&Fil, 16*1024*1024, 1);
//        file_creation_ok = 1;
    } else {
//    	file_creation_ok = 0;
        printf("Failed to create log file, error: %d\n", res);
    }
//    f_close(&logFile);
    return res;
}


void tim5_trigger(){
//	tim2_profiler.start();
	micros_overflow++;
//	tim2_profiler.end();
}

uint64_t micros(){
	return (micros_overflow<<32) + __HAL_TIM_GET_COUNTER(&htim5);
}
