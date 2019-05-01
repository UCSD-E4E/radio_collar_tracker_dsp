#ifndef __SENSOR_MODULE__
#define __SENSOR_MODULE__

#include "nmea.hpp"
#include "Status_Packet.hpp"
#include "HMC5983.hpp"

class Sensor_Module{
	private:
		const static char* RUN_TRUE;
		const static char* RUN_FALSE;
		unsigned long previous_fix;
		HMC5983 compass;

	protected:
		enum GPSFix{
			GPS_FIX_NONE = 0,
			GPS_FIX_FIX = 1
		};
		typedef struct SensorPacket{
			float lat;
			float lon;
			uint16_t hdg;
			char time[10];
			char date[7];
			bool run;
			GPSFix fix;
			int sat;
		} SensorPacket;
		GPSState* state_var;
		NMEA gps{1};
		SensorPacket packet;
		

	public:
		Sensor_Module(GPSState* state_var);
		~Sensor_Module();

		void start();

		int decode(char c);
		int getPacket(char* buf, size_t len);
};

#endif
