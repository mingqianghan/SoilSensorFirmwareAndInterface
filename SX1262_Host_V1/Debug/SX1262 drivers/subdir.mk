################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SX1262\ drivers/SX1262_MX_Process.c \
../SX1262\ drivers/sx126x-board.c \
../SX1262\ drivers/utilities.c 

OBJS += \
./SX1262\ drivers/SX1262_MX_Process.o \
./SX1262\ drivers/sx126x-board.o \
./SX1262\ drivers/utilities.o 

C_DEPS += \
./SX1262\ drivers/SX1262_MX_Process.d \
./SX1262\ drivers/sx126x-board.d \
./SX1262\ drivers/utilities.d 


# Each subdirectory must supply rules for building sources it contributes
SX1262\ drivers/SX1262_MX_Process.o: ../SX1262\ drivers/SX1262_MX_Process.c SX1262\ drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xE -c -I../Core/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I"D:/St Workspace/SX1262_Host_V1/SX1262 drivers" -I"D:/St Workspace/SX1262_Host_V1/SX1262 drivers/hw" -I"D:/St Workspace/SX1262_Host_V1/SX1262 drivers/sx126x" -I"D:/Git tools/DielectricSpectrumSoilData-CommInterface/SX1262_Host_V1/SX1262 drivers" -I"D:/Git tools/DielectricSpectrumSoilData-CommInterface/SX1262_Host_V1/SX1262 drivers/sx126x" -I"D:/Git tools/DielectricSpectrumSoilData-CommInterface/SX1262_Host_V1/SX1262 drivers/hw" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"SX1262 drivers/SX1262_MX_Process.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
SX1262\ drivers/sx126x-board.o: ../SX1262\ drivers/sx126x-board.c SX1262\ drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xE -c -I../Core/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I"D:/St Workspace/SX1262_Host_V1/SX1262 drivers" -I"D:/St Workspace/SX1262_Host_V1/SX1262 drivers/hw" -I"D:/St Workspace/SX1262_Host_V1/SX1262 drivers/sx126x" -I"D:/Git tools/DielectricSpectrumSoilData-CommInterface/SX1262_Host_V1/SX1262 drivers" -I"D:/Git tools/DielectricSpectrumSoilData-CommInterface/SX1262_Host_V1/SX1262 drivers/sx126x" -I"D:/Git tools/DielectricSpectrumSoilData-CommInterface/SX1262_Host_V1/SX1262 drivers/hw" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"SX1262 drivers/sx126x-board.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
SX1262\ drivers/utilities.o: ../SX1262\ drivers/utilities.c SX1262\ drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xE -c -I../Core/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I"D:/St Workspace/SX1262_Host_V1/SX1262 drivers" -I"D:/St Workspace/SX1262_Host_V1/SX1262 drivers/hw" -I"D:/St Workspace/SX1262_Host_V1/SX1262 drivers/sx126x" -I"D:/Git tools/DielectricSpectrumSoilData-CommInterface/SX1262_Host_V1/SX1262 drivers" -I"D:/Git tools/DielectricSpectrumSoilData-CommInterface/SX1262_Host_V1/SX1262 drivers/sx126x" -I"D:/Git tools/DielectricSpectrumSoilData-CommInterface/SX1262_Host_V1/SX1262 drivers/hw" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"SX1262 drivers/utilities.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-SX1262-20-drivers

clean-SX1262-20-drivers:
	-$(RM) ./SX1262\ drivers/SX1262_MX_Process.cyclo ./SX1262\ drivers/SX1262_MX_Process.d ./SX1262\ drivers/SX1262_MX_Process.o ./SX1262\ drivers/SX1262_MX_Process.su ./SX1262\ drivers/sx126x-board.cyclo ./SX1262\ drivers/sx126x-board.d ./SX1262\ drivers/sx126x-board.o ./SX1262\ drivers/sx126x-board.su ./SX1262\ drivers/utilities.cyclo ./SX1262\ drivers/utilities.d ./SX1262\ drivers/utilities.o ./SX1262\ drivers/utilities.su

.PHONY: clean-SX1262-20-drivers

