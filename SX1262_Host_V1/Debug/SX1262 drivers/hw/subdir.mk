################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SX1262\ drivers/hw/board_gpio.c \
../SX1262\ drivers/hw/board_spi.c \
../SX1262\ drivers/hw/board_usart.c 

OBJS += \
./SX1262\ drivers/hw/board_gpio.o \
./SX1262\ drivers/hw/board_spi.o \
./SX1262\ drivers/hw/board_usart.o 

C_DEPS += \
./SX1262\ drivers/hw/board_gpio.d \
./SX1262\ drivers/hw/board_spi.d \
./SX1262\ drivers/hw/board_usart.d 


# Each subdirectory must supply rules for building sources it contributes
SX1262\ drivers/hw/board_gpio.o: ../SX1262\ drivers/hw/board_gpio.c SX1262\ drivers/hw/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xE -c -I../Core/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I"D:/St Workspace/SX1262_Host_V1/SX1262 drivers" -I"D:/St Workspace/SX1262_Host_V1/SX1262 drivers/hw" -I"D:/St Workspace/SX1262_Host_V1/SX1262 drivers/sx126x" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"SX1262 drivers/hw/board_gpio.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
SX1262\ drivers/hw/board_spi.o: ../SX1262\ drivers/hw/board_spi.c SX1262\ drivers/hw/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xE -c -I../Core/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I"D:/St Workspace/SX1262_Host_V1/SX1262 drivers" -I"D:/St Workspace/SX1262_Host_V1/SX1262 drivers/hw" -I"D:/St Workspace/SX1262_Host_V1/SX1262 drivers/sx126x" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"SX1262 drivers/hw/board_spi.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
SX1262\ drivers/hw/board_usart.o: ../SX1262\ drivers/hw/board_usart.c SX1262\ drivers/hw/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xE -c -I../Core/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I"D:/St Workspace/SX1262_Host_V1/SX1262 drivers" -I"D:/St Workspace/SX1262_Host_V1/SX1262 drivers/hw" -I"D:/St Workspace/SX1262_Host_V1/SX1262 drivers/sx126x" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"SX1262 drivers/hw/board_usart.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-SX1262-20-drivers-2f-hw

clean-SX1262-20-drivers-2f-hw:
	-$(RM) ./SX1262\ drivers/hw/board_gpio.cyclo ./SX1262\ drivers/hw/board_gpio.d ./SX1262\ drivers/hw/board_gpio.o ./SX1262\ drivers/hw/board_gpio.su ./SX1262\ drivers/hw/board_spi.cyclo ./SX1262\ drivers/hw/board_spi.d ./SX1262\ drivers/hw/board_spi.o ./SX1262\ drivers/hw/board_spi.su ./SX1262\ drivers/hw/board_usart.cyclo ./SX1262\ drivers/hw/board_usart.d ./SX1262\ drivers/hw/board_usart.o ./SX1262\ drivers/hw/board_usart.su

.PHONY: clean-SX1262-20-drivers-2f-hw

