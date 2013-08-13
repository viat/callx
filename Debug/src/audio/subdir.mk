################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/audio/AudioHandler.cpp \
../src/audio/G711Decoder.cpp \
../src/audio/GsmDecoder.cpp \
../src/audio/PcmAudio.cpp \
../src/audio/RtpSink.cpp 

OBJS += \
./src/audio/AudioHandler.o \
./src/audio/G711Decoder.o \
./src/audio/GsmDecoder.o \
./src/audio/PcmAudio.o \
./src/audio/RtpSink.o 

CPP_DEPS += \
./src/audio/AudioHandler.d \
./src/audio/G711Decoder.d \
./src/audio/GsmDecoder.d \
./src/audio/PcmAudio.d \
./src/audio/RtpSink.d 


# Each subdirectory must supply rules for building sources it contributes
src/audio/%.o: ../src/audio/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DDEBUG -I../src -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


