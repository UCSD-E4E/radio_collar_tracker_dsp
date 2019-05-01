/*
 * @fle io_core.c
 *
 * @author Nathan Hui, nthui@eng.ucsd.edu
 *
 * @description Radio Telemetry Tracker UI Core
 *
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <Arduino.h>
#include <stdio.h>
#include "ui_core.hpp"
#include "Sensor_Module.hpp"
#include "Status_Module.hpp"
#include "LED.hpp"

#define SENSOR_PACKET_MAX_LEN 128

#define SLEEP_TIME 100

uint8_t count = 0;
uint8_t blueState = 0, 
	redState = 0, 
	orangeState = 0, 
	yellowState = 0, 
	greenState = 0;
char sensor_packet_buf[SENSOR_PACKET_MAX_LEN];
StatusPacket status;
Sensor_Module sensor(&status.gps);
Status_Module obc(&status);

LED blue, red, orange, yellow, green;

// write interrupt handler (happens everything timer interrupt happens)
// Figure out how to configure timer to 5 hz, set leds at 5hz only if handler called

inline void blink(uint8_t pin){
	digitalWrite(pin, HIGH);
	delay(SLEEP_TIME);
	digitalWrite(pin, LOW);
}

void setup() {
	Serial.begin(9600); // via USB
	Serial1.begin(9600); // GPS
	// Set up LEDs
	blue.pin = 4;
	red.pin = 12;
	orange.pin = 6;
	yellow.pin = 8;
	green.pin = 9;
	pinMode(green.pin, OUTPUT);
	pinMode(yellow.pin, OUTPUT);
	pinMode(orange.pin, OUTPUT);
	pinMode(red.pin, OUTPUT);
	pinMode(blue.pin, OUTPUT);
	
	// Set up timer
	cli();
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;
	OCR1A = 3124;
	TCCR1B |= (1 << WGM12);
	TCCR1B |= (1 << CS12) | (1 << CS10);  
	TIMSK1 |= (1 << OCIE1A);
	sei();

	sensor.start();

	blink(blue.pin);
	blink(red.pin);
	blink(orange.pin);
	blink(yellow.pin);
	blink(green.pin);
}

inline void setLED(LEDState state, uint8_t count, uint8_t& blinkState){
	switch(state){
		case OFF:
			blinkState = LOW;
			break;
		case SLOW:
			if(count == 4){
				blinkState = !blinkState;
			}
			break;
		case FAST:
			blinkState = !blinkState;
			break;
		case ON:
			blinkState = HIGH;
	}

}

ISR( TIMER1_COMPA_vect ) { //timer1 interrupt 1Hz
	count = (count + 1) % 5;
	setLED(blue.ledstate, count, blueState);
	setLED(red.ledstate, count, redState);
	setLED(orange.ledstate, count, orangeState);
	setLED(yellow.ledstate, count, yellowState);
	setLED(green.ledstate, count, greenState);
	digitalWrite(green.pin, greenState);
	digitalWrite(yellow.pin, yellowState);
	digitalWrite(orange.pin, orangeState);
	digitalWrite(red.pin, redState);
	digitalWrite(blue.pin, blueState);
}

LEDState system_map[5] {FAST, OFF, SLOW, ON, OFF};
LEDState storage_map[4] {FAST, ON, OFF, SLOW};
LEDState sdr_map[4] {FAST, ON, OFF, SLOW};
LEDState gps_map[4] {FAST, ON, OFF, SLOW};

void loop() {
	if (Serial1.available() > 0){
		char c = Serial1.read();
		if(sensor.decode(c)){
			sensor.getPacket(sensor_packet_buf, SENSOR_PACKET_MAX_LEN);
			Serial.println(sensor_packet_buf);
		}
	}

	if(Serial.available() > 0){
		char c = Serial.read();
		if(obc.decode(c)){
			
			blue.ledstate = system_map[status.system];
			red.ledstate = storage_map[status.storage];
			orange.ledstate = sdr_map[status.sdr];
			yellow.ledstate = gps_map[status.gps];
			
			if( status.system == SYS_WAIT_FOR_START
				&& status.storage == STR_READY
				&& status.sdr == SDR_READY
				&& status.gps == GPS_READY ) {
				green.ledstate = ON;
				// Serial.println( "green" );
			}
		}
	}
	yellow.ledstate = gps_map[status.gps];
}
