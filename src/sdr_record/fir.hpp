#ifndef __FIR2_H__
#define __FIR2_H__

#include <queue>
#include <complex>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "tagged_signal.hpp"

namespace RTT{

	/**
	 * This module is a FIR low pass filter configured as a 5 kHz low pass
	 * filter.  The filter takes in complex data, and outputs the magnitude of
	 * the filtered signal.  This module will also decimate the signal.
	 */
	class FIR{
	public:
		/**
		 * Contructs an FIR object.
		 */
		FIR();

		/**
		 * Destructs this FIR object.
		 */
		~FIR();

		/**
		 * Starts this FIR object with the specified data streams.
		 * @param input_queue  Input queue, takes in a sequence of 
		 *                     std::complex<double>
		 * @param input_mutex  Input queue mutex
		 * @param input_cv     Input queue condition variable
		 * @param output_queue Output queue, outputs a sequence of TaggedSignal*.
		 * @param output_mutex Output queue mutex
		 * @param output_cv    Output queue condition variable
		 * @param run          Run flag
		 */
		void start(std::queue<std::complex<double>>& input_queue, 
			std::mutex& input_mutex, std::condition_variable& input_cv,
			std::queue<TaggedSignal*>& output_queue, std::mutex& output_mutex,
			std::condition_variable& output_cv);

		/**
		 * Stops the FIR object and flushes the pipeline.  This method will
		 * return when the last sample is placed in the output queue.
		 */
		void stop();
	private:
		void _process(std::queue<std::complex<double>>& input_queue,
			std::mutex& input_mutex, std::condition_variable& input_cv,
			std::queue<TaggedSignal*>& output_queue, std::mutex& output_mutex,
			std::condition_variable& output_cv);
		std::condition_variable* _input_cv;
		std::thread* _thread;
		static const std::size_t _num_taps = 1;
		// Filter taps developed from http://t-filter.engineerjs.com/
		// passband of 0 to 2.7 kHz, gain of 1, max ripple 5dB
		// stopband 8 kHz to 125 kHz, gain of 0, minimum attenuation -60 dB
		// Sampling freq at 250 kHz
		// const double _TAPS[85] = {-0.00036150764063310153, 
		// 	0.00024193687833744056, 0.00028958674221148025, 
		// 	0.0003941191602366769, 0.000547444017659356, 0.0007478680073695282, 
		// 	0.00099694321243077, 0.0012976362108565959, 0.0016536795089213871, 
		// 	0.002068901105989994, 0.0025470217894508485, 0.003091456713599459, 
		// 	0.003705056587755878, 0.0043900699082569, 0.005148006244260592, 
		// 	0.005978945405444023, 0.006883639393691239, 0.007858513076392468, 
		// 	0.008903458277707636, 0.010014247501690962, 0.011185755778741127, 
		// 	0.01241239971467253, 0.013687410811589843, 0.01500306549513536, 
		// 	0.016350410071987526, 0.017719725005767854, 0.019100463252251527, 
		// 	0.020481395717255702, 0.021850797955537515, 0.023196547915872705, 
		// 	0.02450624917291441, 0.025767779114476058, 0.026968473501434588, 
		// 	0.028096870754723814, 0.029140996046608858, 0.030090234109354838, 
		// 	0.030934828872662582, 0.03166559102876683, 0.032274771345017675, 
		// 	0.032755538024800315, 0.0331027994631492, 0.033312693793739176, 
		// 	0.03338291257558254, 0.033312693793739176, 0.0331027994631492, 
		// 	0.032755538024800315, 0.032274771345017675, 0.03166559102876683, 
		// 	0.030934828872662582, 0.030090234109354838, 0.029140996046608858, 
		// 	0.028096870754723814, 0.026968473501434588, 0.025767779114476058, 
		// 	0.02450624917291441, 0.023196547915872705, 0.021850797955537515, 
		// 	0.020481395717255702, 0.019100463252251527, 0.017719725005767854, 
		// 	0.016350410071987526, 0.01500306549513536, 0.013687410811589843, 
		// 	0.01241239971467253, 0.011185755778741127, 0.010014247501690962, 
		// 	0.008903458277707636, 0.007858513076392468, 0.006883639393691239, 
		// 	0.005978945405444023, 0.005148006244260592, 0.0043900699082569, 
		// 	0.003705056587755878, 0.003091456713599459, 0.0025470217894508485, 
		// 	0.002068901105989994, 0.0016536795089213871, 0.0012976362108565959, 
		// 	0.00099694321243077, 0.0007478680073695282, 0.000547444017659356, 
		// 	0.0003941191602366769, 0.00028958674221148025, 
		// 	0.00024193687833744056, -0.00036150764063310153};

		const double _TAPS[1] = {1};
		volatile bool _run = false;
	};
}

#endif