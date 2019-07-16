#ifndef __DSPV1_H__
#define __DSPV1_H__

#include "dsp.hpp"
#include <thread>
#include "iq_data.hpp"
#include <string>
#include <fstream>
#include <condition_variable>

namespace RTT{
	class DSP_V1 : public DSP{
		std::queue<IQdataPtr>* input_queue;
		std::mutex* input_mutex;
		std::condition_variable* input_var;
		std::thread* stream_thread;
		void process(const volatile bool*);
		std::string output_folder;
		const size_t QUEUE_WARNING_LENGTH = 15;
		const size_t QUEUE_CRITICAL_LENGTH = 63;
		size_t run_num;
		std::string generatePrefix();

	public:
		DSP_V1(std::string, size_t run_num);
		DSP_V1(const char*, size_t run_num);
		void startProcessing(
			std::queue<IQdataPtr>& inputQueue, 
			std::mutex& inputMutex, std::condition_variable& inputVar,
			std::queue<PingPtr>& outputQueue, std::mutex& outputMutex, 
			std::condition_variable& outputVar, const volatile bool* ndie);
		void stopProcessing();
		void setStartTime(std::size_t);
	};

	class StorageEngine{
		std::string data_dir;
		size_t max_size;
		std::string prefix;
		std::string suffix;
		size_t file_num = 1;
		std::ofstream* output_stream;
		std::string generateFileName();
		size_t byte_counter = 0;
		const std::ios_base::openmode stream_flags = std::ios_base::out | std::ios_base::binary;
	public:
		enum FileSize{
			FS_16k = 16384,
			FS_32k = 32768,
			FS_64k = 65536,
			FS_128k = 131072,
			FS_256k = 262144,
			FS_512k = 524288,
			FS_1M = 1048576,
			FS_2M = 2097152,
			FS_4M = 4194304,
			FS_8M = 8388608,
			FS_16M = 16777216,
			FS_32M = 33554432,
			FS_64M = 67108864,
			FS_128M = 134217728,
			FS_256M = 268435456,
			FS_512M = 536870912,
			FS_1G = 1073741824
		};

		/**
		 * @param	datadir	output data directory
		 * @param	maxSize	maximum size per file
		 * @param	prefix	file prefix
		 * @param	suffix	file suffix
		 */
		StorageEngine(std::string dataDir, FileSize maxSize, std::string prefix, std::string suffix);

		/**
		 * @param	buffer	byte array of data
		 * @param	len		length of byte array
		 * @return	number of bytes written
		 */
		size_t write(void* buffer, size_t len);

		~StorageEngine();
	};
}

#endif