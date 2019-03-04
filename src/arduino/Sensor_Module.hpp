#ifndef __SENSOR_MODULE__
#define __SENSOR_MODULE__

#include "nmea.hpp"
#include "Status_Packet.hpp"

class Sensor_Module{
	private:
		const static char* RUN_TRUE;
		const static char* RUN_FALSE;

	protected:
		enum GPSFix{
			GPS_FIX_NONE = 0,
			GPS_FIX_FIX = 1
		};
		typedef struct SensorPacket{
			float lat;
			float lon;
			uint16_t hdg;
			uint32_t time;
			bool run;
			GPSFix fix;
			int sat;
		} SensorPacket;
		GPSState* state_var;
		NMEA gps;
		SensorPacket packet;
		

	public:
		Sensor_Module(GPSState* state_var);
		~Sensor_Module();

		int decode(char c);
		int getPacket(char* buf, size_t len);
};

#endif