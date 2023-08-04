################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./src/main.c \
./src/diskio.c \
./src/ff.c \
./src/ffsystem.c \
./src/ffunicode.c \
./src/spi.c \
./src/spi_sd.c

C_DEPS += \
./src/main.d \
./src/diskio.d \
./src/ff.d \
./src/ffsystem.d \
./src/ffunicode.d \
./src/spi.d \
./src/spi_sd.d


OBJS += \
./src/main.o \
./src/diskio.o \
./src/ff.o \
./src/ffsystem.o \
./src/ffunicode.o \
./src/spi.o \
./src/spi_sd.o


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ./src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

#src/stm32f4xx_hal_msp.o: ./src/stm32f4xx_hal_msp.c src/subdir.mk
#	@echo 'Building file: $<'
#	@echo 'Invoking: GNU Arm Cross C Compiler'
#	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -std=gnu11 -Wno-padded -Wno-missing-prototypes -Wno-missing-declarations -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
#	@echo 'Finished building: $<'
#	@echo ' '


