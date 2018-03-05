################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/interfaces/ACPInterface.cpp \
../src/interfaces/ExternalProcess.cpp \
../src/interfaces/HotSwapInterface.cpp \
../src/interfaces/NMEAInterface.cpp \
../src/interfaces/PowerMonitorInterface.cpp \
../src/interfaces/SubPowerInterface.cpp \
../src/interfaces/TempInterface.cpp 

OBJS += \
./src/interfaces/ACPInterface.o \
./src/interfaces/ExternalProcess.o \
./src/interfaces/HotSwapInterface.o \
./src/interfaces/NMEAInterface.o \
./src/interfaces/PowerMonitorInterface.o \
./src/interfaces/SubPowerInterface.o \
./src/interfaces/TempInterface.o 

CPP_DEPS += \
./src/interfaces/ACPInterface.d \
./src/interfaces/ExternalProcess.d \
./src/interfaces/HotSwapInterface.d \
./src/interfaces/NMEAInterface.d \
./src/interfaces/PowerMonitorInterface.d \
./src/interfaces/SubPowerInterface.d \
./src/interfaces/TempInterface.d 


# Each subdirectory must supply rules for building sources it contributes
src/interfaces/%.o: ../src/interfaces/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	arm-linux-gnueabihf-g++ -DMOCK=virtual -DPRIVATE=public -DCDHBUILD -I"/home/fsw/Robert/cdh-fsw/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


