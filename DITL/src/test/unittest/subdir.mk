################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/test/unittest/test_ACPInterface.cpp \
../src/test/unittest/test_ACS.cpp \
../src/test/unittest/test_BeaconManager.cpp \
../src/test/unittest/test_ByteStream.cpp \
../src/test/unittest/test_COM.cpp \
../src/test/unittest/test_EPS.cpp \
../src/test/unittest/test_ExternalProcess.cpp \
../src/test/unittest/test_FileManager.cpp \
../src/test/unittest/test_GPS.cpp \
../src/test/unittest/test_GroundCommunication.cpp \
../src/test/unittest/test_HotSwapInterface.cpp \
../src/test/unittest/test_Lock.cpp \
../src/test/unittest/test_Logger.cpp \
../src/test/unittest/test_MockHAL.cpp \
../src/test/unittest/test_NMEAInterface.cpp \
../src/test/unittest/test_PowerMonitorInterface.cpp \
../src/test/unittest/test_RAD.cpp \
../src/test/unittest/test_ScheduleManager.cpp \
../src/test/unittest/test_TempInterface.cpp \
../src/test/unittest/test_Thread.cpp 

OBJS += \
./src/test/unittest/test_ACPInterface.o \
./src/test/unittest/test_ACS.o \
./src/test/unittest/test_BeaconManager.o \
./src/test/unittest/test_ByteStream.o \
./src/test/unittest/test_COM.o \
./src/test/unittest/test_EPS.o \
./src/test/unittest/test_ExternalProcess.o \
./src/test/unittest/test_FileManager.o \
./src/test/unittest/test_GPS.o \
./src/test/unittest/test_GroundCommunication.o \
./src/test/unittest/test_HotSwapInterface.o \
./src/test/unittest/test_Lock.o \
./src/test/unittest/test_Logger.o \
./src/test/unittest/test_MockHAL.o \
./src/test/unittest/test_NMEAInterface.o \
./src/test/unittest/test_PowerMonitorInterface.o \
./src/test/unittest/test_RAD.o \
./src/test/unittest/test_ScheduleManager.o \
./src/test/unittest/test_TempInterface.o \
./src/test/unittest/test_Thread.o 

CPP_DEPS += \
./src/test/unittest/test_ACPInterface.d \
./src/test/unittest/test_ACS.d \
./src/test/unittest/test_BeaconManager.d \
./src/test/unittest/test_ByteStream.d \
./src/test/unittest/test_COM.d \
./src/test/unittest/test_EPS.d \
./src/test/unittest/test_ExternalProcess.d \
./src/test/unittest/test_FileManager.d \
./src/test/unittest/test_GPS.d \
./src/test/unittest/test_GroundCommunication.d \
./src/test/unittest/test_HotSwapInterface.d \
./src/test/unittest/test_Lock.d \
./src/test/unittest/test_Logger.d \
./src/test/unittest/test_MockHAL.d \
./src/test/unittest/test_NMEAInterface.d \
./src/test/unittest/test_PowerMonitorInterface.d \
./src/test/unittest/test_RAD.d \
./src/test/unittest/test_ScheduleManager.d \
./src/test/unittest/test_TempInterface.d \
./src/test/unittest/test_Thread.d 


# Each subdirectory must supply rules for building sources it contributes
src/test/unittest/%.o: ../src/test/unittest/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	arm-linux-gnueabihf-g++ -DMOCK=virtual -DPRIVATE=public -DCDHBUILD -I"/home/fsw/Robert/cdh-fsw/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


