DEVICE		=	atmega32u4
PORT		=	/dev/ttyACM0
BAUD		=	115200
CLOCK		=	16000000
ARDUINO_DIR	=	/usr/share/arduino
LIBRARY_DIR	=	$(ARDUINO_DIR)/hardware/arduino/cores/arduino/
INC			=	-I$(LIBRARY_DIR) -I$(LIBRARY_DIR)/../..//variants/leonardo
MACRO_DEFS	=	-DF_CPU=$(CLOCK) -DUSB_VID=0x2341 -DUSB_PID=0x8036
CFLAGS		=	-c -g -Wall -ffunction-sections -fdata-sections -mmcu=$(DEVICE) $(INC) $(MACRO_DEFS)
CXXFLAGS	=	-c -g -Wall -ffunction-sections -fdata-sections -fno-exceptions -mmcu=$(DEVICE) $(INC) $(MACRO_DEFS)
LDFLAGS		=	-Os -Wl,--gc-sections -mmcu=$(DEVICE) -lm

CC			=	avr-gcc
CXX			=	avr-g++
LD			=	avr-g++
AR			=	avr-ar
OBJCOPY		=	avr-objcopy

AVRLIB_OBJ	=	malloc.o realloc.o
AVRC_OBJ	=	wiring.o wiring_analog.o wiring_digital.o wiring_pulse.o wiring_shift.o WInterrupts.o
AVRCPP_OBJ	=	CDC.o HardwareSerial.o HID.o IPAddress.o main.o Print.o Stream.o Tone.o USBCore.o WMath.o WString.o
AVR_AR		=	core.a
HEX			=	ui_core.hex
ELF			=	ui_core.elf
OBJ			=	ui_core.o

.PHONY: all install clean

$(HEX): $(ELF)
	${OBJCOPY} -O ihex -R .eeprom $< $@

$(ELF): ui_core.o core.a
	${LD} -o $@ $^ $(LDFLAGS)

ui_core.o: ui_core.cpp
	$(CXX) $(CXXFLAGS) ui_core.cpp -o ui_core.o

${AVRLIB_OBJ}: 
	$(CC) $(CFLAGS) $(LIBRARY_DIR)/avr-libc/$(*F).c -o $@

${AVRC_OBJ}:
	$(CC) $(CFLAGS) $(LIBRARY_DIR)/$(*F).c -o $@

${AVRCPP_OBJ}:
	$(CXX) $(CXXFLAGS) $(LIBRARY_DIR)/$(*F).cpp -o $@

core.a: ${AVRLIB_OBJ} $(AVRC_OBJ) $(AVRCPP_OBJ)
	rm -rf core.a
	$(AR) rcs core.a $^

install: $(HEX)
	./reset.py $(PORT)
	avrdude -v -c avr109 -p $(DEVICE) -P ${PORT} -U flash:w:$< -C $(LIBRARY_DIR)/../../tools/avrdude.conf -b 57600 -D -V

clean:
	rm -f $(AVRC_OBJ) $(AVRLIB_OBJ) $(AVRCPP_OBJ)
	rm -f $(AVR_AR)
	rm -f $(HEX)
	rm -f $(ELF)
	rm -f $(OBJ)