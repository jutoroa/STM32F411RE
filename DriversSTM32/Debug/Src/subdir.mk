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
../Src/MPU6050Driver.c \
../Src/OLEDDriver.c \
../Src/PWMxDriver.c \
../Src/SysTickDriver.c \
../Src/TIMxDriver.c \
../Src/USARTxDriver.c \
../Src/rtcDS1307Driver.c 

OBJS += \
./Src/AdcDriver.o \
./Src/EXTIxDriver.o \
./Src/GPIOxDriver.o \
./Src/I2CxDriver.o \
./Src/MPU6050Driver.o \
./Src/OLEDDriver.o \
./Src/PWMxDriver.o \
./Src/SysTickDriver.o \
./Src/TIMxDriver.o \
./Src/USARTxDriver.o \
./Src/rtcDS1307Driver.o 

C_DEPS += \
./Src/AdcDriver.d \
./Src/EXTIxDriver.d \
./Src/GPIOxDriver.d \
./Src/I2CxDriver.d \
./Src/MPU6050Driver.d \
./Src/OLEDDriver.d \
./Src/PWMxDriver.d \
./Src/SysTickDriver.d \
./Src/TIMxDriver.d \
./Src/USARTxDriver.d \
./Src/rtcDS1307Driver.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F411RE -DSTM32 -DSTM32F4 -DSTM32F411RETx -DSTM32F411xE -c -I../Inc -I/home/jutoroa/workspace/STMDirectory/mcu_headers/CMSIS/Include -I/home/jutoroa/workspace/STMDirectory/mcu_headers/CMSIS/Device/ST/STM32F4xx/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/AdcDriver.d ./Src/AdcDriver.o ./Src/AdcDriver.su ./Src/EXTIxDriver.d ./Src/EXTIxDriver.o ./Src/EXTIxDriver.su ./Src/GPIOxDriver.d ./Src/GPIOxDriver.o ./Src/GPIOxDriver.su ./Src/I2CxDriver.d ./Src/I2CxDriver.o ./Src/I2CxDriver.su ./Src/MPU6050Driver.d ./Src/MPU6050Driver.o ./Src/MPU6050Driver.su ./Src/OLEDDriver.d ./Src/OLEDDriver.o ./Src/OLEDDriver.su ./Src/PWMxDriver.d ./Src/PWMxDriver.o ./Src/PWMxDriver.su ./Src/SysTickDriver.d ./Src/SysTickDriver.o ./Src/SysTickDriver.su ./Src/TIMxDriver.d ./Src/TIMxDriver.o ./Src/TIMxDriver.su ./Src/USARTxDriver.d ./Src/USARTxDriver.o ./Src/USARTxDriver.su ./Src/rtcDS1307Driver.d ./Src/rtcDS1307Driver.o ./Src/rtcDS1307Driver.su

.PHONY: clean-Src

