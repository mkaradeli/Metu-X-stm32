################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32h730vbtx.s 

S_DEPS += \
./Core/Startup/startup_stm32h730vbtx.d 

OBJS += \
./Core/Startup/startup_stm32h730vbtx.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m7 -c -I../Core/Inc -I../FATFS/Target -I../FATFS/App -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FatFs/src -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"/Users/karadeli/Documents/STM32CubeIDE/workspace_1.19.0/metux-test/app/IMU" -I"/Users/karadeli/Documents/STM32CubeIDE/workspace_1.19.0/metux-test/app/sh2" -I"/Users/karadeli/Documents/STM32CubeIDE/workspace_1.19.0/metux-test/app" -I"/Users/karadeli/Documents/STM32CubeIDE/workspace_1.19.0/metux-test/app/Lidar" -I"/Users/karadeli/Documents/STM32CubeIDE/workspace_1.19.0/metux-test/app/Motor" -I"/Users/karadeli/Documents/STM32CubeIDE/workspace_1.19.0/metux-test/app/PressureSensor" -I"/Users/karadeli/Documents/STM32CubeIDE/workspace_1.19.0/metux-test/app/SDCard" -I"/Users/karadeli/Documents/STM32CubeIDE/workspace_1.19.0/metux-test/app/SDCard/inc" -I"/Users/karadeli/Documents/STM32CubeIDE/workspace_1.19.0/metux-test/app/Controller/inc" -I"/Users/karadeli/Documents/STM32CubeIDE/workspace_1.19.0/metux-test/app/encoder" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32h730vbtx.d ./Core/Startup/startup_stm32h730vbtx.o

.PHONY: clean-Core-2f-Startup

