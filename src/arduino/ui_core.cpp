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

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600); // via USB
	Serial1.begin(9600); // GPS
}


NMEA gps(ALL);

void loop() {
	// put your main code here, to run repeatedly:
	if (Serial1.available() > 0){
		char c = Serial1.read();
		Serial.write(c);
		if(gps.decode(c)){
			if(gps.gprmc_status() == 'A'){
				Serial.print(gps.gprmc_latitude());
				Serial.println(gps.gprmc_longitude());
			}
		}
	}
	if(Serial.available() > 0){
		char c = Serial.read();
		Serial1.write(c);
	}
}
