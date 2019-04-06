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
#include "nmea.hpp"
#include "HMC5983.hpp"
#include <stdio.h>
#include "ui_core.hpp"
#include "Sensor_Module.hpp"
#include "Status_Module.hpp"
#include "LED.hpp"

#define SENSOR_PACKET_MAX_LEN 128

HMC5983 compass;

int serialToggle = 0;

int ref = 1;
int count = 0;
//int state = 0;
int blueState = 0, 
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

void setup() {
        Serial.begin(9600); // via USB
        Serial1.begin(9600); // GPS
	// Set up LEDs
        pinMode(9, OUTPUT);
	pinMode( 8, OUTPUT );
	pinMode( 6, OUTPUT );
	pinMode( 12, OUTPUT );
	pinMode( 4, OUTPUT );
	blue.pin = 9;
        red.pin = 8;
        orange.pin = 6;
        yellow.pin = 12;
        green.pin = 4;
	blue.ledstate = OFF;
	red.ledstate = OFF;
	orange.ledstate = OFF;
	yellow.ledstate = OFF;
	green.ledstate = OFF;

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

        // Broken!!!
        // compass.begin(NULL);
        // status.gps = GPS_INIT;
        // obc = new Status_Module(&status);
        // sensor = new Sensor_Module(&status.gps);
}

inline void configureLED(){
}

ISR( TIMER1_COMPA_vect ) { //timer1 interrupt 1Hz
	count++;
	configureLED();
	if( 1) {
		switch( blue.ledstate ) {
		case OFF:
			digitalWrite( blue.pin, LOW );
			break;
		case SLOW:
			if( count == 5 ) {
				blueState = !blueState;
				digitalWrite( blue.pin, blueState );
			}
			break;
		case FAST:
			blueState = !blueState;
			digitalWrite( blue.pin, blueState );
			break;
		case ON:
			digitalWrite( blue.pin, HIGH );
			break;
		}
		switch( red.ledstate ) {
		case OFF:
			digitalWrite( red.pin, LOW );
			break;
		case SLOW:
			if( count == 5 ) {
				redState = !redState;
				digitalWrite( red.pin, redState );
			}
			break;
		case FAST:
			redState = !redState;
			digitalWrite( red.pin, redState );
			break;
		case ON:
			digitalWrite( red.pin, HIGH );
			break;
		}
		switch( orange.ledstate ) {
		case OFF:
			digitalWrite( orange.pin, LOW );
			break;
		case SLOW:
			if( count == 5 ) {
				orangeState = !orangeState;
				digitalWrite( orange.pin, orangeState );
			}
			break;
		case FAST:
			orangeState = !orangeState;
			digitalWrite( orange.pin, orangeState );
			break;
		case ON:
			digitalWrite( orange.pin, HIGH );
			break;
		}
		switch( yellow.ledstate ) {
		case OFF:
			digitalWrite( yellow.pin, LOW );
			break;
		case SLOW:
			if( count == 5 ) {
				yellowState = !yellowState;
				digitalWrite( yellow.pin, yellowState );
			}
			break;
		case FAST:
			yellowState = !yellowState;
			digitalWrite( yellow.pin, yellowState );
			break;
		case ON:
			digitalWrite( yellow.pin, HIGH );
			break;
		}
		switch( green.ledstate ) {
		case OFF:
			digitalWrite( green.pin, LOW );
			break;
		case SLOW:
			if( count == 5 ) {
				greenState = !greenState;
				digitalWrite( green.pin, greenState );
			}
			break;
		case FAST:
			greenState = !greenState;
			digitalWrite( green.pin, greenState );
			break;
		case ON:
			digitalWrite( green.pin, HIGH );
			break;
		}
	}
	if( count == 5 ) {
		count = 0;
	}
}

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
                        switch( status.system ) {
                                case SYS_INIT:
                                        blue.ledstate = FAST;
                                        Serial.println( "blue SYS_INIT");
					break;
                                case SYS_WAIT_FOR_START:
                                        blue.ledstate = OFF;
                                        Serial.println( "blue. SYS_WAIT_FOR_START");
					break;
                                case SYS_WAIT_FOR_END:
                                        blue.ledstate = SLOW;
                                        Serial.println( "blue SYS_WAIT_FOR_END");
					break;
                                case SYS_FINISH:
                                        blue.ledstate = ON;
                                        Serial.println( "blue SYS_FINISH");
					break;
                                case SYS_FAIL:
                                        blue.ledstate = OFF;
                                        Serial.println( "blue SYS_FAIL");
					break;
                        }
                        switch( status.storage ) {
                                case STR_INIT:
                                        red.ledstate = FAST;
                                        Serial.println( "red STR_INIT" );
					break;
                                case STR_READY:
                                        red.ledstate = ON;
                                        Serial.println( "red STR_READY" );
					break;
                                case STR_FAIL:
                                        red.ledstate = OFF;
                                        Serial.println( "red STR_FAIL" );
					break;
                                case STR_RETRY:
                                        red.ledstate = SLOW;
                                        Serial.println( "red STR_RETRY" );
					break;
                        }
                        switch( status.sdr ) {
                                case SDR_INIT:
                                        orange.ledstate = FAST;
                                        Serial.println( "orange SDR_INIT" );
					break;
                                case SDR_READY:
                                        orange.ledstate = ON;
                                        Serial.println( "orange SDR_READY" );
					break;
                                case SDR_FAIL:
                                        orange.ledstate = OFF;
                                        Serial.println( "orange SDR_FAIL" );
					break;
                                case SDR_RETRY:
                                        orange.ledstate = SLOW;
                                        Serial.println( "orange SDR_RETRY" );
					break;
                        }
                     switch( status.gps ) {
                                case GPS_INIT:
                                        yellow.ledstate = FAST;
                                        Serial.println( "yellow GPS_INIT" );
					break;
                                case GPS_READY:
                                        yellow.ledstate = ON;
                                        Serial.println( "yellow GPS_READY" );
					break;
                                case GPS_FAIL:
                                        yellow.ledstate = OFF;
                                        Serial.println( "yellow GPS_FAIL" );
					break;
                                case GPS_RETRY:
                                        yellow.ledstate = SLOW;
                                        Serial.println( "yellow GPS_RETRY" );
					break;
                        }
                        if( status.system == SYS_WAIT_FOR_START
                                && status.storage == STR_READY
                                && status.sdr == SDR_READY
                                && status.gps == GPS_READY ) {
                                green.ledstate = ON;
                                Serial.println( "green" );
                        }
                }
        }
}
