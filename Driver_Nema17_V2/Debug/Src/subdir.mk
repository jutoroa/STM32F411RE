################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/newmainV2.c \
../Src/syscalls.c \
../Src/sysmem.c 

OBJS += \
./Src/newmainV2.o \
./Src/syscalls.o \
./Src/sysmem.o 

C_DEPS += \
./Src/newmainV2.d \
./Src/syscalls.d \
./Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F411RE -DSTM32 -DSTM32F4 -DSTM32F411RETx -DSTM32F411xE -c -I../Inc -I"G:/Mi unidad/Unalmed/Taller_5/STM32DIRECTORY/mcu_headers/CMSIS/Device/ST/STM32F4xx/Include" -I"G:/Mi unidad/Unalmed/Taller_5/STM32DIRECTORY/mcu_headers/CMSIS/Include" -I"G:/Mi unidad/Unalmed/Taller_5/STM32DIRECTORY/Driver_Nema17_V2/Drivers/Inc" -I"G:/Mi unidad/Unalmed/Taller_5/STM32DIRECTORY/Driver_Nema17_V2/Drivers/Src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/newmainV2.d ./Src/newmainV2.o ./Src/newmainV2.su ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su

.PHONY: clean-Src

