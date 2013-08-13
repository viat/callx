################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/config/CallxConfig.cpp \
../src/config/Config.cpp 

OBJS += \
./src/config/CallxConfig.o \
./src/config/Config.o 

CPP_DEPS += \
./src/config/CallxConfig.d \
./src/config/Config.d 


# Each subdirectory must supply rules for building sources it contributes
src/config/%.o: ../src/config/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DDEBUG -I../src -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


