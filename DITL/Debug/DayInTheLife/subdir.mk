################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../DayInTheLife/softwareTest.cpp 

OBJS += \
./DayInTheLife/softwareTest.o 

CPP_DEPS += \
./DayInTheLife/softwareTest.d 


# Each subdirectory must supply rules for building sources it contributes
DayInTheLife/%.o: ../DayInTheLife/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/alpa3119/eclipse-workspace/DITL/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


