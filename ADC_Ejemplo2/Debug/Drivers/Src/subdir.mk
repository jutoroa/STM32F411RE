################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Src/AdcDriver.c \
../Drivers/Src/GPIOxDriver.c \
../Drivers/Src/TIMxDriver.c \
../Drivers/Src/USARTxDriver.c 

OBJS += \
./Drivers/Src/AdcDriver.o \
./Drivers/Src/GPIOxDriver.o \
./Drivers/Src/TIMxDriver.o \
./Drivers/Src/USARTxDriver.o 

C_DEPS += \
./Drivers/Src/AdcDriver.d \
./Drivers/Src/GPIOxDriver.d \
./Drivers/Src/TIMxDriver.d \
./Drivers/Src/USARTxDriver.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Src/%.o Drivers/Src/%.su: ../Drivers/Src/%.c Drivers/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F411RE -DSTM32 -DSTM32F4 -DSTM32F411RETx -DSTM32F411xE -c -I../Inc -I"G:/Mi unidad/Unalmed/Taller_5/STM32DIRECTORY/mcu_headers/CMSIS/Device/ST/STM32F4xx/Include" -I"G:/Mi unidad/Unalmed/Taller_5/STM32DIRECTORY/mcu_headers/CMSIS/Include" -I"G:/Mi unidad/Unalmed/Taller_5/STM32DIRECTORY/ADC_Ejemplo2/Drivers/Inc" -I"G:/Mi unidad/Unalmed/Taller_5/STM32DIRECTORY/ADC_Ejemplo2/Drivers/Src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Src

clean-Drivers-2f-Src:
	-$(RM) ./Drivers/Src/AdcDriver.d ./Drivers/Src/AdcDriver.o ./Drivers/Src/AdcDriver.su ./Drivers/Src/GPIOxDriver.d ./Drivers/Src/GPIOxDriver.o ./Drivers/Src/GPIOxDriver.su ./Drivers/Src/TIMxDriver.d ./Drivers/Src/TIMxDriver.o ./Drivers/Src/TIMxDriver.su ./Drivers/Src/USARTxDriver.d ./Drivers/Src/USARTxDriver.o ./Drivers/Src/USARTxDriver.su

.PHONY: clean-Drivers-2f-Src

