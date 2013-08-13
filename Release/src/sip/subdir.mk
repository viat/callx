################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/sip/Call.cpp \
../src/sip/SipPacket.cpp \
../src/sip/SipProcessor.cpp 

OBJS += \
./src/sip/Call.o \
./src/sip/SipPacket.o \
./src/sip/SipProcessor.o 

CPP_DEPS += \
./src/sip/Call.d \
./src/sip/SipPacket.d \
./src/sip/SipProcessor.d 


# Each subdirectory must supply rules for building sources it contributes
src/sip/%.o: ../src/sip/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../src -O3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


