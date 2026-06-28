################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../External/Src/DCF77.c \
../External/Src/DHT22.c \
../External/Src/DS3231.c \
../External/Src/MCP23S17.c \
../External/Src/VFDDisplay.c 

OBJS += \
./External/Src/DCF77.o \
./External/Src/DHT22.o \
./External/Src/DS3231.o \
./External/Src/MCP23S17.o \
./External/Src/VFDDisplay.o 

C_DEPS += \
./External/Src/DCF77.d \
./External/Src/DHT22.d \
./External/Src/DS3231.d \
./External/Src/MCP23S17.d \
./External/Src/VFDDisplay.d 


# Each subdirectory must supply rules for building sources it contributes
External/Src/%.o External/Src/%.su External/Src/%.cyclo: ../External/Src/%.c External/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F030x6 -c -I../Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -I"D:/Munka/NJE/IV6-digital-clock/IV6_Clock/External/Inc" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-External-2f-Src

clean-External-2f-Src:
	-$(RM) ./External/Src/DCF77.cyclo ./External/Src/DCF77.d ./External/Src/DCF77.o ./External/Src/DCF77.su ./External/Src/DHT22.cyclo ./External/Src/DHT22.d ./External/Src/DHT22.o ./External/Src/DHT22.su ./External/Src/DS3231.cyclo ./External/Src/DS3231.d ./External/Src/DS3231.o ./External/Src/DS3231.su ./External/Src/MCP23S17.cyclo ./External/Src/MCP23S17.d ./External/Src/MCP23S17.o ./External/Src/MCP23S17.su ./External/Src/VFDDisplay.cyclo ./External/Src/VFDDisplay.d ./External/Src/VFDDisplay.o ./External/Src/VFDDisplay.su

.PHONY: clean-External-2f-Src

