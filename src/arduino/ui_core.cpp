/*
 * @file io_core.c
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

#define SENSOR_PACKET_MAX_LEN 128

HMC5983 compass;

int ref = 1;
int count = 0;
int state = 0;
char sensor_packet_buf[SENSOR_PACKET_MAX_LEN];
StatusPacket status;
Sensor_Module sensor(&status.gps);
Status_Module obc(&status);


void setup() {
	Serial.begin(9600); // via USB
	Serial1.begin(9600); // GPS
	pinMode(9, OUTPUT);
	// Broken!!!
	// compass.begin(NULL);
	// status.gps = GPS_INIT;
	// obc = new Status_Module(&status);
	// sensor = new Sensor_Module(&status.gps);
}

void loop() {
	// Serial.println("Hello");
	if (Serial1.available() > 0){
		char c = Serial1.read();
		Serial.print(c);
		if(sensor.decode(c)){
			sensor.getPacket(sensor_packet_buf, SENSOR_PACKET_MAX_LEN);
			Serial.println(sensor_packet_buf);
		}
	}

	if(Serial.available() > 0){
		char c = Serial.read();
		if(obc.decode(c)){

		}
	}

	if(count++ == ref){
		count = 0;
		digitalWrite(9, state);
		state = !state;
	}
	delay(50);
	if(count++ == ref){
		count = 0;
		digitalWrite(9, state);
		state = !state;
	}
}