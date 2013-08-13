################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/console/CommandServer.cpp \
../src/console/Console.cpp \
../src/console/Session.cpp \
../src/console/TcpServer.cpp 

OBJS += \
./src/console/CommandServer.o \
./src/console/Console.o \
./src/console/Session.o \
./src/console/TcpServer.o 

CPP_DEPS += \
./src/console/CommandServer.d \
./src/console/Console.d \
./src/console/Session.d \
./src/console/TcpServer.d 


# Each subdirectory must supply rules for building sources it contributes
src/console/%.o: ../src/console/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DDEBUG -I../src -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


