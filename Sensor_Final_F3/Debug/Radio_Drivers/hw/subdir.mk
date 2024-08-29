################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Radio_Drivers/hw/board_gpio.c \
../Radio_Drivers/hw/board_spi.c 

OBJS += \
./Radio_Drivers/hw/board_gpio.o \
./Radio_Drivers/hw/board_spi.o 

C_DEPS += \
./Radio_Drivers/hw/board_gpio.d \
./Radio_Drivers/hw/board_spi.d 


# Each subdirectory must supply rules for building sources it contributes
Radio_Drivers/hw/%.o Radio_Drivers/hw/%.su Radio_Drivers/hw/%.cyclo: ../Radio_Drivers/hw/%.c Radio_Drivers/hw/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"D:/St Workspace/Sensor_Final_F3/DDS" -I"D:/St Workspace/Sensor_Final_F3/Radio_Drivers" -I"D:/St Workspace/Sensor_Final_F3/Sensor_Drivers" -I"D:/Git tools/DielectricSpectrumSoilData-CommInterface/Sensor_Final_F3/DDS" -I"D:/Git tools/DielectricSpectrumSoilData-CommInterface/Sensor_Final_F3/Radio_Drivers" -I"D:/Git tools/DielectricSpectrumSoilData-CommInterface/Sensor_Final_F3/Sensor_Drivers" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Radio_Drivers-2f-hw

clean-Radio_Drivers-2f-hw:
	-$(RM) ./Radio_Drivers/hw/board_gpio.cyclo ./Radio_Drivers/hw/board_gpio.d ./Radio_Drivers/hw/board_gpio.o ./Radio_Drivers/hw/board_gpio.su ./Radio_Drivers/hw/board_spi.cyclo ./Radio_Drivers/hw/board_spi.d ./Radio_Drivers/hw/board_spi.o ./Radio_Drivers/hw/board_spi.su

.PHONY: clean-Radio_Drivers-2f-hw

