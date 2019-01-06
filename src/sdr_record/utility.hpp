#ifndef __UTILITY_H__
#define __UTILITY_H__
// #include <complex.h>
#include <queue>
#include <complex>
#include <list>

namespace RTT{
	/**
	 * Generates a discrete complex sinusoid at the specified sampling frequency
	 * at the given frequency with the given amplitude with the given length.
	 * @param frequency				Frequency of complex sinusoid to generate
	 * @param sampling_frequency	Sampling frequency of output signal
	 * @param length				Length of signal in samples
	 * @param amplitude				Amplitude of signal
	 * @return						Complex double array containing sinusoid.
	 */
	std::complex<double>* generateSinusoid(std::int64_t frequency,
		std::size_t sampling_frequency, std::size_t length, 
		double amplitude = 1.0);

	// void load_data(std::queue<std::complex<double>>& input_queue, 
	// 	std::complex<double>* data, std::size_t num_samples);

	void load_data(std::queue<std::complex<double>>& input_queue,
		std::list<std::complex<double>>& data, std::size_t num_samples,
		std::size_t target_length);

	std::complex<double> convolve(std::complex<double>* f, 
		std::complex<double>* g, std::size_t length);

	std::complex<double> convolve(std::list<double>& f, std::list<double>& g, 
		std::size_t length);

	/**
	 * Calculates the convolution of sig and taps, evaluated at time t.
	 * @param sig		Input signal
	 * @param taps		Filter Taps (n to 1)
	 * @param length	Length of filter taps
	 * @param t			time to evaluate convolution at
	 * @return			Convolution evaluated at t
	 */
	std::complex<double> convolve(std::list<std::complex<double>>& sig, 
		std::complex<double>* taps, std::size_t length, std::size_t t = 0);

	/**
	 * Converts the amplitude to dB scale, absolute reference
	 * @param  amplitude Amplitude to scale to dB
	 * @return           dB
	 */
	double amplitudeToDB(double amplitude);

	/**
	 * Converts the power to dB scale, absolute referece
	 * @param  power Power to scale to dB
	 * @return       dB
	 */
	double powerToDB(double power);
}

#endif
