################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/core/AccessMode.cpp \
../src/core/Arbitrator.cpp \
../src/core/ArrayDatatype.cpp \
../src/core/BoolDatatype.cpp \
../src/core/BusPriorityMode.cpp \
../src/core/ComMode.cpp \
../src/core/CommandMessage.cpp \
../src/core/ConfigMessage.cpp \
../src/core/DataMessage.cpp \
../src/core/Datatype.cpp \
../src/core/Dispatcher.cpp \
../src/core/DoubleDatatype.cpp \
../src/core/ErrorMessage.cpp \
../src/core/ErrorMode.cpp \
../src/core/ACPPacket.cpp \
../src/core/Factory.cpp \
../src/core/FloatDatatype.cpp \
../src/core/Int32Datatype.cpp \
../src/core/Message.cpp \
../src/core/MessageHandler.cpp \
../src/core/MessageHandlerRegistry.cpp \
../src/core/ModeManager.cpp \
../src/core/MultiDataMessage.cpp \
../src/core/PacketDatatype.cpp \
../src/core/PayloadPriorityMode.cpp \
../src/core/Permission.cpp \
../src/core/ReturnMessage.cpp \
../src/core/SendMessage.cpp \
../src/core/Setting.cpp \
../src/core/SingleDataMessage.cpp \
../src/core/Singleton.cpp \
../src/core/StartupMode.cpp \
../src/core/StringDatatype.cpp \
../src/core/SystemMode.cpp \
../src/core/Uint32Datatype.cpp \
../src/core/VariableTypeData.cpp \
../src/core/WatchdogManager.cpp 

OBJS += \
./src/core/AccessMode.o \
./src/core/Arbitrator.o \
./src/core/ArrayDatatype.o \
./src/core/BoolDatatype.o \
./src/core/BusPriorityMode.o \
./src/core/ComMode.o \
./src/core/CommandMessage.o \
./src/core/ConfigMessage.o \
./src/core/DataMessage.o \
./src/core/Datatype.o \
./src/core/Dispatcher.o \
./src/core/DoubleDatatype.o \
./src/core/ErrorMessage.o \
./src/core/ErrorMode.o \
./src/core/ACPPacket.o \
./src/core/Factory.o \
./src/core/FloatDatatype.o \
./src/core/Int32Datatype.o \
./src/core/Message.o \
./src/core/MessageHandler.o \
./src/core/MessageHandlerRegistry.o \
./src/core/ModeManager.o \
./src/core/MultiDataMessage.o \
./src/core/PacketDatatype.o \
./src/core/PayloadPriorityMode.o \
./src/core/Permission.o \
./src/core/ReturnMessage.o \
./src/core/SendMessage.o \
./src/core/Setting.o \
./src/core/SingleDataMessage.o \
./src/core/Singleton.o \
./src/core/StartupMode.o \
./src/core/StringDatatype.o \
./src/core/SystemMode.o \
./src/core/Uint32Datatype.o \
./src/core/VariableTypeData.o \
./src/core/WatchdogManager.o 

CPP_DEPS += \
./src/core/AccessMode.d \
./src/core/Arbitrator.d \
./src/core/ArrayDatatype.d \
./src/core/BoolDatatype.d \
./src/core/BusPriorityMode.d \
./src/core/ComMode.d \
./src/core/CommandMessage.d \
./src/core/ConfigMessage.d \
./src/core/DataMessage.d \
./src/core/Datatype.d \
./src/core/Dispatcher.d \
./src/core/DoubleDatatype.d \
./src/core/ErrorMessage.d \
./src/core/ErrorMode.d \
./src/core/ACPPacket.d \
./src/core/Factory.d \
./src/core/FloatDatatype.d \
./src/core/Int32Datatype.d \
./src/core/Message.d \
./src/core/MessageHandler.d \
./src/core/MessageHandlerRegistry.d \
./src/core/ModeManager.d \
./src/core/MultiDataMessage.d \
./src/core/PacketDatatype.d \
./src/core/PayloadPriorityMode.d \
./src/core/Permission.d \
./src/core/ReturnMessage.d \
./src/core/SendMessage.d \
./src/core/Setting.d \
./src/core/SingleDataMessage.d \
./src/core/Singleton.d \
./src/core/StartupMode.d \
./src/core/StringDatatype.d \
./src/core/SystemMode.d \
./src/core/Uint32Datatype.d \
./src/core/VariableTypeData.d \
./src/core/WatchdogManager.d 


# Each subdirectory must supply rules for building sources it contributes
src/core/%.o: ../src/core/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -D HOST -D DEBUG -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


