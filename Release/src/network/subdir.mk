################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/network/PcapHandler.cpp \
../src/network/PcapWrapper.cpp \
../src/network/SocketAddress.cpp \
../src/network/TlayerDispatcher.cpp \
../src/network/TlayerPacket.cpp \
../src/network/UdpHandler.cpp 

OBJS += \
./src/network/PcapHandler.o \
./src/network/PcapWrapper.o \
./src/network/SocketAddress.o \
./src/network/TlayerDispatcher.o \
./src/network/TlayerPacket.o \
./src/network/UdpHandler.o 

CPP_DEPS += \
./src/network/PcapHandler.d \
./src/network/PcapWrapper.d \
./src/network/SocketAddress.d \
./src/network/TlayerDispatcher.d \
./src/network/TlayerPacket.d \
./src/network/UdpHandler.d 


# Each subdirectory must supply rules for building sources it contributes
src/network/%.o: ../src/network/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../src -O3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


