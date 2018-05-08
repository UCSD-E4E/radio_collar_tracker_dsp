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
#include <pins_arduino.h>
#include "nmea.hpp"
#include "HMC5983.hpp"

NMEA gps(ALL);
HMC5983 compass;

float lat;
float lon;
float time;
double hdg;

void setup() {
	Serial.begin(9600); // via USB
	Serial1.begin(9600); // GPS
	compass.begin(NULL);
}


void loop() {
	if (Serial1.available() > 0){
		char c = Serial1.read();
		if(gps.decode(c)){
			if(gps.gprmc_status() == 'V'){
				lat = gps.gprmc_latitude();
				lon = gps.gprmc_longitude();
				time = gps.gprmc_utc();
				hdg = compass.read();
				Serial.print(lat);
				Serial.print("\t");
				Serial.print(lon);
				Serial.print("\t");
				Serial.print(time);
				Serial.print("\t");
				Serial.println(hdg);
			}
		}
	}
}
