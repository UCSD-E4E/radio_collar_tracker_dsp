DEVICE		=	atmega32u4
PORT		=	/dev/ttyACM0
BAUD		=	115200
CLOCK		=	16000000
ARDUINO_DIR	=	/usr/share/arduino
LIBRARY_DIR	=	$(ARDUINO_DIR)/hardware/arduino/cores/arduino/
INC			=	-I$(LIBRARY_DIR) -I$(LIBRARY_DIR)/../../variants/leonardo -I$(LIBRARY_DIR)/../../../../libraries/Wire -I/usr/share/arduino/libraries/Wire/utility
MACRO_DEFS	=	-DF_CPU=$(CLOCK) -DUSB_VID=0x2341 -DUSB_PID=0x8036 -DARDUINO=105 -D__PROG_TYPES_COMPAT__
CFLAGS		=	-c -g -Wall -ffunction-sections -fdata-sections -mmcu=$(DEVICE) $(INC) $(MACRO_DEFS)
CXXFLAGS	=	-c -g -Wall -ffunction-sections -fdata-sections -fno-exceptions -mmcu=$(DEVICE) $(INC) $(MACRO_DEFS)
LDFLAGS		=	-Os -Wl,--gc-sections -mmcu=$(DEVICE) -lm

CC			=	avr-gcc
CXX			=	avr-g++
LD			=	avr-gcc
AR			=	avr-ar
OBJCOPY		=	avr-objcopy

AVRLIB_OBJ	=	malloc.o realloc.o
AVRC_OBJ	=	wiring.o wiring_analog.o wiring_digital.o wiring_pulse.o wiring_shift.o WInterrupts.o
WIRE_C_OBJ	=	twi.o
WIRE_CXX_OBJ=	Wire.o
AVRCXX_OBJ	=	CDC.o HardwareSerial.o HID.o IPAddress.o main.o Print.o Stream.o Tone.o USBCore.o WMath.o WString.o new.o
AVR_AR		=	core.a
HEX			=	ui_core.hex
ELF			=	ui_core.elf
OBJ			=	ui_core.o nmea.o HMC5983.o Status_Module.o Sensor_Module.o
CJSON_OBJ	=	cJSON.o
BIT_RATE	=	4
# OBC_HOST	=	e4e-upcore-1.dynamic.ucsd.edu
OBC_HOST	=	100.80.229.30

.PHONY: all install clean

$(HEX): $(ELF)
	${OBJCOPY} -j .text -j .data -O ihex $< $@
	# ${OBJCOPY} -j .text -j .data -O ihex $< $@

$(ELF): $(OBJ) ${CJSON_OBJ} core.a
	${LD} -o $@ $^ $(LDFLAGS)

ui_core.o: ui_core.cpp ui_core.hpp nmea.hpp HMC5983.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

nmea.o: nmea.cpp nmea.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

HMC5983.o: HMC5983.cpp HMC5983.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

Sensor_Module.o: Sensor_Module.cpp Sensor_Module.hpp Status_Packet.hpp
	$(CXX) $(CXXFLAGS) $< -o $@	

Status_Module.o: Status_Module.cpp Status_Module.hpp Status_Packet.hpp
	$(CXX) $(CXXFLAGS) $< -o $@	

${CJSON_OBJ}:
	make -C cJSON cJSON.o CC=${CC} R_CFLAGS="${CFLAGS}"
	mv cJSON/cJSON.o .

${AVRLIB_OBJ}: 
	$(CC) $(CFLAGS) $(LIBRARY_DIR)/avr-libc/$(*F).c -o $@

${AVRC_OBJ}:
	$(CC) $(CFLAGS) $(LIBRARY_DIR)/$(*F).c -o $@

${AVRCXX_OBJ}:
	$(CXX) $(CXXFLAGS) $(LIBRARY_DIR)/$(*F).cpp -o $@

${WIRE_C_OBJ}:
	$(CC) $(CFLAGS) $(LIBRARY_DIR)/../../../../libraries/Wire/utility/$(*F).c -o $@	

${WIRE_CXX_OBJ}:
	$(CXX) $(CXXFLAGS) $(LIBRARY_DIR)/../../../../libraries/Wire/$(*F).cpp -o $@	


core.a: ${AVRLIB_OBJ} $(AVRC_OBJ) $(AVRCXX_OBJ) ${WIRE_C_OBJ} ${WIRE_CXX_OBJ}
	rm -rf core.a
	$(AR) rcs core.a $^

install: $(HEX)
	# ./reset.py $(PORT)
	avrdude -v -c avr109 -p $(DEVICE) -P ${PORT} -U flash:w:$< -C $(LIBRARY_DIR)/../../../tools/avrdude.conf -b 57600 -D -V

install-dragon: $(HEX)
	avrdude -p m32u4 -c dragon_isp -B ${BIT_RATE} -P usb -U flash:w:$<

install-upcore: $(HEX)
	scp $< e4e@${OBC_HOST}:$<
	ssh -t e4e@${OBC_HOST} 'sudo avrdude -p m32u4 -c dragon_isp -B ${BIT_RATE} -P usb -U flash:w:$<'

clean:
	rm -f $(AVRC_OBJ) $(AVRLIB_OBJ) $(AVRCXX_OBJ)
	rm -f $(AVR_AR)
	rm -f $(HEX)
	rm -f $(ELF)
	rm -f $(OBJ)
	rm -f core.a
	-rm test_status_module

test_status_module: Status_Module.cpp Status_Module.hpp gps_status.hpp test_status_module.cpp
	g++ -c -g -Wall -ffunction-sections -fdata-sections -fno-exceptions Status_Module.cpp
	g++ -c -g -Wall -ffunction-sections -fdata-sections -fno-exceptions test_status_module.cpp
	g++ -o test_status_module Status_Module.o test_status_module.o -Os -Wl,--gc-sections -lm