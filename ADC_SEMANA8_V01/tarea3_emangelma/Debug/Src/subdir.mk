################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/newmain.c \
../Src/syscalls.c \
../Src/sysmem.c 

OBJS += \
./Src/newmain.o \
./Src/syscalls.o \
./Src/sysmem.o 

C_DEPS += \
./Src/newmain.d \
./Src/syscalls.d \
./Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F411RE -DSTM32 -DSTM32F4 -DSTM32F411RETx -DSTM32F411xE -c -I../Inc -I"D:/TallerV_Workspace/mcu_headers/CMSIS/Device/ST/STM32F4xx/Include" -I"D:/TallerV_Workspace/mcu_headers/CMSIS/Include" -I"D:/TallerV_Workspace/tarea3_emangelma/Drivers/Inc" -I"D:/TallerV_Workspace/tarea3_emangelma/Drivers/Src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

