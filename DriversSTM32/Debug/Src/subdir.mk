################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/AdcDriver.c \
../Src/EXTIxDriver.c \
../Src/GPIOxDriver.c \
../Src/I2CxDriver.c \
../Src/PWMxDriver.c \
../Src/SysTickDriver.c \
../Src/TIMxDriver.c \
../Src/USARTxDriver.c 

OBJS += \
./Src/AdcDriver.o \
./Src/EXTIxDriver.o \
./Src/GPIOxDriver.o \
./Src/I2CxDriver.o \
./Src/PWMxDriver.o \
./Src/SysTickDriver.o \
./Src/TIMxDriver.o \
./Src/USARTxDriver.o 

C_DEPS += \
./Src/AdcDriver.d \
./Src/EXTIxDriver.d \
./Src/GPIOxDriver.d \
./Src/I2CxDriver.d \
./Src/PWMxDriver.d \
./Src/SysTickDriver.d \
./Src/TIMxDriver.d \
./Src/USARTxDriver.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F411RE -DSTM32 -DSTM32F4 -DSTM32F411RETx -DSTM32F411xE -c -I../Inc -I/home/jutoroa/workspace/STMDirectory/mcu_headers/CMSIS/Include -I/home/jutoroa/workspace/STMDirectory/mcu_headers/CMSIS/Device/ST/STM32F4xx/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/AdcDriver.d ./Src/AdcDriver.o ./Src/AdcDriver.su ./Src/EXTIxDriver.d ./Src/EXTIxDriver.o ./Src/EXTIxDriver.su ./Src/GPIOxDriver.d ./Src/GPIOxDriver.o ./Src/GPIOxDriver.su ./Src/I2CxDriver.d ./Src/I2CxDriver.o ./Src/I2CxDriver.su ./Src/PWMxDriver.d ./Src/PWMxDriver.o ./Src/PWMxDriver.su ./Src/SysTickDriver.d ./Src/SysTickDriver.o ./Src/SysTickDriver.su ./Src/TIMxDriver.d ./Src/TIMxDriver.o ./Src/TIMxDriver.su ./Src/USARTxDriver.d ./Src/USARTxDriver.o ./Src/USARTxDriver.su

.PHONY: clean-Src

