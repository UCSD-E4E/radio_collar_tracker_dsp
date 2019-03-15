#ifndef __DSPV3_H__
#define __DSPV3_H__
#include "dsp.hpp"
#include <complex>
#include "fir.hpp"
#include "integrator.hpp"
#include "classifier.hpp"
#include <condition_variable>
#include <thread>

namespace RTT{
	class DSP_V3 : public DSP{

	public:
		DSP_V3(const std::size_t sampling_freq);
		~DSP_V3();
		void startProcessing(std::queue<IQdataPtr>& inputQueue,
			std::mutex& inputMutex, std::condition_variable& inputVar,
			std::queue<PingPtr>& outputQueue, std::mutex& outputMutex,
			std::condition_variable& outputVar);
		void stopProcessing();
	private:
		/**
		 * Unpacks IQdataPtr objects and pushes them into the IQ data queue as a
		 * sequence of std::complex<double>
		 * @param i_q Input Queue
		 * @param i_m Input Mutex
		 * @param i_v Input Condition Variable
		 * @param run Run flag
		 */
		void _unpack(std::queue<IQdataPtr>& i_q, std::mutex& i_m, 
			std::condition_variable& i_v);
		std::condition_variable* _in_v;
		std::thread* _thread;
		volatile bool _run = false;

		/**
		 * Time of first sample
		 */
		std::size_t time_start_ms = 0;

		/**
		 * IQ data queue from unpack to FIR
		 */
		std::queue<std::complex<double>> _iq_data_queue;
		std::mutex _iq_mux;
		std::condition_variable _iq_cv;

		FIR _fir;

		/**
		 * Magnitude Data queue from FIR to Integrator
		 */
		std::queue<double> _mag_data_queue;
		std::mutex _mag_mux;
		std::condition_variable _mag_cv;

		const std::size_t INT_FACTOR = 800;

		Integrator _int;

		/**
		 * Candidate signal data queue from Integrator to Classifier
		 */
		std::queue<double> _candidate_queue;
		std::mutex _can_mux;
		std::condition_variable _can_cv;

		Classifier _clfr;
	};
}
#endif