################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/util/propagator/AllStarOrbitProp.c \
../src/util/propagator/IAU2006_XYS.c \
../src/util/propagator/OrbitalMotionAllStar.c \
../src/util/propagator/gpsFrameRotation.c \
../src/util/propagator/leapSecond.c \
../src/util/propagator/vector3D.c \
../src/util/propagator/vector3DAllStar.c 

OBJS += \
./src/util/propagator/AllStarOrbitProp.o \
./src/util/propagator/IAU2006_XYS.o \
./src/util/propagator/OrbitalMotionAllStar.o \
./src/util/propagator/gpsFrameRotation.o \
./src/util/propagator/leapSecond.o \
./src/util/propagator/vector3D.o \
./src/util/propagator/vector3DAllStar.o 

C_DEPS += \
./src/util/propagator/AllStarOrbitProp.d \
./src/util/propagator/IAU2006_XYS.d \
./src/util/propagator/OrbitalMotionAllStar.d \
./src/util/propagator/gpsFrameRotation.d \
./src/util/propagator/leapSecond.d \
./src/util/propagator/vector3D.d \
./src/util/propagator/vector3DAllStar.d 


# Each subdirectory must supply rules for building sources it contributes
src/util/propagator/%.o: ../src/util/propagator/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-linux-gnueabihf-gcc -I"/home/fsw/Robert/cdh-fsw/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


