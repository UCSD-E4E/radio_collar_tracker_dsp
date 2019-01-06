#include "utility.hpp"
#include <queue>
#include <complex>
#include <list>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#ifndef M_NLOG10
	#define M_NLOG10 0.301029995663981195
#endif

namespace RTT{
	std::complex<double>* generateSinusoid(std::int64_t frequency,
		std::size_t sampling_frequency, std::size_t length, 
		double amplitude){
		std::complex<double>* retval = new std::complex<double>[length];
		for(std::size_t j = 0; j < length; j++){
			retval[j] = amplitude * std::exp(std::complex<double>(0, 2.0 * 
				M_PI * frequency * j / sampling_frequency));
		}
		return retval;
	}

	// void load_data(std::queue<std::complex<double>>& input_queue, 
	// 	std::complex<double>* data, std::size_t num_samples){
	// 	for(std::size_t i = 0; i < num_samples; i++){
	// 		data[i] = input_queue.front();
	// 		input_queue.pop();
	// 	}
	// }

	void load_data(std::queue<std::complex<double>>& input_queue,
		std::list<std::complex<double>>& data, std::size_t num_samples,
		std::size_t target_length){
		for(std::size_t i = 0; i < num_samples; i++){
			data.push_back(input_queue.front());
			if(data.size() > target_length){
				data.pop_front();
			}
			input_queue.pop();
		}
	}

	// std::complex<double> convolve(std::complex<double>* f, 
	// 	std::complex<double>* g, std::size_t length){
	// 	std::complex<double> retval = 0;
	// 	for(std::size_t i = 0; i < length; i++){
	// 		retval += f[i] * g[length - i - 1];
	// 	}
	// 	return retval;
	// }

	// std::complex<double> convolve(std::list<double>& f, std::list<double>& g, 
	// 	std::size_t length){
	// 	std::complex<double> retval = 0;
	// 	auto f_it = f.begin();
	// 	auto g_it = g.end();
	// 	g_it--;
	// 	for(std::size_t i = 0; i < length; i++){
	// 		retval += *f_it * *g_it;
	// 		f_it++;
	// 		g_it--;
	// 	}
	// 	return retval;
	// }

	
	std::complex<double> convolve(std::list<std::complex<double>>& sig, 
		std::complex<double>* taps, std::size_t length, std::size_t t){
		std::complex<double> retval = 0;
		auto sig_it = sig.begin();
		for(std::size_t i = 0; i < t; i++){
			sig_it++;
		}
		for(std::size_t i = 0; i < length && sig_it != sig.end(); i++, sig_it++){
			retval += *sig_it * taps[length - 1 - i];
		}
		return retval;
	}

	double amplitudeToDB(double amplitude){
		// return 20 * log10(amplitude);
		return 20 * M_NLOG10 * log2(amplitude);
	}

	double powerToDB(double power){
		return 10 * log10(power);
	}
}

