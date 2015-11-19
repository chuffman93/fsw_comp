################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/HAL/SAMA5D3/Interrupt.cpp \
../src/HAL/SAMA5D3/RTC.cpp \
../src/HAL/SAMA5D3/Signal.cpp 

OBJS += \
./src/HAL/SAMA5D3/Interrupt.o \
./src/HAL/SAMA5D3/RTC.o \
./src/HAL/SAMA5D3/Signal.o 

CPP_DEPS += \
./src/HAL/SAMA5D3/Interrupt.d \
./src/HAL/SAMA5D3/RTC.d \
./src/HAL/SAMA5D3/Signal.d 


# Each subdirectory must supply rules for building sources it contributes
src/HAL/SAMA5D3/%.o: ../src/HAL/SAMA5D3/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -D HOST -D DEBUG -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


