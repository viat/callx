################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/output/filesystem/RawFileWriter.cpp \
../src/output/filesystem/WaveFileWriter.cpp 

OBJS += \
./src/output/filesystem/RawFileWriter.o \
./src/output/filesystem/WaveFileWriter.o 

CPP_DEPS += \
./src/output/filesystem/RawFileWriter.d \
./src/output/filesystem/WaveFileWriter.d 


# Each subdirectory must supply rules for building sources it contributes
src/output/filesystem/%.o: ../src/output/filesystem/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DDEBUG -I../src -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


