################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/main/CallxThread.cpp \
../src/main/callx.cpp 

OBJS += \
./src/main/CallxThread.o \
./src/main/callx.o 

CPP_DEPS += \
./src/main/CallxThread.d \
./src/main/callx.d 


# Each subdirectory must supply rules for building sources it contributes
src/main/%.o: ../src/main/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../src -O3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


