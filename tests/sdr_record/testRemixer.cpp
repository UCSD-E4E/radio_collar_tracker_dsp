#include <functional>
#include <fstream>
#include <complex>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>

#define private public
#define protected public
#include <remixer.hpp>
#undef private
#undef protected

#include <cassert>
#include <fftw3.h>
#include <utility.hpp>
#include <syslog.h>
#include <math.h>

void testConstructor(){
	std::int64_t shift = 12000;
	std::size_t f_s = 2000000;
	std::size_t k_up = 1;
	std::size_t k_down = 10000;
	RTT::Remixer testObj{shift, f_s, k_up, k_down};
	assert(testObj._bbeat);
	assert(testObj._thread == nullptr);
	assert(testObj._input_cv == nullptr);
	assert(testObj._upsample_factor == k_up);
	assert(testObj._downsample_factor == k_down);
	std::cout << testObj._period << std::endl;
}

void testProcess(){
	const std::size_t f1		=    100000;
	const std::size_t f_s		=   2000000;
	const std::size_t N			=   4000000;
	const std::size_t k_up		=         1;
	const std::size_t k_dn		=      1000;
	const std::int64_t f_targ	=       500;

	const std::int64_t shift	= f_targ - f1;
	const std::int64_t f_s_new	= f_s / k_dn;
	const std::size_t f_t_idx	= (std::size_t)((double)f_targ / f_s_new * N / k_dn);

	// std::ofstream ostr("testRemixer_signal_in.log");

	std::complex<double>* test_signal = RTT::generateSinusoid(f1, f_s, N);

	// for(std::size_t i = 0; i < N; i++){
	// 	ostr << test_signal[i].real();
	// 	if(test_signal[i].imag() >= 0){
	// 		ostr << "+";
	// 	}
	// 	ostr << test_signal[i].imag() << "i" << std::endl;
	// }
	// ostr.close();

	RTT::Remixer testObj{shift, f_s, k_up, k_dn};
	std::queue<std::complex<double>> i_q;
	std::mutex i_mux;
	std::condition_variable i_cv;

	std::queue<std::complex<double>> o_q;
	std::mutex o_mux;
	std::condition_variable o_cv;

	volatile bool run = false;

	std::ofstream ostr2("testRemixer_signal_out.log");


	for(std::size_t i = 0; i < N; i++){
		i_q.push(test_signal[i]);
	}

	testObj._process(i_q, i_mux, i_cv, o_q, o_mux, o_cv, &run);

	assert(i_q.empty());
	assert(o_q.size() == N / k_dn);

	fftw_complex* out_signal = (fftw_complex*) fftw_malloc(
		sizeof(fftw_complex) * o_q.size());
	double* fft = new double[o_q.size()];

	for(std::size_t i = 0; !o_q.empty(); i++){
		out_signal[i][0] = o_q.front().real();
		out_signal[i][1] = o_q.front().imag();
		o_q.pop();
		
		// ostr2 << out_signal[i][0];
		// if(out_signal[i][1] >= 0){
		// 	ostr2 << "+";
		// }
		// ostr2 << out_signal[i][1] << "i" << std::endl;
	}
	ostr2.close();

	fftw_plan p;
	fftw_init_threads();
	fftw_plan_with_nthreads(8);
	p = fftw_plan_dft_1d((int)(N / k_dn), out_signal, out_signal, FFTW_FORWARD,
		FFTW_ESTIMATE);

	fftw_execute(p);

	std::ofstream ostr3("remixer.log");

	std::cout << f_t_idx << std::endl;

	double mean = 0;
	double max = -315;

	for(std::size_t i = 0; i < N / k_dn; i++){
		fft[i] = 20 * log10(abs(std::complex<double>(out_signal[i][0] / (N / k_dn), 
			out_signal[i][1] / (N / k_dn))));
		ostr3 << fft[i] << std::endl;

		mean += fft[i];
		if(fft[i] > max){
			max = fft[i];
		}
		if(i != f_t_idx){
			assert(fft[i] < -6);
		}
	}
	mean /= N / k_dn;

	assert(fft[f_t_idx] > -6);
	assert(max > -6);


	ostr3.close();
	delete[] fft;
	fftw_free(out_signal);
	fftw_destroy_plan(p);
	fftw_cleanup();
	fftw_cleanup_threads();
	delete[] test_signal;
}

int main(int argc, char const *argv[]){
	openlog("testRemixer", LOG_PID | LOG_PERROR, LOG_USER);
	setlogmask(LOG_UPTO(LOG_INFO));
	testConstructor();
	testProcess();
	return 0;
}