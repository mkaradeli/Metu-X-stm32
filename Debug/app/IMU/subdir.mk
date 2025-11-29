################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app/IMU/IMU.c \
../app/IMU/spi_hal.c 

C_DEPS += \
./app/IMU/IMU.d \
./app/IMU/spi_hal.d 

OBJS += \
./app/IMU/IMU.o \
./app/IMU/spi_hal.o 


# Each subdirectory must supply rules for building sources it contributes
app/IMU/%.o app/IMU/%.su app/IMU/%.cyclo: ../app/IMU/%.c app/IMU/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H730xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/IMU" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/sh2" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/Lidar" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/Motor" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/PressureSensor" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/SDCard" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/SDCard/inc" -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/Controller/inc" -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-app-2f-IMU

clean-app-2f-IMU:
	-$(RM) ./app/IMU/IMU.cyclo ./app/IMU/IMU.d ./app/IMU/IMU.o ./app/IMU/IMU.su ./app/IMU/spi_hal.cyclo ./app/IMU/spi_hal.d ./app/IMU/spi_hal.o ./app/IMU/spi_hal.su

.PHONY: clean-app-2f-IMU

