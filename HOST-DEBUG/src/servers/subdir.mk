################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/servers/ACSHandlers.cpp \
../src/servers/ACSServer.cpp \
../src/servers/ACSStdTasks.cpp \
../src/servers/CMDServer.cpp \
../src/servers/CMDStdTasks.cpp \
../src/servers/COMHandlers.cpp \
../src/servers/COMServer.cpp \
../src/servers/COMStdTasks.cpp \
../src/servers/DispatchStdTasks.cpp \
../src/servers/EPSHandlers.cpp \
../src/servers/EPSServer.cpp \
../src/servers/EPSStdTasks.cpp \
../src/servers/ErrorHandlers.cpp \
../src/servers/ERRServer.cpp \
../src/servers/ErrorQueue.cpp \
../src/servers/ErrorStdTasks.cpp \
../src/servers/GPSHandlers.cpp \
../src/servers/GPSServer.cpp \
../src/servers/GPSStdTasks.cpp \
../src/servers/PLDHandlers.cpp \
../src/servers/PLDServer.cpp \
../src/servers/PLDStdTasks.cpp \
../src/servers/SCHEvent.cpp \
../src/servers/SCHEventPosition.cpp \
../src/servers/SCHEventTime.cpp \
../src/servers/SCHHandlers.cpp \
../src/servers/SCHMode.cpp \
../src/servers/SCHModePosition.cpp \
../src/servers/SCHModeTime.cpp \
../src/servers/SCHServer.cpp \
../src/servers/SCHStdTasks.cpp \
../src/servers/Server.cpp \
../src/servers/SubsystemServer.cpp \
../src/servers/THMHandlers.cpp \
../src/servers/THMServer.cpp \
../src/servers/THMStdTasks.cpp 

OBJS += \
./src/servers/ACSHandlers.o \
./src/servers/ACSServer.o \
./src/servers/ACSStdTasks.o \
./src/servers/CMDServer.o \
./src/servers/CMDStdTasks.o \
./src/servers/COMHandlers.o \
./src/servers/COMServer.o \
./src/servers/COMStdTasks.o \
./src/servers/DispatchStdTasks.o \
./src/servers/EPSHandlers.o \
./src/servers/EPSServer.o \
./src/servers/EPSStdTasks.o \
./src/servers/ErrorHandlers.o \
./src/servers/ERRServer.o \
./src/servers/ErrorQueue.o \
./src/servers/ErrorStdTasks.o \
./src/servers/GPSHandlers.o \
./src/servers/GPSServer.o \
./src/servers/GPSStdTasks.o \
./src/servers/PLDHandlers.o \
./src/servers/PLDServer.o \
./src/servers/PLDStdTasks.o \
./src/servers/SCHEvent.o \
./src/servers/SCHEventPosition.o \
./src/servers/SCHEventTime.o \
./src/servers/SCHHandlers.o \
./src/servers/SCHMode.o \
./src/servers/SCHModePosition.o \
./src/servers/SCHModeTime.o \
./src/servers/SCHServer.o \
./src/servers/SCHStdTasks.o \
./src/servers/Server.o \
./src/servers/SubsystemServer.o \
./src/servers/THMHandlers.o \
./src/servers/THMServer.o \
./src/servers/THMStdTasks.o 

CPP_DEPS += \
./src/servers/ACSHandlers.d \
./src/servers/ACSServer.d \
./src/servers/ACSStdTasks.d \
./src/servers/CMDServer.d \
./src/servers/CMDStdTasks.d \
./src/servers/COMHandlers.d \
./src/servers/COMServer.d \
./src/servers/COMStdTasks.d \
./src/servers/DispatchStdTasks.d \
./src/servers/EPSHandlers.d \
./src/servers/EPSServer.d \
./src/servers/EPSStdTasks.d \
./src/servers/ErrorHandlers.d \
./src/servers/ERRServer.d \
./src/servers/ErrorQueue.d \
./src/servers/ErrorStdTasks.d \
./src/servers/GPSHandlers.d \
./src/servers/GPSServer.d \
./src/servers/GPSStdTasks.d \
./src/servers/PLDHandlers.d \
./src/servers/PLDServer.d \
./src/servers/PLDStdTasks.d \
./src/servers/SCHEvent.d \
./src/servers/SCHEventPosition.d \
./src/servers/SCHEventTime.d \
./src/servers/SCHHandlers.d \
./src/servers/SCHMode.d \
./src/servers/SCHModePosition.d \
./src/servers/SCHModeTime.d \
./src/servers/SCHServer.d \
./src/servers/SCHStdTasks.d \
./src/servers/Server.d \
./src/servers/SubsystemServer.d \
./src/servers/THMHandlers.d \
./src/servers/THMServer.d \
./src/servers/THMStdTasks.d 


# Each subdirectory must supply rules for building sources it contributes
src/servers/%.o: ../src/servers/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -D HOST -D DEBUG -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


