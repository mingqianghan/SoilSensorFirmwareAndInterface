################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Radio_Drivers/sx126x/radio.c \
../Radio_Drivers/sx126x/sx126x.c 

OBJS += \
./Radio_Drivers/sx126x/radio.o \
./Radio_Drivers/sx126x/sx126x.o 

C_DEPS += \
./Radio_Drivers/sx126x/radio.d \
./Radio_Drivers/sx126x/sx126x.d 


# Each subdirectory must supply rules for building sources it contributes
Radio_Drivers/sx126x/%.o Radio_Drivers/sx126x/%.su Radio_Drivers/sx126x/%.cyclo: ../Radio_Drivers/sx126x/%.c Radio_Drivers/sx126x/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"D:/St Workspace/Sensor_Final_F3/DDS" -I"D:/St Workspace/Sensor_Final_F3/Radio_Drivers" -I"D:/St Workspace/Sensor_Final_F3/Sensor_Drivers" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Radio_Drivers-2f-sx126x

clean-Radio_Drivers-2f-sx126x:
	-$(RM) ./Radio_Drivers/sx126x/radio.cyclo ./Radio_Drivers/sx126x/radio.d ./Radio_Drivers/sx126x/radio.o ./Radio_Drivers/sx126x/radio.su ./Radio_Drivers/sx126x/sx126x.cyclo ./Radio_Drivers/sx126x/sx126x.d ./Radio_Drivers/sx126x/sx126x.o ./Radio_Drivers/sx126x/sx126x.su

.PHONY: clean-Radio_Drivers-2f-sx126x

