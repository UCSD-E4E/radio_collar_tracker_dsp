#include "dspv1.hpp"
#include <thread>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <syslog.h>
#include <functional>

namespace RTT{
	DSP_V1::DSP_V1(std::string output_dir, size_t run_num) : output_folder(output_dir), run_num(run_num){
	}

	DSP_V1::DSP_V1(const char* output_dir, size_t run_num) : output_folder(output_dir), run_num(run_num){
	}

	void DSP_V1::startProcessing(std::queue<IQdataPtr>& inputQueue, 
			std::mutex& inputMutex, std::condition_variable& inputVar,
			std::queue<PingPtr>& outputQueue, std::mutex& outputMutex, 
			std::condition_variable& outputVar, const volatile bool* ndie){
		input_queue = &inputQueue;
		input_mutex = &inputMutex;
		input_var = &inputVar;
		stream_thread = new std::thread(&DSP_V1::process, this, ndie);
	}

	void DSP_V1::stopProcessing(){
		stream_thread->join();
	}

	std::string DSP_V1::generatePrefix(){
		std::stringstream string_builder;
		string_builder << "RAW_DATA_";
		string_builder << std::setw(6);
		string_builder << std::setfill('0');
		string_builder << run_num;
		string_builder << "_";
		return string_builder.str();
	}


	void DSP_V1::process(const volatile bool* ndie){
		syslog(LOG_INFO, "wx: starting thread");
		int16_t* data_array;

		StorageEngine storage(output_folder, StorageEngine::FileSize::FS_64M, generatePrefix(), std::string());

		while(*ndie || !input_queue->empty()){
			std::unique_lock<std::mutex> in_lock(*input_mutex);
			if(input_queue->empty()){
				input_var->wait(in_lock);
			}
			syslog(LOG_DEBUG, "wx: checking for frame");
			syslog(LOG_DEBUG, "wx: queue has %zd remaining", input_queue->size());
			if(input_queue->size() > QUEUE_WARNING_LENGTH){
				syslog(LOG_NOTICE, "wx: queue has more than %zd frames!", QUEUE_WARNING_LENGTH);
			}
			if(input_queue->size() > QUEUE_CRITICAL_LENGTH){
				syslog(LOG_WARNING, "wx: queue has more than %zd frames!", QUEUE_CRITICAL_LENGTH);
			}

			if(!input_queue->empty()){
				syslog(LOG_DEBUG, "wx: data frame exists");
				IQdataPtr data_buffer = input_queue->front();
				input_queue->pop();
				in_lock.unlock();
				data_array = new int16_t[data_buffer->data->size() * 2];
				for(size_t i = 0; i < data_buffer->data->size(); i++){
					data_array[2 * i] = (*data_buffer->data)[i].real();
					data_array[2 * i + 1] = (*data_buffer->data)[i].imag();
				}

				storage.write(data_array, data_buffer->data->size() * 2 * sizeof(int16_t));

				delete(data_array);
			}
		}
	}

	std::string StorageEngine::generateFileName(){
		std::stringstream string_builder;
		string_builder << data_dir;
		string_builder << "/" << prefix;
		string_builder << std::setw(6);
		string_builder << std::setfill('0');
		string_builder << file_num;
		string_builder << std::setw(suffix.length());
		string_builder << suffix;
		syslog(LOG_DEBUG, "Using filename %s", string_builder.str().c_str());
		return string_builder.str();
	}

	StorageEngine::StorageEngine(std::string dataDir, FileSize maxSize, std::string prefix, std::string suffix) : data_dir(dataDir), max_size(maxSize), prefix(prefix), suffix(suffix){
		
		output_stream = new std::ofstream(generateFileName(), stream_flags);
	}

	StorageEngine::~StorageEngine(){
		if(output_stream){
			if(output_stream->is_open()){
				output_stream->close();
			}
		}
	}

	size_t StorageEngine::write(void* buffer, size_t len){
		// Check for file overflow
		if(len + (byte_counter % max_size) > max_size){
			// overflow!
			output_stream->close();
			file_num++;
			std::string newFilename = generateFileName();
			output_stream->open(newFilename, stream_flags);
		}
		output_stream->write(reinterpret_cast<char*>(buffer), len).flush();
		return len;
	}

	void DSP_V1::setStartTime(std::size_t t){
		
	}
}