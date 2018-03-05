################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/test/swintegration/MockACP.cpp \
../src/test/swintegration/MockNMEA.cpp \
../src/test/swintegration/MockSubPower.cpp 

OBJS += \
./src/test/swintegration/MockACP.o \
./src/test/swintegration/MockNMEA.o \
./src/test/swintegration/MockSubPower.o 

CPP_DEPS += \
./src/test/swintegration/MockACP.d \
./src/test/swintegration/MockNMEA.d \
./src/test/swintegration/MockSubPower.d 


# Each subdirectory must supply rules for building sources it contributes
src/test/swintegration/%.o: ../src/test/swintegration/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	arm-linux-gnueabihf-g++ -DMOCK=virtual -DPRIVATE=public -DCDHBUILD -I"/home/fsw/Robert/cdh-fsw/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


