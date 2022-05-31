################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/mainServoV1.c \
../Src/syscalls.c \
../Src/sysmem.c 

OBJS += \
./Src/mainServoV1.o \
./Src/syscalls.o \
./Src/sysmem.o 

C_DEPS += \
./Src/mainServoV1.d \
./Src/syscalls.d \
./Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F411RE -DSTM32 -DSTM32F4 -DSTM32F411RETx -DSTM32F411xE -c -I../Inc -I"/home/jutoroa/workspace/STMDirectory/DriversSTM32/Inc" -I/home/jutoroa/workspace/STMDirectory/mcu_headers/CMSIS/Device/ST/STM32F4xx/Include -I/home/jutoroa/workspace/STMDirectory/mcu_headers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/mainServoV1.d ./Src/mainServoV1.o ./Src/mainServoV1.su ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su

.PHONY: clean-Src

