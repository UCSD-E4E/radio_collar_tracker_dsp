/*
 * HMC5983.cpp - library header
 *
 * simple library for the HMC5983 sensor from Honeywell
 *
 * (c) 2014 Korneliusz Jarzebski, www.jarzebski.pl
 * (c) 2014 David Cuartielles, Arduino LLC
 * (c) 2016 Abel Romero, www.abelromero.com
 */

#ifndef HMC5983_h
#define HMC5983_h

/*! \file */

#include <Arduino.h>

// I2C ADDRESS
#define HMC5983_ADDRESS 0x1E

// I2C COMMANDS
#define HMC5983_WRITE  0x3C
#define HMC5983_READ  0x3D

// MEMORY MAPPING
/*
Address Location  Name    Access
---------------------------------------------------
0x00  Configuration Register A  Read/Write
0x01  Configuration Register B  Read/Write
0x02  Mode Register     Read/Write
0x03  Data Output X MSB Register  Read
0x04  Data Output X LSB Register  Read
0x05  Data Output Z MSB Register  Read
0x06  Data Output Z LSB Register  Read
0x07  Data Output Y MSB Register  Read
0x08  Data Output Y LSB Register  Read
0x09  Status Register     Read
0x0A  Identification Register A Read
0x0B  Identification Register B Read
0x0C  Identification Register C Read
0x31  Temperature Output MSB Register Read
0x32  Temperature Output LSB Register Read
*/

#define HMC5983_REG_CONFIG_A (0x00)
#define HMC5983_REG_CONFIG_B (0x01)
#define HMC5983_REG_MODE     (0x02)

#define HMC5983_OUT_X_MSB (0x03)
#define HMC5983_OUT_X_LSB (0x04)
#define HMC5983_OUT_Z_MSB (0x05)
#define HMC5983_OUT_Z_LSB (0x06)
#define HMC5983_OUT_Y_MSB (0x07)
#define HMC5983_OUT_Y_LSB (0x08)

#define HMC5983_STATUS (0x09)

#define HMC5983_TEMP_OUT_MSB (0x31)
#define HMC5983_TEMP_OUT_LSB (0x32)

#define HMC5983_REG_OUT_X_M (0x03)
#define HMC5983_REG_OUT_X_L (0x04)
#define HMC5983_REG_OUT_Z_M (0x05)
#define HMC5983_REG_OUT_Z_L (0x06)
#define HMC5983_REG_OUT_Y_M (0x07)
#define HMC5983_REG_OUT_Y_L (0x08)

#define HMC5983_REG_IDENT_A (0x0A)
#define HMC5983_REG_IDENT_B (0x0B)
#define HMC5983_REG_IDENT_C (0x0C)

/**
 * HMC5983 sampling rate (DOx).  This configures the rate at which the sensor samples
 * each channel.
 */
typedef enum {
	/**
	 * 220 Hz Sampling Rate
	 */
	HMC5983_DATARATE_220HZ      = 0b111,
	/**
	 * 75 Hz Sampling Rate
	 */
	HMC5983_DATARATE_75HZ       = 0b110,
	/**
	 * 30 Hz Sampling Rate
	 */
	HMC5983_DATARATE_30HZ       = 0b101,
	/**
	 * 15 Hz Sampling Rate (Default)
	 */
	HMC5983_DATARATE_15HZ       = 0b100,
	/**
	 * 7.5 Hz Sampling Rate
	 */
	HMC5983_DATARATE_7_5HZ      = 0b011,
	/**
	 * 3 Hz Sampling Rate
	 */
	HMC5983_DATARATE_3HZ        = 0b010,
	/**
	 * 1.5 Hz Sampling Rate
	 */
	HMC5983_DATARATE_1_5HZ      = 0b001,
	/**
	 * 0.75 Hz Sampling Rate
	 */
	HMC5983_DATARATE_0_75HZ     = 0b000
} hmc5983_dataRate_t;

/**
 * HMC5893 Sample Averaging (MAx).  This configures how many measurement are averaged
 * together per output.
 */
typedef enum {
	/**
	 * Average 8 samples
	 */
	HMC5983_SAMPLEAVERAGE_8     = 0b11,
	/**
	 * Average 4 samples
	 */
	HMC5983_SAMPLEAVERAGE_4     = 0b10,
	/**
	 * Average 2 samples
	 */
	HMC5983_SAMPLEAVERAGE_2     = 0b01,
	/**
	 * Average 1 sample (Default)
	 */
	HMC5983_SAMPLEAVERAGE_1     = 0b00
} hmc5983_sampleAverages_t;

/**
 * HMC5983 Dynamic Range (GNx).  These are all reported as signed 12 bit values.
 */
typedef enum {
	/**
	 * +/- 8.1 Gauss
	 */
	HMC5983_RANGE_8_1GA     = 0b111,
	/**
	 * +/- 5.6 Gauss
	 */
	HMC5983_RANGE_5_6GA     = 0b110,
	/**
	 * +/- 4.7 Gauss
	 */
	HMC5983_RANGE_4_7GA     = 0b101,
	/**
	 * +/- 4.0 Gauss
	 */
	HMC5983_RANGE_4GA       = 0b100,
	/**
	 * +/- 2.5 Gauss
	 */
	HMC5983_RANGE_2_5GA     = 0b011,
	/**
	 * +/- 1.9 Gauss
	 */
	HMC5983_RANGE_1_9GA     = 0b010,
	/**
	 * +/- 1.3 Gauss (Default)
	 */
	HMC5983_RANGE_1_3GA     = 0b001,
	/**
	 * +/- 0.88 Gauss
	 */
	HMC5983_RANGE_0_88GA    = 0b000
} hmc5983_range_t;

/**
 * HMC5983 Measurement Mode (MDx).  Configures whether or not the device is in
 * idle, single measurement, or continuous measurement mode.
 */
typedef enum {
	/**
	 * Idle mode.  Device is placed in idle mode.
	 */
	HMC5983_IDLE1         = 0b11,
	/**
	 * Idle mode.  Device is placed in idle mode.
	 */
	HMC5983_IDLE2         = 0b10,
	/**
	 * Single-Measurement Mode (Default). When single-measurement mode is 
	 * selected, device performs a single measurement, sets RDY high and 
	 * returned to idle mode. Mode register returns to idle mode bit values. The
	 * measurement remains in the data output register and RDY remains high 
	 * until the data output register is read or another measurement is 
	 * performed. 
	 */
	HMC5983_SINGLE        = 0b01,
	/**
	 * Continuous-Measurement Mode. In continuous-measurement mode, the device 
	 * continuously performs measurements and places the result in the data 
	 * register. RDY goes high when new data is placed in all three registers. 
	 * After a power-on or a write to the mode or configuration register, the 
	 * first measurement set is available from all three data output registers 
	 * after a period of 2/fDO and subsequent measurements are available at a 
	 * frequency of fDO, where fDO is the frequency of data output.
	 */
	HMC5983_CONTINOUS     = 0b00
} hmc5983_mode_t;

/**
 * Class to interface with HMC5983 magnetometer
 */
class HMC5983 {
	public:
		/**
		 * Initializes the HMC5983 interface.
		 * @param  ISR_callback Function pointer to the data ready interrupt
		 *                      function
		 * @param  D    		Debug flag
		 * @return      		True if HMC5983 identified, false otherwise
		 */
		bool begin(void (*ISR_callback)() = NULL, int D = false);
		
		/**
		 * Sets the dynamic range of the HMC5983.  
		 * @param  range The hmc5983_range_t corresponding to the desired range.
		 */
		void  setRange(hmc5983_range_t range);
		/**
		 * Gets the current dynamic range of the HMC5983.
		 * @return  The hmc5983_range_t corresponding to the currently set range.
		 */
		hmc5983_range_t getRange(void);
		/**
		 * Configures the current measurement mode of the HMC5983.
		 * @param  mode The hmc5983_mode_t corresponding to the desired mode.
		 */
		void  setMeasurementMode(hmc5983_mode_t mode);
		/**
		 * Gets the current measurement mode of the HMC5983.
		 * @return  The hmc5983_mode_t corresponding to the currently set mode.
		 */
		hmc5983_mode_t getMeasurementMode(void);
		/**
		 * Configures the current sampling rate of the HMC5983.
		 * @param  dataRate The hmc5983_dataRate_t corresponding to the desired
		 *                  rate
		 */
		void  setDataRate(hmc5983_dataRate_t dataRate);
		/**
		 * Gets the currently set sampling rate of the HMC5983.
		 * @return  The hmc5983_dataRate_t corresponding to the currently set
		 *          sampling rate.
		 */
		hmc5983_dataRate_t getDataRate(void);
		/**
		 * Configures the current averaging behavior of the HMC5983.
		 * @param  sampleAverages The hmc5983_sampleAverages_t corresponding to
		 *                        the desired behavior.
		 */
		void  setSampleAverages(hmc5983_sampleAverages_t sampleAverages);
		/**
		 * Gets the currently configured sample average behavior of the HMC5983.
		 * @return  The hmc5983_sampleAverages_t corresponding to the configured
		 *          behavior.
		 */
		hmc5983_sampleAverages_t getSampleAverages(void);
		
		/**
		 * Reads and returns the current heading measurement from the HMC5983.
		 * This is the magnetic field vector projected onto the XY plane,
		 * reported in +/- decimal degrees from magnetic North.
		 * @return Magnetic heading in decimal degrees. Range +/- 180.
		 */
		double read();
		
	private:
		void writeRegister8(uint8_t reg, uint8_t value);
		uint8_t readRegister8(uint8_t reg);
		uint8_t fastRegister8(uint8_t reg);
		int16_t readRegister16(uint8_t reg);
		int DEBUG;
};

#endif
