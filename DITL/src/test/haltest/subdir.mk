################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/test/haltest/test_ACPInterface.cpp \
../src/test/haltest/test_GPIOManager.cpp \
../src/test/haltest/test_I2CManager.cpp \
../src/test/haltest/test_OneWireManager.cpp \
../src/test/haltest/test_SPIManager.cpp \
../src/test/haltest/test_UARTManager.cpp 

OBJS += \
./src/test/haltest/test_ACPInterface.o \
./src/test/haltest/test_GPIOManager.o \
./src/test/haltest/test_I2CManager.o \
./src/test/haltest/test_OneWireManager.o \
./src/test/haltest/test_SPIManager.o \
./src/test/haltest/test_UARTManager.o 

CPP_DEPS += \
./src/test/haltest/test_ACPInterface.d \
./src/test/haltest/test_GPIOManager.d \
./src/test/haltest/test_I2CManager.d \
./src/test/haltest/test_OneWireManager.d \
./src/test/haltest/test_SPIManager.d \
./src/test/haltest/test_UARTManager.d 


# Each subdirectory must supply rules for building sources it contributes
src/test/haltest/%.o: ../src/test/haltest/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	arm-linux-gnueabihf-g++ -DMOCK=virtual -DPRIVATE=public -DCDHBUILD -I"/home/fsw/Robert/cdh-fsw/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


