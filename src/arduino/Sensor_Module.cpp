#include "Sensor_Module.hpp"
#include "HMC5983.hpp"
#include <Wire.h>

#define RUN_SWITCH_PIN 10

const char* Sensor_Module::RUN_TRUE = "true";
const char* Sensor_Module::RUN_FALSE = "false";

// #define DEBUG

Sensor_Module::Sensor_Module(GPSState* state_var) : state_var(state_var), gps(ALL){
	*state_var = GPS_INIT;
	compass_ready = false;
	packet.lat = 181;
	packet.lon = 181;
	packet.hdg = 361;
	for(uint8_t i = 0; i < 10; i++){
		packet.time[i] = 0;
	}
	for(uint8_t i = 0; i < 7; i++){
		packet.date[i] = 0;
	}
	packet.run = false;
	packet.fix = GPS_FIX_NONE;
	packet.sat = 0;
	packet.rail = 0;
}

Sensor_Module::~Sensor_Module(){
}

int Sensor_Module::decode(const char c){
	if(gps.decode(c)){
		#ifdef DEBUG
		Serial.println(gps.sentence());
		Serial.println(gps.term(0));
		#endif
		if(	gps.term(0)[2] == 'R' &&
			gps.term(0)[3] == 'M' &&
			gps.term(0)[4] == 'C'){
			
			// have RMC message
			if(gps.gprmc_status() == 'A'){
				packet.lat = gps.gprmc_latitude();
				packet.lon = gps.gprmc_longitude();
				for(int i = 0; i < 10; i++){
					if(gps.term(1)[i] != 0){
						packet.time[i] = gps.term(1)[i];
					}else{
						break;
					}
				}
				for(int i = 0; i < 7; i++){
					if(gps.term(9)[i] != 0){
						packet.date[i] = gps.term(9)[i];
					}else{
						break;
					}
				}
				if(compass_ready){
					packet.hdg = compass.read();
				}
				packet.run = digitalRead(RUN_SWITCH_PIN);
				previous_fix = millis();
				if(packet.fix == GPS_FIX_FIX){
					return 1;
				}
			}
			return 0;
		}
		if(	gps.term(0)[2] == 'G' && 
			gps.term(0)[3] == 'G' && 
			gps.term(0)[4] == 'A'){
			// have GGA message
			switch(gps.term(6)[0]){
				case '0':
					packet.fix = GPS_FIX_NONE;
					*state_var = GPS_INIT;
					break;
				default:
					packet.fix = GPS_FIX_FIX;
					*state_var = GPS_READY;
					break;

			}
			packet.lat = gps.term_decimal(2);
			packet.lon = gps.term_decimal(4);
			if(compass_ready){
				packet.hdg = compass.read();
			}
			packet.sat = gps.term_decimal(7);
			return 0;
		}
		if(	gps.term(0)[2] == 'Z' && 
			gps.term(0)[3] == 'D' && 
			gps.term(0)[4] == 'A'){
			// have ZDA message
			utc_offset_ms = gps.term_decimal(1) * 1e3;
			offset_timestamp_ms = millis();
			return 0;
		}
	}
	if(millis() - previous_fix > 5000){
		*state_var = GPS_INIT;
	}
	return 0;
}

void Sensor_Module::start(){
	// Check if compass device is present first
	Wire.begin();
	Wire.beginTransmission(0x1E);
	if(Wire.endTransmission() == 0){
		compass.begin(NULL);
		compass.setMeasurementMode(HMC5983_CONTINOUS);
		compass_ready = true;
	}else{
		*state_var = GPS_FAIL;
		compass_ready = false;
		packet.hdg = 361;
	}
	packet.lat = 181;
	packet.lon = 181;
	packet.hdg = 361;
	for(uint8_t i = 0; i < 10; i++){
		packet.time[i] = 0;
	}
	for(uint8_t i = 0; i < 7; i++){
		packet.date[i] = 0;
	}
	packet.run = false;
	packet.fix = GPS_FIX_NONE;
	packet.sat = 0;
	packet.rail = 0;
}

int Sensor_Module::getPacket(char* buf, size_t len){
	return snprintf(buf, len, "{\"lat\": %ld, \"lon\": %ld, \"hdg\": %d, \"tme\": \"%s\", "
		"\"run\": \"%s\", \"fix\": %d, \"sat\": %d, \"dat\": \"%s\"}", (long)(packet.lat * 1e7), 
		(long)(packet.lon*1e7), packet.hdg, packet.time, 
		((packet.run) ? RUN_TRUE : RUN_FALSE), (int)packet.fix, packet.sat, packet.date);
}
