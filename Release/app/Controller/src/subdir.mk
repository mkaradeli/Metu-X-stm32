################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app/Controller/src/metux_model_6dof_only_controller.c \
../app/Controller/src/metux_model_6dof_only_controller_data.c 

C_DEPS += \
./app/Controller/src/metux_model_6dof_only_controller.d \
./app/Controller/src/metux_model_6dof_only_controller_data.d 

OBJS += \
./app/Controller/src/metux_model_6dof_only_controller.o \
./app/Controller/src/metux_model_6dof_only_controller_data.o 


# Each subdirectory must supply rules for building sources it contributes
app/Controller/src/%.o app/Controller/src/%.su app/Controller/src/%.cyclo: ../app/Controller/src/%.c app/Controller/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H730xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/alica/OneDrive/Desktop/metux-launch/app/IMU" -I"C:/Users/alica/OneDrive/Desktop/metux-launch/app/sh2" -I"C:/Users/alica/OneDrive/Desktop/metux-launch/app" -I"C:/Users/alica/OneDrive/Desktop/metux-launch/app/Lidar" -I"C:/Users/alica/OneDrive/Desktop/metux-launch/app/Motor" -I"C:/Users/alica/OneDrive/Desktop/metux-launch/app/PressureSensor" -I"C:/Users/alica/OneDrive/Desktop/metux-launch/app/SDCard" -I"C:/Users/alica/OneDrive/Desktop/metux-launch/app/SDCard/inc" -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/alica/OneDrive/Desktop/metux-launch/app/Controller/inc" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-app-2f-Controller-2f-src

clean-app-2f-Controller-2f-src:
	-$(RM) ./app/Controller/src/metux_model_6dof_only_controller.cyclo ./app/Controller/src/metux_model_6dof_only_controller.d ./app/Controller/src/metux_model_6dof_only_controller.o ./app/Controller/src/metux_model_6dof_only_controller.su ./app/Controller/src/metux_model_6dof_only_controller_data.cyclo ./app/Controller/src/metux_model_6dof_only_controller_data.d ./app/Controller/src/metux_model_6dof_only_controller_data.o ./app/Controller/src/metux_model_6dof_only_controller_data.su

.PHONY: clean-app-2f-Controller-2f-src

