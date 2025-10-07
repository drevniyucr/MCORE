################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/main.cpp \
../Core/Src/mcore_eth.cpp \
../Core/Src/mcore_eth_utils.cpp \
../Core/Src/mcore_gpio.cpp \
../Core/Src/mcore_mpu.cpp \
../Core/Src/mcore_net.cpp \
../Core/Src/mcore_rcc.cpp \
../Core/Src/mcore_system.cpp \
../Core/Src/mcore_tcp.cpp 

C_SRCS += \
../Core/Src/stm32f7xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f7xx.c 

C_DEPS += \
./Core/Src/stm32f7xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f7xx.d 

OBJS += \
./Core/Src/main.o \
./Core/Src/mcore_eth.o \
./Core/Src/mcore_eth_utils.o \
./Core/Src/mcore_gpio.o \
./Core/Src/mcore_mpu.o \
./Core/Src/mcore_net.o \
./Core/Src/mcore_rcc.o \
./Core/Src/mcore_system.o \
./Core/Src/mcore_tcp.o \
./Core/Src/stm32f7xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f7xx.o 

CPP_DEPS += \
./Core/Src/main.d \
./Core/Src/mcore_eth.d \
./Core/Src/mcore_eth_utils.d \
./Core/Src/mcore_gpio.d \
./Core/Src/mcore_mpu.d \
./Core/Src/mcore_net.d \
./Core/Src/mcore_rcc.d \
./Core/Src/mcore_system.d \
./Core/Src/mcore_tcp.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.cpp Core/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++20 -g3 -DDEBUG -DSTM32F767xx -c -I"C:/Users/doomt/Desktop/mcore/Core/Inc" -I"C:/Users/doomt/Desktop/mcore/Core/Src" -I"C:/Users/doomt/Desktop/mcore/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/doomt/Desktop/mcore/Drivers/CMSIS/Include" -Og -ffunction-sections -fdata-sections -fno-use-cxa-atexit -Wall -Wextra -Werror -pedantic -pedantic-errors -Wmissing-include-dirs -Wswitch-default -Wconversion -v -fstack-usage -fcyclomatic-complexity -fshort-enums -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu18 -g3 -DDEBUG -DSTM32F767xx -c -I"C:/Users/doomt/Desktop/mcore/Core/Inc" -I"C:/Users/doomt/Desktop/mcore/Core/Src" -I"C:/Users/doomt/Desktop/mcore/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/doomt/Desktop/mcore/Drivers/CMSIS/Include" -Og -ffunction-sections -fdata-sections -Wall -Wextra -Werror -pedantic -pedantic-errors -Wmissing-include-dirs -Wswitch-default -Wconversion -v -fstack-usage -fcyclomatic-complexity -fshort-enums -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/mcore_eth.cyclo ./Core/Src/mcore_eth.d ./Core/Src/mcore_eth.o ./Core/Src/mcore_eth.su ./Core/Src/mcore_eth_utils.cyclo ./Core/Src/mcore_eth_utils.d ./Core/Src/mcore_eth_utils.o ./Core/Src/mcore_eth_utils.su ./Core/Src/mcore_gpio.cyclo ./Core/Src/mcore_gpio.d ./Core/Src/mcore_gpio.o ./Core/Src/mcore_gpio.su ./Core/Src/mcore_mpu.cyclo ./Core/Src/mcore_mpu.d ./Core/Src/mcore_mpu.o ./Core/Src/mcore_mpu.su ./Core/Src/mcore_net.cyclo ./Core/Src/mcore_net.d ./Core/Src/mcore_net.o ./Core/Src/mcore_net.su ./Core/Src/mcore_rcc.cyclo ./Core/Src/mcore_rcc.d ./Core/Src/mcore_rcc.o ./Core/Src/mcore_rcc.su ./Core/Src/mcore_system.cyclo ./Core/Src/mcore_system.d ./Core/Src/mcore_system.o ./Core/Src/mcore_system.su ./Core/Src/mcore_tcp.cyclo ./Core/Src/mcore_tcp.d ./Core/Src/mcore_tcp.o ./Core/Src/mcore_tcp.su ./Core/Src/stm32f7xx_it.cyclo ./Core/Src/stm32f7xx_it.d ./Core/Src/stm32f7xx_it.o ./Core/Src/stm32f7xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f7xx.cyclo ./Core/Src/system_stm32f7xx.d ./Core/Src/system_stm32f7xx.o ./Core/Src/system_stm32f7xx.su

.PHONY: clean-Core-2f-Src

