################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/Users/josep/AppData/Local/Energia15/packages/energia/tools/arm-none-eabi-gcc/6.3.1-20170620

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./drivers/uartstdio.o" \
"./lab2_assignment2.o" \
$(GEN_CMDS__FLAG) \
-Wl,--start-group \
-Wl,-T"C:/Users/josep/AppData/Local/Energia15/packages/energia/hardware/tivac/1.0.3/variants/EK-TM4C1294XL/lm4fcpp_snowflake.ld" \
-ltivac_EK-TM4C1294XL_SPI \
-ltivac_EK-TM4C1294XL_core \
-ldriverlib \
-lm \
-lc \
-lgcc \
-Wl,--end-group \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include drivers/subdir_vars.mk
-include subdir_rules.mk
-include drivers/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
lab2_assignment2.out \

EXE_OUTPUTS__QUOTED += \
"lab2_assignment2.out" \

BIN_OUTPUTS += \
lab2_assignment2.bin \

BIN_OUTPUTS__QUOTED += \
"lab2_assignment2.bin" \


# All Target
all: $(OBJS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "lab2_assignment2.out" secondary-outputs

dependents:
	-cd C:/Users/josep/gitrepos/mdu-embedded-systems/tivac_EK-TM4C1294XL_core/Debug && $(MAKE) all
	-cd C:/Users/josep/gitrepos/mdu-embedded-systems/tivac_EK-TM4C1294XL_SPI/Debug && $(MAKE) all

# Tool invocations
lab2_assignment2.out: $(OBJS) $(GEN_CMDS) C:/Users/josep/gitrepos/mdu-embedded-systems/tivac_EK-TM4C1294XL_core/Debug/libtivac_EK-TM4C1294XL_core.a C:/Users/josep/gitrepos/mdu-embedded-systems/tivac_EK-TM4C1294XL_SPI/Debug/libtivac_EK-TM4C1294XL_SPI.a
	@echo 'Building target: "$@"'
	@echo 'Invoking: GNU Linker'
	"C:/Users/josep/AppData/Local/Energia15/packages/energia/tools/arm-none-eabi-gcc/6.3.1-20170620/bin/arm-none-eabi-gcc.exe" -Dprintf=iprintf -DF_CPU=120000000L -DENERGIA_EK_TM4C1294XL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Wl,-Map,"lab2_assignment2.map" --entry ResetISR -nostartfiles -nostdlib -Wl,--gc-sections -L"C:/Users/josep/gitrepos/mdu-embedded-systems/tivac_EK-TM4C1294XL_SPI/Debug" -L"C:/Users/josep/gitrepos/mdu-embedded-systems/tivac_EK-TM4C1294XL_core/Debug" -L"C:/Users/josep/AppData/Local/Energia15/packages/energia/hardware/tivac/1.0.3/system/driverlib" -Os -specs=nano.specs -specs=rdimon.specs -specs=nosys.specs -Wl,--check-sections -Wl,--unresolved-symbols=report-all -Wl,--warn-common -fsingle-precision-constant -o"lab2_assignment2.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

lab2_assignment2.bin: $(EXE_OUTPUTS)
	@echo 'Building files: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo 'Invoking: GNU Objcopy Utility'
	"C:/Users/josep/AppData/Local/Energia15/packages/energia/tools/arm-none-eabi-gcc/6.3.1-20170620/bin/arm-none-eabi-objcopy.exe" -O binary $(EXE_OUTPUTS__QUOTED) "lab2_assignment2.bin"
	@echo 'Finished building: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "lab2_assignment2.o" "drivers\uartstdio.o" 
	-$(RM) "drivers\uartstdio.d" 
	-$(RM) "lab2_assignment2.d" 
	-@echo 'Finished clean'
	-@echo ' '

secondary-outputs: $(BIN_OUTPUTS)

.PHONY: all clean dependents
.SECONDARY:
C:/Users/josep/gitrepos/mdu-embedded-systems/tivac_EK-TM4C1294XL_core/Debug/libtivac_EK-TM4C1294XL_core.a:
C:/Users/josep/gitrepos/mdu-embedded-systems/tivac_EK-TM4C1294XL_SPI/Debug/libtivac_EK-TM4C1294XL_SPI.a:

-include ../makefile.targets

