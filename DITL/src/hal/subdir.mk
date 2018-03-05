################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/hal/GPIOManager.cpp \
../src/hal/I2CManager.cpp \
../src/hal/OneWireManager.cpp \
../src/hal/SPIManager.cpp \
../src/hal/UARTManager.cpp 

OBJS += \
./src/hal/GPIOManager.o \
./src/hal/I2CManager.o \
./src/hal/OneWireManager.o \
./src/hal/SPIManager.o \
./src/hal/UARTManager.o 

CPP_DEPS += \
./src/hal/GPIOManager.d \
./src/hal/I2CManager.d \
./src/hal/OneWireManager.d \
./src/hal/SPIManager.d \
./src/hal/UARTManager.d 


# Each subdirectory must supply rules for building sources it contributes
src/hal/%.o: ../src/hal/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	arm-linux-gnueabihf-g++ -DMOCK=virtual -DPRIVATE=public -DCDHBUILD -I"/home/fsw/Robert/cdh-fsw/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


