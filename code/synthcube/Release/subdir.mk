################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../synthcube.cpp 

CPP_DEPS += \
./synthcube.cpp.d 

LINK_OBJ += \
./synthcube.cpp.o 


# Each subdirectory must supply rules for building sources it contributes
synthcube.cpp.o: ../synthcube.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	-MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -D__IN_ECLIPSE__=1 -x c++ "$<"  "$@"
	@echo 'Finished building: $<'
	@echo ' '


