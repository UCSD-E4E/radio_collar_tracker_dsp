#ifndef __DSPV3_H__
#define __DSPV3_H__
#include "dsp.hpp"
#include <complex>
#include "fir.hpp"
#include "integrator.hpp"
#include "classifier.hpp"
#include <condition_variable>
#include <thread>
#include "tagged_signal.hpp"
#include <fftw3.h>
#include <memory>
#include <set>
#include "AbstractSDR.hpp"

namespace RTT{
	class DSP_V3 : public DSP{

	public:
		DSP_V3(const std::size_t sampling_freq, const std::size_t center_freq,
			const std::vector<std::size_t>& target_freqs,
			const std::size_t width_ms,
			const double snr,
			const double max_len_threshold,
			const double min_len_threshold);
		~DSP_V3();
		void startProcessing(std::queue<std::complex<double>*>& inputQueue,
			std::mutex& inputMutex, std::condition_variable& inputVar,
			std::queue<PingPtr>& outputQueue, std::mutex& outputMutex,
			std::condition_variable& outputVar);
		void stopProcessing();
		void setStartTime(const std::size_t time_start_ms);
		void setOutputDir(const std::string& dir, const std::string& fmt);

		void setPingWidth(const std::size_t width_ms);
		void setMinSNR(const double snr);
		void setHighThreshold(const double threshold);
		void setLowThreshold(const double threshold);
	private:
		/**
		 * Unpacks std::complex<double> objects and pushes them into the IQ data queue as a
		 * sequence of std::complex<double>
		 * @param i_q Input Queue
		 * @param i_m Input Mutex
		 * @param i_v Input Condition Variable
		 * @param run Run flag
		 */
		void _unpack(std::queue<std::complex<double>*>& i_q, std::mutex& i_m, 
			std::condition_variable& i_v);


		void classify(std::queue<std::shared_ptr<std::vector<double>>>&, std::mutex&,
			std::condition_variable&, std::queue<PingPtr>&, std::mutex&,
			std::condition_variable&);
		std::condition_variable* _in_v;
		std::thread* _thread;
		volatile bool _run = false;

		/**
		 * Time of first sample
		 */
		std::size_t time_start_ms = 0;


		std::queue<std::shared_ptr<std::vector<double>>> _c_q;
		std::mutex _c_m;
		std::condition_variable _c_v;
		std::thread* _c_thread;

		const float int_time_s = 6e-3;
		std::size_t int_factor;

		std::string _output_dir;
		char* _output_fmt;

		std::size_t maximizer_len;
		std::size_t median_len;
		std::size_t data_len;
		
		// 2017 data
		// const static std::size_t ping_width_ms = 15;
		// const double MIN_SNR = 0.07;
		// const double HIGH_THRESHOLD = 2;
		// const double LOW_THRESHOLD = 0.75;

		// 2019 test data
		std::size_t ping_width_ms = 36;
		double MIN_SNR = 4;
		double HIGH_THRESHOLD = 1.5;
		double LOW_THRESHOLD = 0.75;

		std::size_t ping_width_samp;
		const std::size_t FFT_LEN = 2048;
		double clfr_input_freq;


		const std::size_t SAMPLES_PER_FILE = 64*1024*1024/sizeof(int16_t) / 2;
		const std::size_t FRAMES_PER_FILE = SAMPLES_PER_FILE / AbstractSDR::rx_buffer_size;

		std::shared_ptr<std::vector<double>> max(boost::circular_buffer<std::shared_ptr<std::vector<double>>>& sig);

		std::vector<double>* sig_median(boost::circular_buffer<std::shared_ptr<std::vector<double>>>& sig);

		fftw_complex* _unpack_fft_in;
		fftw_complex* _unpack_fft_out;
		fftw_plan _unpack_fft_plan;

		std::shared_ptr<std::vector<bool>> compare(const std::vector<double>& data, const std::vector<double>& threshold, double min_snr) const;
		std::shared_ptr<std::set<std::size_t>> has_falling_edge(boost::circular_buffer<std::shared_ptr<std::vector<bool>>>::iterator it);

		const std::size_t get_pulse_width(
		boost::circular_buffer<std::shared_ptr<std::vector<bool>>>::iterator begin,
		boost::circular_buffer<std::shared_ptr<std::vector<bool>>>::iterator end,
		std::size_t i);
		double _ms_per_sample;

		const std::vector<std::size_t> target_freqs;
		const std::size_t nFreqs;
		std::vector<std::size_t> target_bins;
		const double pow(const fftw_complex& sample) const;
		const double get_pulse_magnitude(boost::circular_buffer<std::shared_ptr<std::vector<double>>>::iterator start,
			boost::circular_buffer<std::shared_ptr<std::vector<double>>>::iterator end, std::size_t idx) const;
		const std::size_t idxToFreq(const std::size_t idx) const;
		const std::size_t s_freq;
		const std::size_t c_freq;
	};
}
#endif