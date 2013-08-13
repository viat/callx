################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/watchdog/Watchdog.cpp 

OBJS += \
./src/watchdog/Watchdog.o 

CPP_DEPS += \
./src/watchdog/Watchdog.d 


# Each subdirectory must supply rules for building sources it contributes
src/watchdog/%.o: ../src/watchdog/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DDEBUG -I../src -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


