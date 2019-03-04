#ifndef __STATUS_PACKET__
#define __STATUS_PACKET__
enum GPSState{
	GPS_INIT = 0,
	GPS_READY = 1,
	GPS_FAIL = 2,
	GPS_RETRY = 3
};

enum StorageState{
	STR_INIT = 0,
	STR_READY = 1,
	STR_FAIL = 2,
	STR_RETRY = 3
};

enum SDRState{
	SDR_INIT = 0,
	SDR_READY = 1,
	SDR_FAIL = 2,
	SDR_RETRY = 3
};

enum SystemState{
	SYS_INIT = 0,
	SYS_READY = 1,
	SYS_FAIL = 2,
	SYS_RETRY = 3
};
typedef struct StatusPacket{
	StorageState storage;
	SDRState sdr;
	SystemState system;
	GPSState gps;
} StatusPacket;

#endif