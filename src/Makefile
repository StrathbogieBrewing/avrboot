###############################################################################
# Makefile for the project avrboot
###############################################################################

## General Flags
PROJECT = avrboot
MCU = atmega8
TARGET = avrboot.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF $(@F).d

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -nodefaultlibs -nostartfiles  -Wl,--section-start=.text=0x1E00 -Wl,-Map=avrboot.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = avrboot.o

## Objects explicitly added by the user
LINKONLYOBJECTS =

## Build
all: $(TARGET) avrboot.hex avrboot.eep avrboot.lss size

## Compile
avrboot.o: avrboot.S
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) avrboot.elf avrboot.hex avrboot.eep avrboot.lss avrboot.map

## AVRDude Programming
program: avrboot.hex
	avrdude -C avrdude.conf -c dragon_isp -p m8 -P usb -e -U flash:w:avrboot.hex -U lfuse:w:0xa4:m -U hfuse:w:0xc4:m
	sleep 2
	avrdude -C avrdude.conf -c dragon_isp -p m8 -P usb -U hfuse:w:0x44:m
