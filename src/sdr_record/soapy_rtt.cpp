#include "soapy_rtt.hpp"
namespace RTT{
	//radio::radio(double gain, double sampling_rate, double frequency, std::queue<std::complex<float>*>* q){
	radio::radio(double gain, double sampling_rate, double frequency){
		try{
		error.open("log.txt", std::ios::app);
		r_gain = gain;
		r_sampling_rate = sampling_rate;
		r_frequency = frequency;
		//output_queue = q;
		double fullscale;
		device = SoapySDR::Device::make();
		device->setSampleRate(SOAPY_SDR_RX, 0, r_sampling_rate);
		device->setFrequency(SOAPY_SDR_RX, 0, r_frequency);
		device->setGain(SOAPY_SDR_RX, 0, r_gain);
	
	
		error << "****************************** Device Suppport Information ******************************" << std::endl;

		if (device->hasDCOffsetMode(SOAPY_SDR_RX, 0))
			error << "Device supports automatic DC offset corrections..." << std::endl;
		else
			error << "Device does not support automatic DC offset corrections..." << std::endl;

		if (device->hasDCOffset(SOAPY_SDR_RX, 0))
			error << "Device supports frontend DC offset corrections..." << std::endl;
		else
			error << "Device does not support frontend DC offset corrections..." << std::endl;

		if (device->hasIQBalance(SOAPY_SDR_RX, 0))
			error << "Device supports frontend IQ balance corrections..." << std::endl;
		else
			error << "Device does not support frontend IQ balance corrections..." << std::endl;

		if (device->hasFrequencyCorrection(SOAPY_SDR_RX, 0))
			error << "Device supports frontend frequency corrections..." << std::endl;
		else
			error << "Device does not support frontend frequency corrections..." << std::endl;

		if (device->hasGainMode(SOAPY_SDR_RX, 0))
			error << "Device supports automatic gain control..." << std::endl;
		else
			error << "Device does not support automatic gain control..." << std::endl;
	
		std::vector<std::string> string_formats = device->getStreamFormats(SOAPY_SDR_RX, 0);
		error << "Sting formats: ";
		for (int y = 0; y < string_formats.size(); y++){
			error << string_formats[y] << ", ";
		}
		error << std::endl;
	
		error << "Native stream format: " << device->getNativeStreamFormat(SOAPY_SDR_RX, 0, fullscale) << std::endl;
		error << "*****************************************************************************************" << std::endl;
	
		}
		catch(std::exception& e){
			error << "Exception thrown: " << e.what() << std::endl;
		}
		error.close();
	

	}

	void radio::stopStreaming(){

		run = false;
		stream_thread->join();
		delete stream_thread;
			
	}

	void radio::startStreaming(std::queue<std::complex<double>*>& queue, std::mutex& mutex, std::condition_variable& var){
	
		output_queue = &queue;
		output_mutex = &mutex;
		output_var = &var;
		run = true;
		stream_thread = new std::thread(&radio::streamer, this);

	}


	void radio::streamer(){	

		try{
		error.open("log.txt", std::ios::app);
		data_stream = device->setupStream(SOAPY_SDR_RX, "CF32", channels);
		int active = device->activateStream(data_stream, 0, 0, 0);
		if(active == 0){
			error << "Stream activation successful..." << std::endl;
		}
		else{
			error << "Unsuccessful stream activation...error code: "
				<< active << std::endl;
		}
		error << "Samping Rate: " << r_sampling_rate << std::endl;
		error << "Frequency: " << r_frequency << std::endl;
		error << "Gain: " << r_gain << std::endl;
		error.close();
		
		while(run){
			error.open("log.txt", std::ios::app);
			std::complex<double>* init = new std::complex<double>[rx_buffer_size];
			void *buff_ar[] = {init};		
			int ret = device->readStream(data_stream, buff_ar, rx_buffer_size, r_flags, r_timeNs, r_timeout);
			switch(ret){
				case -1:
					error << "readStream error: Timeout." << std::endl;
					break;
				case -2:
					error << "readStream error: non-specific stream error." << std::endl;
					break;
				case -3:
					error << "readStream error: Read has data corruption, i.e. the driver saw a malformed packet." 	<< std::endl;
					break;
				case -4:
					error << "readStream error: Read has an overflow condition, i.e. internal buffer is filled." << std::endl;
						break;
				case -5:
					error << "readStream error: requested operation or flag setting is not supported by the underlying implementation." << std::endl;
					break;
				case -6:
					error << "readStream error: device encountered a stream time which was expired (late) or too early "
						<< "to process." << std::endl;
					break;
				case -7:
					error << "readStream error: write caused an underflow condition, i.e. a continuous stream was "
						<< "interrupted." << std::endl;
					break;
				default:
					break;
			}
			error.close();
			sample_counter += ret;		
			std::unique_lock<std::mutex> guard(*output_mutex);
			output_queue->push(init);
			guard.unlock();
			output_var->notify_all();
			
				
		}
		}
		catch(std::exception& e){
			std::cout << "Exception thrown: " << e.what() << std::endl;
		}
		
	
	}
	radio::~radio(){
		device->deactivateStream(data_stream, 0, 0);
		device->closeStream(data_stream);
	}
	const size_t radio::getStartTime_ms() const{
		return _start_ms;
	}
	
}


