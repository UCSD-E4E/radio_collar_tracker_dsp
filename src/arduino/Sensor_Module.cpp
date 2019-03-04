#include "Sensor_Module.hpp"

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
			packet.time = gps.gprmc_utc();
			packet.hdg = 0;
			packet.run = digitalRead(10);
		}
	}
	return 0;
}

int Sensor_Module::getPacket(char* buf, size_t len){
	return snprintf(buf, len, "{\"lat\":%ld,\"lon\":%ld,\"hdg\":%d,\"tme\":%lu,"
		"\"run\":%s,\"fix\",%d,\"sat\",%d}", (long)(packet.lat * 1e7), 
		(long)(packet.lon*1e7), packet.hdg, packet.time, 
		((packet.run) ? RUN_TRUE : RUN_FALSE), (int)packet.fix, packet.sat);
}