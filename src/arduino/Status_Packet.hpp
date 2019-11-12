#ifndef __STATUS_PACKET__
#define __STATUS_PACKET__
/*! \file */
/**
 * GPS Subsystem state.
 */
typedef enum{
	GPS_INIT = 0,
	GPS_READY = 1,
	GPS_FAIL = 2,
	GPS_RETRY = 3
} GPSState;

/**
 * Storage Subsystem state.
 */
enum StorageState{
	STR_INIT = 0,
	STR_READY = 1,
	STR_FAIL = 2,
	STR_RETRY = 3
};

/**
 * SDR Subsystem state.
 */
enum SDRState{
	SDR_INIT = 0,
	SDR_READY = 1,
	SDR_FAIL = 2,
	SDR_RETRY = 3
};

/**
 * Control subystem state.
 */
enum SystemState{
	SYS_INIT = 0,
	SYS_WAIT_FOR_START = 1,
	SYS_WAIT_FOR_END = 2,
	SYS_FINISH = 3,
	SYS_FAIL = 4,
};

/**
 * System Status struct.  This structure contains the last known states of each
 * subsystem.
 */
typedef struct StatusPacket{
	StorageState storage;
	SDRState sdr;
	SystemState system;
	GPSState gps;
} StatusPacket;

#endif
