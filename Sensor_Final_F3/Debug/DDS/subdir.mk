################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DDS/AD9833.c \
../DDS/AD9914.c 

OBJS += \
./DDS/AD9833.o \
./DDS/AD9914.o 

C_DEPS += \
./DDS/AD9833.d \
./DDS/AD9914.d 


# Each subdirectory must supply rules for building sources it contributes
DDS/%.o DDS/%.su DDS/%.cyclo: ../DDS/%.c DDS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"D:/St Workspace/Sensor_Final_F3/DDS" -I"D:/St Workspace/Sensor_Final_F3/Radio_Drivers" -I"D:/St Workspace/Sensor_Final_F3/Sensor_Drivers" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-DDS

clean-DDS:
	-$(RM) ./DDS/AD9833.cyclo ./DDS/AD9833.d ./DDS/AD9833.o ./DDS/AD9833.su ./DDS/AD9914.cyclo ./DDS/AD9914.d ./DDS/AD9914.o ./DDS/AD9914.su

.PHONY: clean-DDS

