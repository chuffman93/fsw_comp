################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/core/Architecture.cpp \
../src/core/FileManager.cpp \
../src/core/GroundCommunication.cpp \
../src/core/Lock.cpp \
../src/core/ScheduleManager.cpp \
../src/core/Watchdog.cpp 

OBJS += \
./src/core/Architecture.o \
./src/core/FileManager.o \
./src/core/GroundCommunication.o \
./src/core/Lock.o \
./src/core/ScheduleManager.o \
./src/core/Watchdog.o 

CPP_DEPS += \
./src/core/Architecture.d \
./src/core/FileManager.d \
./src/core/GroundCommunication.d \
./src/core/Lock.d \
./src/core/ScheduleManager.d \
./src/core/Watchdog.d 


# Each subdirectory must supply rules for building sources it contributes
src/core/%.o: ../src/core/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	arm-linux-gnueabihf-g++ -DMOCK=virtual -DPRIVATE=public -DCDHBUILD -I"/home/fsw/Robert/cdh-fsw/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


