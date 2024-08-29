################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sensor_Drivers/Sensor_Drivers.c 

OBJS += \
./Sensor_Drivers/Sensor_Drivers.o 

C_DEPS += \
./Sensor_Drivers/Sensor_Drivers.d 


# Each subdirectory must supply rules for building sources it contributes
Sensor_Drivers/%.o Sensor_Drivers/%.su Sensor_Drivers/%.cyclo: ../Sensor_Drivers/%.c Sensor_Drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"D:/St Workspace/Sensor_Final_F3/DDS" -I"D:/St Workspace/Sensor_Final_F3/Radio_Drivers" -I"D:/St Workspace/Sensor_Final_F3/Sensor_Drivers" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Sensor_Drivers

clean-Sensor_Drivers:
	-$(RM) ./Sensor_Drivers/Sensor_Drivers.cyclo ./Sensor_Drivers/Sensor_Drivers.d ./Sensor_Drivers/Sensor_Drivers.o ./Sensor_Drivers/Sensor_Drivers.su

.PHONY: clean-Sensor_Drivers

