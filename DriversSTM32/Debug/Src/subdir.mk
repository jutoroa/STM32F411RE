################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/AdcDriver.c \
../Src/CaptureFrecDriver.c \
../Src/EXTIxDriver.c \
../Src/FPUDriver.c \
../Src/GPIOxDriver.c \
../Src/I2CxDriver.c \
../Src/I2sDriver.c \
../Src/MPU6050Driver.c \
../Src/OLEDDriver.c \
../Src/PLLDriver.c \
../Src/PWMxDriver.c \
../Src/RTCDriver.c \
../Src/SpiDriver.c \
../Src/SysTickDriver.c \
../Src/TIMxDriver.c \
../Src/USARTxDriver.c \
../Src/rtcDS1307Driver.c 

OBJS += \
./Src/AdcDriver.o \
./Src/CaptureFrecDriver.o \
./Src/EXTIxDriver.o \
./Src/FPUDriver.o \
./Src/GPIOxDriver.o \
./Src/I2CxDriver.o \
./Src/I2sDriver.o \
./Src/MPU6050Driver.o \
./Src/OLEDDriver.o \
./Src/PLLDriver.o \
./Src/PWMxDriver.o \
./Src/RTCDriver.o \
./Src/SpiDriver.o \
./Src/SysTickDriver.o \
./Src/TIMxDriver.o \
./Src/USARTxDriver.o \
./Src/rtcDS1307Driver.o 

C_DEPS += \
./Src/AdcDriver.d \
./Src/CaptureFrecDriver.d \
./Src/EXTIxDriver.d \
./Src/FPUDriver.d \
./Src/GPIOxDriver.d \
./Src/I2CxDriver.d \
./Src/I2sDriver.d \
./Src/MPU6050Driver.d \
./Src/OLEDDriver.d \
./Src/PLLDriver.d \
./Src/PWMxDriver.d \
./Src/RTCDriver.d \
./Src/SpiDriver.d \
./Src/SysTickDriver.d \
./Src/TIMxDriver.d \
./Src/USARTxDriver.d \
./Src/rtcDS1307Driver.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F411RE -DSTM32 -DSTM32F4 -DSTM32F411RETx -DSTM32F411xE -c -I"/home/jutoroa/workspace/STMDirectory/DriversSTM32/Inc" -I/home/jutoroa/workspace/CMSIS_REPO/STM32CubeF4-master/Drivers/CMSIS/Core/Include -I/home/jutoroa/workspace/CMSIS_REPO/STM32CubeF4-master/Drivers/CMSIS/Device/ST/STM32F4xx/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/AdcDriver.d ./Src/AdcDriver.o ./Src/AdcDriver.su ./Src/CaptureFrecDriver.d ./Src/CaptureFrecDriver.o ./Src/CaptureFrecDriver.su ./Src/EXTIxDriver.d ./Src/EXTIxDriver.o ./Src/EXTIxDriver.su ./Src/FPUDriver.d ./Src/FPUDriver.o ./Src/FPUDriver.su ./Src/GPIOxDriver.d ./Src/GPIOxDriver.o ./Src/GPIOxDriver.su ./Src/I2CxDriver.d ./Src/I2CxDriver.o ./Src/I2CxDriver.su ./Src/I2sDriver.d ./Src/I2sDriver.o ./Src/I2sDriver.su ./Src/MPU6050Driver.d ./Src/MPU6050Driver.o ./Src/MPU6050Driver.su ./Src/OLEDDriver.d ./Src/OLEDDriver.o ./Src/OLEDDriver.su ./Src/PLLDriver.d ./Src/PLLDriver.o ./Src/PLLDriver.su ./Src/PWMxDriver.d ./Src/PWMxDriver.o ./Src/PWMxDriver.su ./Src/RTCDriver.d ./Src/RTCDriver.o ./Src/RTCDriver.su ./Src/SpiDriver.d ./Src/SpiDriver.o ./Src/SpiDriver.su ./Src/SysTickDriver.d ./Src/SysTickDriver.o ./Src/SysTickDriver.su ./Src/TIMxDriver.d ./Src/TIMxDriver.o ./Src/TIMxDriver.su ./Src/USARTxDriver.d ./Src/USARTxDriver.o ./Src/USARTxDriver.su ./Src/rtcDS1307Driver.d ./Src/rtcDS1307Driver.o ./Src/rtcDS1307Driver.su

.PHONY: clean-Src

