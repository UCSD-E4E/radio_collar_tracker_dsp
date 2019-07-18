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
		std::uint64_t ltime = 0;

		/**
		 * Global time in ms since GPS Epoch
		 */
		std::uint64_t gtime = 0;

		/**
		 * degrees*1e7 latitude from the WGS84 datum
		 */
		std::int64_t lat = 0;

		/**
		 * degrees*1e7 longitude from the WGS84 datum
		 */
		std::int64_t lon = 0;

		/**
		 * Altitude in meters from WGS84 spheroid
		 */
		std::int32_t alt = 0;

		/**
		 * Altitude in meters from the starting altitude
		 */
		std::int32_t rel_alt = 0;

		/**
		 * Speed in the local x axis in m/s
		 */
		float vx = 0;

		/**
		 * Speed in the local y axis in m/s
		 */
		float vy = 0;

		/**
		 * Speed in the local z axis in m/s
		 */
		float vz = 0;

		/**
		 * Heading in degrees from True North in WGS84
		 */
		std::int16_t hdg = 0;
	};
}

#endif