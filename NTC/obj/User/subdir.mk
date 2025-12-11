################################################################################
# MRS Version: 2.3.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/OLED.c \
../User/ch32v00X_it.c \
../User/main.c \
../User/system_ch32v00X.c 

C_DEPS += \
./User/OLED.d \
./User/ch32v00X_it.d \
./User/main.d \
./User/system_ch32v00X.d 

OBJS += \
./User/OLED.o \
./User/ch32v00X_it.o \
./User/main.o \
./User/system_ch32v00X.o 

DIR_OBJS += \
./User/*.o \

DIR_DEPS += \
./User/*.d \

DIR_EXPANDS += \
./User/*.253r.expand \


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	riscv-wch-elf-gcc -march=rv32ec_zmmul_xw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"d:/PROJECT/mounriver-studio-projects/ntc-temperature-measurement/NTC/Debug" -I"d:/PROJECT/mounriver-studio-projects/ntc-temperature-measurement/NTC/Core" -I"d:/PROJECT/mounriver-studio-projects/ntc-temperature-measurement/NTC/User" -I"d:/PROJECT/mounriver-studio-projects/ntc-temperature-measurement/NTC/Peripheral/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

