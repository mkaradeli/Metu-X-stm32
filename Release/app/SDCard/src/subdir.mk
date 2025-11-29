################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app/SDCard/src/sd_diskio_spi.c \
../app/SDCard/src/sd_functions.c \
../app/SDCard/src/sd_spi.c 

C_DEPS += \
./app/SDCard/src/sd_diskio_spi.d \
./app/SDCard/src/sd_functions.d \
./app/SDCard/src/sd_spi.d 

OBJS += \
./app/SDCard/src/sd_diskio_spi.o \
./app/SDCard/src/sd_functions.o \
./app/SDCard/src/sd_spi.o 


# Each subdirectory must supply rules for building sources it contributes
app/SDCard/src/%.o app/SDCard/src/%.su app/SDCard/src/%.cyclo: ../app/SDCard/src/%.c app/SDCard/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H730xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/alica/OneDrive/Desktop/metux-launch/app/IMU" -I"C:/Users/alica/OneDrive/Desktop/metux-launch/app/sh2" -I"C:/Users/alica/OneDrive/Desktop/metux-launch/app" -I"C:/Users/alica/OneDrive/Desktop/metux-launch/app/Lidar" -I"C:/Users/alica/OneDrive/Desktop/metux-launch/app/Motor" -I"C:/Users/alica/OneDrive/Desktop/metux-launch/app/PressureSensor" -I"C:/Users/alica/OneDrive/Desktop/metux-launch/app/SDCard" -I"C:/Users/alica/OneDrive/Desktop/metux-launch/app/SDCard/inc" -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/alica/OneDrive/Desktop/metux-launch/app/Controller/inc" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-app-2f-SDCard-2f-src

clean-app-2f-SDCard-2f-src:
	-$(RM) ./app/SDCard/src/sd_diskio_spi.cyclo ./app/SDCard/src/sd_diskio_spi.d ./app/SDCard/src/sd_diskio_spi.o ./app/SDCard/src/sd_diskio_spi.su ./app/SDCard/src/sd_functions.cyclo ./app/SDCard/src/sd_functions.d ./app/SDCard/src/sd_functions.o ./app/SDCard/src/sd_functions.su ./app/SDCard/src/sd_spi.cyclo ./app/SDCard/src/sd_spi.d ./app/SDCard/src/sd_spi.o ./app/SDCard/src/sd_spi.su

.PHONY: clean-app-2f-SDCard-2f-src

