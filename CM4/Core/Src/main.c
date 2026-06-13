/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "shared_memory.h"
#include "ring_buffer.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "task_timer.h"
//#include <stdbool.h>

//#include "../../Ap/FATFS_SD/FATFS_SD.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* DUAL_CORE_BOOT_SYNC_SEQUENCE: Define for dual core boot synchronization    */
/*                             demonstration code based on hardware semaphore */
/* This define is present in both CM7/CM4 projects                            */
/* To comment when developping/debugging on a single core                     */
#define DUAL_CORE_BOOT_SYNC_SEQUENCE

#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif
#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
COM_InitTypeDef BspCOMInit;
//timeOfLastToggleForGreen
uint32_t timeOfLastToggleForGreen=false;
//__attribute__((section(".shared_memory"), used))
//SensorData_t sensor_data_buffer_a[BUFFER_PACKET_COUNT/2-1];
//__attribute__((section(".shared_memory"), used))
//SensorData_t sensor_data_buffer_b[BUFFER_PACKET_COUNT/2-1];
//__attribute__((section(".shared_memory"), used))
//uint8_t ready_to_write_a;
//__attribute__((section(".shared_memory"), used))
//uint8_t ready_to_write_b;
//SensorData_t sensor_data;



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void SD_Card_Test(void);
void LED_Counter_Tick(void);

FATFS FatFs;
FIL Fil;
FRESULT FR_Status;
FATFS *FS_Ptr;
UINT RWC, WWC; // Read/Write Word Counter
DWORD FreeClusters;
uint32_t TotalSize, FreeSpace;


//static void SD_Card_Test(void);
char TxBuffer[250];
int left_filename_index = 0;
int right_filename_index = 0;
FIL logFile;
FATFS FatFs;
int sd_create_log_file(void);
static int sd_mount();

FRESULT disk_mounted= FR_DISK_ERR;
FRESULT file_open = FR_DISK_ERR;



bool disk_mounted = false;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define TX_RING_SZ 1024
static volatile uint8_t  tx_ring[TX_RING_SZ];
static volatile uint16_t tx_head = 0;   // written by _write
static volatile uint16_t tx_tail = 0;   // advanced when DMA completes
static volatile uint8_t  tx_busy = 0;

task_timer_t heartbeat_task = {100, 0}; // period ms, start ms
task_timer_t printf_task = {1000, 0};
//task_timer_t sd_ = {100, 0}; // period ms, start ms

//static uint8_t  uart_tx_buf[256];
static volatile bool uart_tx_busy = false;


int _write(int file, char *ptr, int len)
{
//	SCB_InvalidateDCache_by_Addr((uint32_t *)tail, sizeof(tail));
    (void)file;
    return (int)rb_push_n(ptr, (size_t)len);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

/* USER CODE BEGIN Boot_Mode_Sequence_1 */
#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
  /*HW semaphore Clock enable*/
  __HAL_RCC_HSEM_CLK_ENABLE();
  /* Activate HSEM notification for Cortex-M4*/
  HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));
  /*
  Domain D2 goes to STOP mode (Cortex-M4 in deep-sleep) waiting for Cortex-M7 to
  perform system initialization (system clock config, external memory configuration.. )
  */
  HAL_PWREx_ClearPendingEvent();
  HAL_PWREx_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE, PWR_D2_DOMAIN);
  /* Clear HSEM flag */
  __HAL_HSEM_CLEAR_FLAG(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));

#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */
/* USER CODE END Boot_Mode_Sequence_1 */
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_DMA_Init();
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();
  MX_I2C1_Init();
  MX_SPI2_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  BSP_LED_Init(LED_YELLOW);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//  uint32_t local_timestep = 0;

  BspCOMInit.BaudRate   = 115200;
    BspCOMInit.WordLength = COM_WORDLENGTH_8B;
    BspCOMInit.StopBits   = COM_STOPBITS_1;
    BspCOMInit.Parity     = COM_PARITY_NONE;
    BspCOMInit.HwFlowCtl  = COM_HWCONTROL_NONE;
    if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }

    BSP_LED_On(LED_YELLOW);

//    while (1) {};
//    sd_mount();
  while (1)
  {
	  ready_to_write_b++;
//	  printf("test test test\n\r");
//	  HAL_Delay(1000);
//	  local_timestep ++;
//	if (timeOfLastToggleForGreen<=uwTick+0.1e3){
//	          	timeOfLastToggleForGreen+= 0.1e3;
//	          	BSP_LED_Toggle(LED_GREEN);
//	  }
	if (task_ready(&heartbeat_task))
		LED_Counter_Tick();
	  //        	time_sec = uwTick);

//	          }
//	if (rb_count()){
//		printf_task.last_trigger = uwTick;
//	}
	if (rb_count() || task_ready(&printf_task)) {
//			HAL_Delay(100);
			printf("%d %d , %d\n\r", rb_count(), head, tail);
			printf("%d, %d\n\r",uwTick, timeOfLastToggleForGreen);

			if (disk_mounted != FR_OK){
				FR_Status = f_mount(&FatFs, "", 1);
					if (FR_Status != FR_OK)
					  printf("Error! While Mounting SD Card, Error Code: (%i)\r\n", FR_Status);
					else
						printf("SD Card Mounted Successfully! \r\n\n");
					disk_mounted = FR_Status;
			}
			else {
				if (file_opened == FR_OK) {

				}
			}

//			if (file_opened)


			FR_Status = f_mount(&FatFs, "", 1);
			    if (FR_Status != FR_OK)
			    {
			      printf("Error! While Mounting SD Card, Error Code: (%i)\r\n", FR_Status);
			      // UART_Print(TxBuffer);
//			      break;
			    }
			    else {
					printf("SD Card Mounted Successfully! \r\n\n");

					f_getfree("", &FreeClusters, &FS_Ptr);
					TotalSize = (uint32_t)((FS_Ptr->n_fatent - 2) * FS_Ptr->csize * 0.5);
					FreeSpace = (uint32_t)(FreeClusters * FS_Ptr->csize * 0.5);
					printf("Total SD Card Size: %lu Bytes\r\n", TotalSize);
					// UART_Print(TxBuffer);
					printf("Free SD Card Space: %lu Bytes\r\n\n", FreeSpace);
					sd_create_log_file();

					 FR_Status = f_mount(NULL, "", 0);
					  if (FR_Status != FR_OK)
					  {
					      printf("Error! While Un-mounting SD Card, Error Code: (%i)\r\n", FR_Status);
					      // UART_Print(TxBuffer);
					  } else{
					      printf("SD Card Un-mounted Successfully! \r\n");
					      // UART_Print(TxBuffer);
					  }

			    }



			rb_flush();
//			BSP_LED_On(LED_GREEN);
//			SD_Card_Test();
//			if (!disk_mounted) {
//				FRESULT FR_Status;
//				FR_Status = sd_mount();
//				if (!FR_Status)
//					printf("Error! While Mounting SD Card, Error Code: (%i)\r\n", FR_Status);
//				else
//					disk_mounted = true;

//			}
//			if (sd_mount() == FR_OK) {
//			if (disk_mounted){
//				int res = sd_create_log_file();
//			printf("%d\n\r", res);

//			}
//				f_mount(NULL, "", 1);



	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI2|RCC_PERIPHCLK_SPI1;
  PeriphClkInitStruct.PLL2.PLL2M = 4;
  PeriphClkInitStruct.PLL2.PLL2N = 25;
  PeriphClkInitStruct.PLL2.PLL2P = 8;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


static int sd_mount(){
	FATFS FatFs;
	FRESULT FR_Status;
	FR_Status = f_mount(&FatFs, "", 1);
	if (FR_Status != FR_OK)
	{
		printf("Error! While Mounting SD Card, Error Code: (%i)\r\n", FR_Status);
		return FR_Status;
	}
	return FR_Status;


}


static void SD_Card_Test(void)
{
  FATFS FatFs;
  FIL Fil;
  FRESULT FR_Status;
  FATFS *FS_Ptr;
  UINT RWC, WWC; // Read/Write Word Counter
  DWORD FreeClusters;
  uint32_t TotalSize, FreeSpace;
  char RW_Buffer[200];
  do
  {
    //------------------[ Mount The SD Card ]--------------------
    FR_Status = f_mount(&FatFs, "", 1);
    if (FR_Status != FR_OK)
    {
      printf("Error! While Mounting SD Card, Error Code: (%i)\r\n", FR_Status);
      // UART_Print(TxBuffer);
      break;
    }
    printf("SD Card Mounted Successfully! \r\n\n");
    // UART_Print(TxBuffer);
    //------------------[ Get & Print The SD Card Size & Free Space ]--------------------
    f_getfree("", &FreeClusters, &FS_Ptr);
    TotalSize = (uint32_t)((FS_Ptr->n_fatent - 2) * FS_Ptr->csize * 0.5);
    FreeSpace = (uint32_t)(FreeClusters * FS_Ptr->csize * 0.5);
    printf("Total SD Card Size: %lu Bytes\r\n", TotalSize);
    // UART_Print(TxBuffer);
    printf("Free SD Card Space: %lu Bytes\r\n\n", FreeSpace);
    // UART_Print(TxBuffer);
    //------------------[ Open A Text File For Write & Write Data ]--------------------
    //Open the file
			    FR_Status = f_open(&Fil, "TextFileWrite.txt", FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
			    if(FR_Status != FR_OK)
			    {
			      printf("Error! While Creating/Opening A New Text File, Error Code: (%i)\r\n", FR_Status);
			      // UART_Print(TxBuffer);
			      break;
			    }
			    printf("Text File Created & Opened! Writing Data To The Text File..\r\n\n");
			    // UART_Print(TxBuffer);
			    // (1) Write Data To The Text File [ Using f_puts() Function ]
//			    f_puts("Hello! From STM32 To SD Card Over SPI, Using f_puts()\n", &Fil);
			    // (2) Write Data To The Text File [ Using f_write() Function ]
			    strcpy(RW_Buffer, "Hello! From STM32 To SD Card Over SPI, Using f_write()\r\n");
			    f_write(&Fil, RW_Buffer, strlen(RW_Buffer), &WWC);
			    // Close The File
			    f_close(&Fil);
    //------------------[ Open A Text File For Read & Read Its Data ]--------------------
    // Open The File
    FR_Status = f_open(&Fil, "TextFileWrite.txt", FA_READ);
    if(FR_Status != FR_OK)
    {
      printf("Error! While Opening (TextFileWrite.txt) File For Read.. \r\n");
      // UART_Print(TxBuffer);
      break;
    }
    // (1) Read The Text File's Data [ Using f_gets() Function ]
    f_gets(RW_Buffer, sizeof(RW_Buffer), &Fil);
    printf("Data Read From (TextFileWrite.txt) Using f_gets():%s", RW_Buffer);
    // UART_Print(TxBuffer);
    // (2) Read The Text File's Data [ Using f_read() Function ]
    f_read(&Fil, RW_Buffer, f_size(&Fil), &RWC);
    printf("Data Read From (TextFileWrite.txt) Using f_read():%s", RW_Buffer);
    // UART_Print(TxBuffer);
    // Close The File
    f_close(&Fil);
    printf("File Closed! \r\n\n");
    // UART_Print(TxBuffer);
    //------------------[ Open An Existing Text File, Update Its Content, Read It Back ]--------------------
    // (1) Open The Existing File For Write (Update)
    FR_Status = f_open(&Fil, "TextFileWrite.txt", FA_OPEN_EXISTING | FA_WRITE);
    FR_Status = f_lseek(&Fil, f_size(&Fil)); // Move The File Pointer To The EOF (End-Of-File)
    if(FR_Status != FR_OK)
    {
      printf("Error! While Opening (TextFileWrite.txt) File For Update.. \r\n");
      // UART_Print(TxBuffer);
      break;
    }
    // (2) Write New Line of Text Data To The File
    FR_Status = f_puts("This New Line Was Added During Update!\r\n", &Fil);
    f_close(&Fil);
    memset(RW_Buffer,'\0',sizeof(RW_Buffer)); // Clear The Buffer
    // (3) Read The Contents of The Text File After The Update
    FR_Status = f_open(&Fil, "TextFileWrite.txt", FA_READ); // Open The File For Read
    f_read(&Fil, RW_Buffer, f_size(&Fil), &RWC);
    printf("Data Read From (TextFileWrite.txt) After Update:%s", RW_Buffer);
    // UART_Print(TxBuffer);
    f_close(&Fil);
    //------------------[ Delete The Text File ]--------------------
    // Delete The File
    
//    FR_Status = f_unlink(TextFileWrite.txt);
//    if (FR_Status != FR_OK){
//        printf("Error! While Deleting The (TextFileWrite.txt) File.. \r\n");
        // UART_Print(TxBuffer);
//    }
    
  } while(0);
  //------------------[ Test Complete! Unmount The SD Card ]--------------------
  FR_Status = f_mount(NULL, "", 0);
  if (FR_Status != FR_OK)
  {
      printf("Error! While Un-mounting SD Card, Error Code: (%i)\r\n", FR_Status);
      // UART_Print(TxBuffer);
  } else{
      printf("SD Card Un-mounted Successfully! \r\n");
      // UART_Print(TxBuffer);
  }
}

int sd_create_log_file(void){
    uint16_t index = 0;
    char filename[32];
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
    res = f_open(&logFile, filename, FA_CREATE_NEW | FA_WRITE);
    if (res == FR_OK) {
        printf("Created new log file: %s\n", filename);
    f_expand(&logFile, 16*1024*1024, 1);
//        file_creation_ok = 1;
    } else {
//    	file_creation_ok = 0;
        printf("Failed to create log file, error: %d\n", res);
    }
    f_close(&logFile);
    return res;
}

void LED_Counter_Tick(void)
{
	const static uint8_t timing[] = {1, 0, 1, 0, 0, 0, 0};
	static uint8_t index = 0;
	if (timing[index])
		BSP_LED_On(LED_YELLOW);
	else
		BSP_LED_Off(LED_YELLOW);

	index ++;
	index %= 7;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	BSP_LED_On(LED_YELLOW);
	__disable_irq();
//  BSP_LED_On(LED_YELLOW);
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
