/**
 * This program records broadband signal data from an RTL2832 based SDR to
 * assist detection of radio collars.
 * Copyright (C) 2015 by Nathan Hui <nthui@eng.ucsd.edu>
 *
 */
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <string>
extern "C"{
	#include <rtl-sdr.h>
}
#include <signal.h>
#include <mutex>
#include <queue>
#include <thread>
#include <time.h>
#include <fstream>
#include <cstring>

using namespace std;

// Global constants
#define FILE_CAPTURE_DAEMON_SLEEP_PERIOD_MS	500

// Global variables
static rtlsdr_dev_t *dev = NULL;
static bool run = true;
mutex queue_mutex;
queue<char*> data_queue;

// Function Prototypes
void sighandler(int signal);
int main(int argc, char** argv);
void proc_queue(int run_num, int frame_len);
extern "C" {
   static void rtlsdr_callback(unsigned char* buf, uint32_t len, void *ctx);
}

/**
 * Main function of this program.
 */
int main(int argc, char** argv){
	// Set up variables
	// Options variable
	int opt;
	// SDR Gain
	int gain = 0;
	// Sampling Frequency for SDR in samples per second
	int samp_freq = 2048000;
	// Center Frequency for SDR in Hz
	int center_freq = 172464000;
	// Nominal pulse length in ms
	int pulse_length = 10;
	// Run number
	int run_num = -1;
	// Block size
	int block_size = 0;
	// Start Time
	struct timespec start_time;
	// End Time
	struct timespec end_time;

	// Get command line options
	cout << "Getting command line options" << endl;
	while ((opt = getopt(argc, argv, "g:s:f:p:r:")) != -1){
		switch(opt){
			case 'g':
				gain = (int)(atof(optarg) * 10);
				break;
			case 's':
				samp_freq = (int)(atof(optarg));
				break;
			case 'f':
				center_freq = (int)(atof(optarg));
				break;
			case 'p':
				pulse_length = (int)(atof(optarg));
				break;
			case 'r':
				run_num = (int)(atof(optarg));
				break;
		}
	}
	if(run_num == -1){
		// TODO: Add usage notifcation here!
		cerr << "Bad RUN number!" << endl;
		exit(-1);
	}
	cout << "done" << endl;

	// Initialize environment
	cout << "Configuring environment" << endl;
	block_size = (int)(pulse_length / 1000.0 * 2 * samp_freq);
	block_size = 1 * 16384;
	cout << "Done configuring environment" << endl;

	// Open SDR
	cout << "Opening SDR" << endl;
	if(rtlsdr_open(&dev, 0)){
		cerr << "Failed to open rtlsdr device" << endl;
		exit(1);
	}
	cout << "SDR Opened" << endl;

	// Configure SDR
	cout << "Configuring SDR" << endl;
	if(gain == 0){
		if(rtlsdr_set_tuner_gain_mode(dev, 0)){
			cerr << "WARNING: Failed to set tuner gain." << endl;
			exit(1);
		}
	}else{
		if(rtlsdr_set_tuner_gain_mode(dev, 1)){
			cerr << "WARNING: Failed to enable manual gain." << endl;
			exit(1);
		}
		if(rtlsdr_set_tuner_gain(dev, gain)){
			cerr << "WARNING: Failed to set tuner gain." << endl;
			exit(1);
		}
	}
	if(rtlsdr_set_center_freq(dev, center_freq)){
		cerr << "WARNING: Failed to set center frequency." << endl;
		exit(1);
	}
	if(rtlsdr_set_sample_rate(dev, samp_freq)){
		cerr << "WARNING: Failed to set sampling frequency." << endl;
		exit(1);
	}
	cout << "SDR Configured" << endl;

	// Configure signal handlers
	cout << "Configuring signal handlers" << endl;
	signal(SIGINT, sighandler);
	signal(SIGTERM, sighandler);
	cout << "Signal handlers configured" << endl;

	// Configure worker thread
	cout << "Configured worker thread" << endl;
	thread (proc_queue, run_num, block_size).detach();
	cout << "Worker thread configured" << endl;

	// Get timestamp
	cout << "Starting record" << endl;
	clock_gettime(CLOCK_REALTIME, &start_time);
	cout << block_size << endl;
	// Begin recording
	rtlsdr_read_async(dev, rtlsdr_callback, &data_queue, 0, block_size);
	// clean up
	clock_gettime(CLOCK_REALTIME, &end_time);
	cout << "Stopping record" << endl;
	rtlsdr_close(dev);
}

void sighandler(int signal){
	cout << "Signal caught, exiting!" << endl;
	run = false;
	rtlsdr_cancel_async(dev);
}

void proc_queue(int run_num, int frame_len){
	// setup filestream
	ofstream data_stream;
	char buff[256];
	int frame_num;
	frame_num = 0;
	queue_mutex.lock();
	bool empty = data_queue.empty();
	queue_mutex.unlock();
	cout << "Running thread" << endl;
	while(run  || !empty){
		cout << "Checking queue" << endl;
		queue_mutex.lock();
		empty = data_queue.empty();
		queue_mutex.unlock();
		if(!empty){
			// Process queue
			cout << "Got frame" << endl;
			snprintf(buff, sizeof(buff), 
					"RAW_DATA_%06d_%06d", run_num, 
					frame_num % 4 + 1);
			data_stream.open(buff, ios::out | ios::binary | ios::app);
			queue_mutex.lock();
			char* data_ptr = data_queue.front();
			data_queue.pop();
			queue_mutex.unlock();
			cout << "Writing frame" << endl;
			data_stream.write(data_ptr, frame_len);
			data_stream.close();
			free(data_ptr);
			frame_num++;
			cout << "done" << endl;
		}else{
			// wait
			cout << "No frame" << endl;
			usleep(FILE_CAPTURE_DAEMON_SLEEP_PERIOD_MS * 1000);
		}
	}
}

static void rtlsdr_callback(unsigned char* buf, uint32_t len, void *ctx){
	cout << "Callback" << endl;
	if(!run){
		return;
	}
	char newframe[len];
	memcpy(newframe, buf, len);
	cout << "Pushing frame" << endl;
	queue_mutex.lock();
	data_queue.push(newframe);
	queue_mutex.unlock();
}
