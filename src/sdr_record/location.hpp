#ifndef __LOCATION_H__
#define __LOCATION_H__

#include <cstdint>

namespace RTT{

	/**
	 * Data structure representing a single spatial-temporal coordinate.
	 */
	struct Location{
		/**
		 * Local system time in ms since Unix Epoch
		 */
		std::uint64_t ltime;

		/**
		 * Global time in ms since GPS Epoch
		 */
		std::uint64_t gtime;

		/**
		 * degrees*1e7 latitude from the WGS84 datum
		 */
		std::int64_t lat;

		/**
		 * degrees*1e7 longitude from the WGS84 datum
		 */
		std::int64_t lon;

		/**
		 * Altitude in meters from WGS84 spheroid
		 */
		std::int32_t alt;

		/**
		 * Altitude in meters from the starting altitude
		 */
		std::int32_t rel_alt;

		/**
		 * Speed in the local x axis in m/s
		 */
		float vx;

		/**
		 * Speed in the local y axis in m/s
		 */
		float vy;

		/**
		 * Speed in the local z axis in m/s
		 */
		float vz;

		/**
		 * Heading in degrees from True North in WGS84
		 */
		std::int16_t hdg;
	};
}

#endif