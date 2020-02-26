################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include usbdrv/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
Eskonzo.lss \

FLASH_IMAGE += \
Eskonzo.hex \

EEPROM_IMAGE += \
Eskonzo.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: Eskonzo.elf secondary-outputs

# Tool invocations
Eskonzo.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,Eskonzo.map -mmcu=atmega8 -o "Eskonzo.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

Eskonzo.lss: Eskonzo.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S Eskonzo.elf  >"Eskonzo.lss"
	@echo 'Finished building: $@'
	@echo ' '

Eskonzo.hex: Eskonzo.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Eskonzo.elf  "Eskonzo.hex"
	@echo 'Finished building: $@'
	@echo ' '

Eskonzo.eep: Eskonzo.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex Eskonzo.elf  "Eskonzo.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: Eskonzo.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega8 Eskonzo.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(EEPROM_IMAGE)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) Eskonzo.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
