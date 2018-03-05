################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/util/ACPPacket.cpp \
../src/util/BeaconManager.cpp \
../src/util/InitializationTools.cpp \
../src/util/Logger.cpp \
../src/util/Thread.cpp \
../src/util/TimeKeeper.cpp \
../src/util/crc.cpp 

OBJS += \
./src/util/ACPPacket.o \
./src/util/BeaconManager.o \
./src/util/InitializationTools.o \
./src/util/Logger.o \
./src/util/Thread.o \
./src/util/TimeKeeper.o \
./src/util/crc.o 

CPP_DEPS += \
./src/util/ACPPacket.d \
./src/util/BeaconManager.d \
./src/util/InitializationTools.d \
./src/util/Logger.d \
./src/util/Thread.d \
./src/util/TimeKeeper.d \
./src/util/crc.d 


# Each subdirectory must supply rules for building sources it contributes
src/util/%.o: ../src/util/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	arm-linux-gnueabihf-g++ -DMOCK=virtual -DPRIVATE=public -DCDHBUILD -I"/home/fsw/Robert/cdh-fsw/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


