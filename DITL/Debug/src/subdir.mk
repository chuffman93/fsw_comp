################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ExternalProcess.cpp \
../src/FileManager.cpp \
../src/Lock.cpp \
../src/Logger.cpp \
../src/TimeKeeper.cpp 

OBJS += \
./src/ExternalProcess.o \
./src/FileManager.o \
./src/Lock.o \
./src/Logger.o \
./src/TimeKeeper.o 

CPP_DEPS += \
./src/ExternalProcess.d \
./src/FileManager.d \
./src/Lock.d \
./src/Logger.d \
./src/TimeKeeper.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/alpa3119/eclipse-workspace/DITL/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


