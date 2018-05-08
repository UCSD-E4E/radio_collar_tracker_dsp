/*
 * HMC5983.cpp - library class
 *
 * simple library for the HMC5983 sensor from Honeywell
 *
 * (c) 2014 Korneliusz Jarzebski, www.jarzebski.pl
 * (c) 2014 David Cuartielles, Arduino LLC
 * (c) 2016 Abel Romero, www.abelromero.com
 */

// MISSING: EARTH DECLINATION ANGLE
// In other words, we are not making any compensation for the earth's north pole location vs the magnetic measurement

#include "HMC5983.hpp"
#include <pins_arduino.h>
#include <Wire.h>

bool HMC5983::begin(void (*ISR_callback)(), int D){

	DEBUG = D;

	Wire.begin();

	if ((fastRegister8(HMC5983_REG_IDENT_A) != 0x48)
		|| (fastRegister8(HMC5983_REG_IDENT_B) != 0x34)
		|| (fastRegister8(HMC5983_REG_IDENT_C) != 0x33)) {
		return false;
	}

	// Set Gain Range
	setRange(HMC5983_RANGE_8_1GA);
	// Set DataRate 220Hz ~4.5ms
	setDataRate(HMC5983_DATARATE_220HZ);
	// Set number of samples to average
	setSampleAverages(HMC5983_SAMPLEAVERAGE_2);
	// Set Mode
	setMeasurementMode(HMC5983_CONTINOUS);

	// Setup DRDY int
	if (ISR_callback != NULL) {
		pinMode(3, INPUT_PULLUP);
		attachInterrupt(digitalPinToInterrupt(3), ISR_callback, FALLING);
	}

	return true;
}

/*
From datasheet for the HMC5983
Below is an example of a (power-on) initialization process for “continuous-measurement mode” via I2C interface:
1. Write CRA (00) – send 0x3C 0x00 0x70 (8-average, 15 Hz default or any other rate, normal measurement)
2. Write CRB (01) – send 0x3C 0x01 0xA0 (Gain=5, or any other desired gain)
3. For each measurement query:
	Write Mode (02) – send 0x3C 0x02 0x01 (Single-measurement mode)
	Wait 6 ms or monitor status register or DRDY hardware interrupt pin
	Send 0x3D 0x06 (Read all 6 bytes. If gain is changed then this data set is using previous gain)
	Convert three 16-bit 2’s compliment hex values to decimal values and assign to X, Z, Y, respectively.
(Self addition:)
4. Convert the magnetic information into a compass value

REGARDING THE CALCULATION OF THE ACTUAL HEADING VALUE

From AN-203 http://www51.honeywell.com/aero/common/documents/myaerospacecatalog-documents/Defense_Brochures-documents/Magnetic__Literature_Application_notes-documents/AN203_Compass_Heading_Using_Magnetometers.pdf
The magnetic compass heading can be determined (in degrees) from the magnetometer's x and y readings by using the
following set of equations:
	Direction (y>0) = 90 - [arcTAN(x/y)]*180/PI
	Direction (y<0) = 270 - [arcTAN(x/y)]*180/PI
	Direction (y=0, x<0) = 180.0
	Direction (y=0, x>0) = 0.0
*/

void HMC5983::setRange(hmc5983_range_t range) {

		writeRegister8(HMC5983_REG_CONFIG_B, range << 5);
}

hmc5983_range_t HMC5983::getRange(void)
{
		return (hmc5983_range_t)((readRegister8(HMC5983_REG_CONFIG_B) >> 5));
}

void HMC5983::setMeasurementMode(hmc5983_mode_t mode) {
		uint8_t value;

		value = readRegister8(HMC5983_REG_MODE);
		value &= 0b11111100;
		value |= mode;

		writeRegister8(HMC5983_REG_MODE, value);
}

hmc5983_mode_t HMC5983::getMeasurementMode(void) {
		uint8_t value;

		value = readRegister8(HMC5983_REG_MODE);
		value &= 0b00000011;

		return (hmc5983_mode_t)value;
}

void HMC5983::setDataRate(hmc5983_dataRate_t dataRate) {
		uint8_t value;

		value = readRegister8(HMC5983_REG_CONFIG_A);
		value &= 0b11100011;
		value |= (dataRate << 2);

		writeRegister8(HMC5983_REG_CONFIG_A, value);
}

hmc5983_dataRate_t HMC5983::getDataRate(void) {
		uint8_t value;

		value = readRegister8(HMC5983_REG_CONFIG_A);
		value &= 0b00011100;
		value >>= 2;

		return (hmc5983_dataRate_t)value;
}

void HMC5983::setSampleAverages(hmc5983_sampleAverages_t sampleAverages) {
		uint8_t value;

		value = readRegister8(HMC5983_REG_CONFIG_A);
		value &= 0b10011111;
		value |= (sampleAverages << 5);

		writeRegister8(HMC5983_REG_CONFIG_A, value);
}

hmc5983_sampleAverages_t HMC5983::getSampleAverages(void) {
		uint8_t value;

		value = readRegister8(HMC5983_REG_CONFIG_A);
		value &= 0b01100000;
		value >>= 5;

		return (hmc5983_sampleAverages_t)value;
}

// Write byte to register
void HMC5983::writeRegister8(uint8_t reg, uint8_t value) {
	Wire.beginTransmission(HMC5983_ADDRESS);
			Wire.write(reg);
			Wire.write(value);
	Wire.endTransmission();
}

// Read byte to register
uint8_t HMC5983::fastRegister8(uint8_t reg) {
	uint8_t value;
	Wire.beginTransmission(HMC5983_ADDRESS);
			Wire.write(reg);
	Wire.endTransmission();

	Wire.requestFrom(HMC5983_ADDRESS, 1);
			value = Wire.read();

	return value;
}

// Read byte from register
uint8_t HMC5983::readRegister8(uint8_t reg) {
	uint8_t value;
	Wire.beginTransmission(HMC5983_ADDRESS);
			Wire.write(reg);
	Wire.endTransmission();

	Wire.beginTransmission(HMC5983_ADDRESS);
	Wire.requestFrom(HMC5983_ADDRESS, 1);
	while(!Wire.available()) {};
			value = Wire.read();

	return value;
}

// Read word from register
int16_t HMC5983::readRegister16(uint8_t reg) {
	int16_t value;
	Wire.beginTransmission(HMC5983_ADDRESS);
			Wire.write(reg);
	Wire.endTransmission();

	Wire.beginTransmission(HMC5983_ADDRESS);
	Wire.requestFrom(HMC5983_ADDRESS, 2);
	while(!Wire.available()) {};
			uint8_t vha = Wire.read();
			uint8_t vla = Wire.read();

	value = vha << 8 | vla;

	return value;
}

double HMC5983::read() {
	// the values for X, Y & Z must be read in X, Z & Y order.
	writeRegister8(HMC5983_OUT_X_MSB, 0); // Select MSB X register
	Wire.requestFrom(HMC5983_ADDRESS, 6);
	byte X_MSB = Wire.read();
	byte X_LSB = Wire.read();
	byte Z_MSB = Wire.read();
	byte Z_LSB = Wire.read();
	byte Y_MSB = Wire.read();
	byte Y_LSB = Wire.read();

	// compose byte for X, Y, Z's LSB & MSB 8bit registers
	double HX = (X_MSB << 8) + X_LSB;
	double HZ = (Z_MSB << 8) + Z_LSB;
	double HY = (Y_MSB << 8) + Y_LSB;

	// convert the numbers to fit the
	if (HX > 0x07FF) HX = 0xFFFF - HX;
	if (HZ > 0x07FF) HZ = 0xFFFF - HZ;
	if (HY > 0x07FF) HY = 0xFFFF - HY;

	// declare the heading variable we'll be returning
	double H = 0;

	// this is the correct way, fixed from original David's work.
	// corrected following datasheet and his own comments...xD
	// even corrected from datasheet, the 90-270 angle is a bit confusing, but the 360º are captured.
	if (HY > 0) H = 90.0 - atan(HX / HY) * 180.0 / M_PI;
	if (HY < 0) H = 270.0 - atan(HX / HY) * 180.0 / M_PI;
	if (HY == 0 && HX < 0) H = 180;
	if (HY == 0 && HX > 0) H = 0;

	// point to first data register (from datasheet). Only for continuous-measurement mode.
	Wire.requestFrom(HMC5983_ADDRESS, 0x03);

	return H;
}
