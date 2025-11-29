################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app/sh2/euler.c \
../app/sh2/sh2.c \
../app/sh2/sh2_SensorValue.c \
../app/sh2/sh2_util.c \
../app/sh2/shtp.c 

C_DEPS += \
./app/sh2/euler.d \
./app/sh2/sh2.d \
./app/sh2/sh2_SensorValue.d \
./app/sh2/sh2_util.d \
./app/sh2/shtp.d 

OBJS += \
./app/sh2/euler.o \
./app/sh2/sh2.o \
./app/sh2/sh2_SensorValue.o \
./app/sh2/sh2_util.o \
./app/sh2/shtp.o 


# Each subdirectory must supply rules for building sources it contributes
app/sh2/%.o app/sh2/%.su app/sh2/%.cyclo: ../app/sh2/%.c app/sh2/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H730xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/IMU" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/sh2" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/Lidar" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/Motor" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/PressureSensor" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/SDCard" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/SDCard/inc" -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/Controller/inc" -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-app-2f-sh2

clean-app-2f-sh2:
	-$(RM) ./app/sh2/euler.cyclo ./app/sh2/euler.d ./app/sh2/euler.o ./app/sh2/euler.su ./app/sh2/sh2.cyclo ./app/sh2/sh2.d ./app/sh2/sh2.o ./app/sh2/sh2.su ./app/sh2/sh2_SensorValue.cyclo ./app/sh2/sh2_SensorValue.d ./app/sh2/sh2_SensorValue.o ./app/sh2/sh2_SensorValue.su ./app/sh2/sh2_util.cyclo ./app/sh2/sh2_util.d ./app/sh2/sh2_util.o ./app/sh2/sh2_util.su ./app/sh2/shtp.cyclo ./app/sh2/shtp.d ./app/sh2/shtp.o ./app/sh2/shtp.su

.PHONY: clean-app-2f-sh2

