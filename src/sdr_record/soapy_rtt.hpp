#include <iostream>
#include <SoapySDR/Device.hpp>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <fstream>
#include <ctime>
#include <chrono>
#include "AbstractSDR.hpp"

namespace RTT{
	class radio final : public AbstractSDR{
	
		private:
		// constructor parameters
		double r_gain;
		double r_sampling_rate;
		double r_frequency;
	
		// acquireWriteBuffer parameters
		int r_flags;
		long long r_timeNs;
		int r_timeout = 100000;
		int r_numelems = 1024;
		
		//startStreaming parameters
		std::queue<std::complex<double>*>* output_queue;
		std::mutex* output_mutex;
		std::condition_variable* output_var;
		std::thread* stream_thread;
		
		size_t handle;	// idex value used in the release call
		std::vector<size_t> channels{0}; // only 1 channel for the NooELEC
		bool run = false;
		std::ofstream error;
		std::size_t _start_ms;
		long long int sample_counter = 0;
		
		SoapySDR::Device *device; // radio declaration
		SoapySDR::Stream *data_stream; // tranmission stream declasration
		void streamer();	
	
		public:
	
		// constructor
		radio(double gain, double sampling_rate, double frequency);
		//radio(double gain, double sampling_rate, double frequency, std::queue<std::complex<float>*>* q);
		~radio();
		// member function prototypes
			
		void stopStreaming();
		void startStreaming(std::queue<std::complex<double>*>&, std::mutex&, std::condition_variable&);
		const size_t getStartTime_ms() const;
	};
}
