################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/util/FileHandler.cpp \
../src/util/crc.cpp \
../src/util/itoa.cpp 

OBJS += \
./src/util/FileHandler.o \
./src/util/crc.o \
./src/util/itoa.o 

CPP_DEPS += \
./src/util/FileHandler.d \
./src/util/crc.d \
./src/util/itoa.d 


# Each subdirectory must supply rules for building sources it contributes
src/util/%.o: ../src/util/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -D HOST -D DEBUG -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


