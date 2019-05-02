#include "Sensor_Module.hpp"
#include "HMC5983.hpp"
#include <Wire.h>

#define RUN_SWITCH_PIN 10

const char* Sensor_Module::RUN_TRUE = "true";
const char* Sensor_Module::RUN_FALSE = "false";

Sensor_Module::Sensor_Module(GPSState* state_var) : state_var(state_var), gps(ALL){
	*state_var = GPS_INIT;
}

Sensor_Module::~Sensor_Module(){
}

int Sensor_Module::decode(const char c){
	if(gps.decode(c)){
		if(gps.gprmc_status() == 'A'){
			*state_var = GPS_READY;
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
			packet.hdg = compass.read();
			packet.run = digitalRead(RUN_SWITCH_PIN);
			previous_fix = millis();
			return 1;
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
	}else{
		*state_var = GPS_FAIL;
	}
}

int Sensor_Module::getPacket(char* buf, size_t len){
	return snprintf(buf, len, "{\"lat\":%ld,\"lon\":%ld,\"hdg\":%d,\"tme\":%s,"
		"\"run\":%s,\"fix\":%d,\"sat\":%d,\"dat\": %s}", (long)(packet.lat * 1e7), 
		(long)(packet.lon*1e7), packet.hdg, packet.time, 
		((packet.run) ? RUN_TRUE : RUN_FALSE), (int)packet.fix, packet.sat, packet.date);
}
