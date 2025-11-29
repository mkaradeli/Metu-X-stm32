################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../app/Motor/Motor.cpp 

OBJS += \
./app/Motor/Motor.o 

CPP_DEPS += \
./app/Motor/Motor.d 


# Each subdirectory must supply rules for building sources it contributes
app/Motor/%.o app/Motor/%.su app/Motor/%.cyclo: ../app/Motor/%.cpp app/Motor/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++17 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H730xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/IMU" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/sh2" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/Lidar" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/Motor" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/PressureSensor" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/SDCard" -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/SDCard/inc" -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"/Users/karadeli/STM32CubeIDE/workspace_1.19.0/metux-test/app/Controller/inc" -Og -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-app-2f-Motor

clean-app-2f-Motor:
	-$(RM) ./app/Motor/Motor.cyclo ./app/Motor/Motor.d ./app/Motor/Motor.o ./app/Motor/Motor.su

.PHONY: clean-app-2f-Motor

