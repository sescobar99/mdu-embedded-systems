################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
lab2_assignment2.cpp: ../lab2_assignment2.ino
	@echo 'Building file: "$<"'
	@echo 'Invoking: Resource Custom Build Step'
	
	@echo 'Finished building: "$<"'
	@echo ' '

%.o: ./%.cpp $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"C:/Users/josep/AppData/Local/Energia15/packages/energia/tools/arm-none-eabi-gcc/6.3.1-20170620/bin/arm-none-eabi-gcc.exe" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=120000000L -DENERGIA_EK_TM4C1294XL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"D:/ti/TivaWare_C_Series-2.2.0.295" -I"C:/Users/josep/AppData/Local/Energia15/packages/energia/hardware/tivac/1.0.3/system/driverlib" -I"C:/Users/josep/AppData/Local/Energia15/packages/energia/hardware/tivac/1.0.3/system/inc" -I"C:/Users/josep/AppData/Local/Energia15/packages/energia/hardware/tivac/1.0.3/system" -I"C:/Users/josep/AppData/Local/Energia15/packages/energia/hardware/tivac/1.0.3/cores/tivac" -I"C:/Users/josep/AppData/Local/Energia15/packages/energia/hardware/tivac/1.0.3/variants/EK-TM4C1294XL" -I"C:/Users/josep/gitrepos/mdu-embedded-systems/lab2_assignment2" -I"C:/Users/josep/AppData/Local/Energia15/packages/energia/hardware/tivac/1.0.3/libraries/SPI" -I"C:/Users/josep/AppData/Local/Energia15/packages/energia/tools/arm-none-eabi-gcc/6.3.1-20170620/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -std=gnu++11 -fno-rtti -fno-threadsafe-statics -fno-exceptions $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


