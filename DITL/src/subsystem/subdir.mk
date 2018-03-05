################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/subsystem/ACS.cpp \
../src/subsystem/CDH.cpp \
../src/subsystem/COM.cpp \
../src/subsystem/EPS.cpp \
../src/subsystem/GPS.cpp \
../src/subsystem/RAD.cpp \
../src/subsystem/SubsystemBase.cpp 

OBJS += \
./src/subsystem/ACS.o \
./src/subsystem/CDH.o \
./src/subsystem/COM.o \
./src/subsystem/EPS.o \
./src/subsystem/GPS.o \
./src/subsystem/RAD.o \
./src/subsystem/SubsystemBase.o 

CPP_DEPS += \
./src/subsystem/ACS.d \
./src/subsystem/CDH.d \
./src/subsystem/COM.d \
./src/subsystem/EPS.d \
./src/subsystem/GPS.d \
./src/subsystem/RAD.d \
./src/subsystem/SubsystemBase.d 


# Each subdirectory must supply rules for building sources it contributes
src/subsystem/%.o: ../src/subsystem/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	arm-linux-gnueabihf-g++ -DMOCK=virtual -DPRIVATE=public -DCDHBUILD -I"/home/fsw/Robert/cdh-fsw/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


