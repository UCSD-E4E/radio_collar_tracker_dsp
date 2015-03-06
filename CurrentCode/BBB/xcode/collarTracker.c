#include <cairo.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <rtl-sdr.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include "serialGPS.h"

#define FILE_BUFFER_SIZE			256
#define AUX_STRING_SIZE				256
#define EXTRA_DATA_SIZE				13
#define RAW_DATA_FILENAME 			"/media/RAW_DATA/rct/RAW_DATA_"
#define META_FILE_PREFIX 			"/media/RAW_DATA/rct/RUN_META_"
#define FILE_COUNTER_PATH 			"/media/RAW_DATA/rct/fileCount"
#define CONFIG_FILE_PATH 			"/media/RAW_DATA/rct/cfg"
#define GPS_SERIAL_PERIOD 			250
#define PIN_PATH 					"/sys/class/gpio/gpio7/value"

// Signal Buffer Arrays
unsigned char *time_buffer 			= NULL;	// Time domain buffer
unsigned char *rawFileBuffer 		= NULL;	// Data Storage Time domain buffer

// File Handling Variables
FILE *fileStream;
fpos_t filePos; 
char fileBuffer[FILE_BUFFER_SIZE]; 
char meta_file_path[FILE_BUFFER_SIZE]; 
int currentRun						= 0;
int currentRunFile					= 1;
int trigger 						= 0;

// Config Variables 
const int valid_gain_values[] 		= { 0, 9, 14, 27, 37, 77, 87, 125, 144, 157, 166, 197, 207, 229, 254, 280, 297, 328, 338, 364, 372, 386, 402, 421, 434, 439, 445, 480, 496 };
const int max_gain_index 			= 28;
int  current_gain_index 			= 0;
unsigned int center_freq 			= 0;
unsigned int samp_rate 				= 0;
int timeout_interrupt				= 0; 
int goal_signal_amplitude 			= 0; 
int controller_coef 				= 0; 
int number_frames_per_file 			= 0; 
int gps_serial_mult 				= 0; 
int gps_serial_count 				= 0; 

unsigned int time_buffer_len 		= 0;
unsigned int raw_file_buffer_len 	= 0;
unsigned int dev_index 				= 0;

// Aux Variables
int frame_counter					= 0; 
int maxFindAuxInt					= 0; 
int intAux							= 0;
char auxString[AUX_STRING_SIZE]; 
int device_count;
int r;

// GUI variables
unsigned int tid 					= 0;
cairo_t *cr 						= NULL;
GtkWidget *window 					= NULL;
rtlsdr_dev_t *dev 					= NULL;

//Serial Variables
int fd;
int32_t alt;
int32_t lat;
int32_t lon;

void destroy(GtkWidget *widget, gpointer data);

void init_memmory();

void update_meta(); 

void clean_up_memmory();

void checkRunNumber();

void load_files();

int loadParameter(); 

int timeout_cb(gpointer darea);

void populate_gps();

void compile_data(); 

void store_data(); 

gboolean read_rtlsdr();

void adjust_gain();

void setup_rtlsdr();

void siginthandler(int sig);

int main(int argc, char *argv[]){

	signal(SIGINT, siginthandler);

	load_files(); 

    setup_rtlsdr();

    init_memmory();

    fd=init_serial();
	
	// GPS communication testing
	serial_read(fd);
	
	printf("GPS and SDR operating!\n");
	
	checkRunNumber();

    tid = g_timeout_add(GPS_SERIAL_PERIOD, timeout_cb, window);
    
	gtk_main();
    
    g_source_remove(tid);    

	clean_up_memmory();

	close_port(fd);
    
    return 0;
}

void destroy(GtkWidget *widget, gpointer data){
	cairo_destroy(cr);
	gtk_main_quit();
}

int timeout_cb(gpointer darea){

/*	fileStream = fopen(PIN_PATH,"r");
	fgets(fileBuffer, FILE_BUFFER_SIZE,fileStream);
	fclose(fileStream);
	trigger = atoi(fileBuffer);
	if(trigger)
		exit(0);
*/
	// GPS Handling
	serial_read(fd);

	gps_serial_count++;
	// Gets samples from rtlsdr
	if(gps_serial_count >= gps_serial_mult){
		gps_serial_count = 0;
		if (read_rtlsdr())
			return FALSE;  // error handling

		populate_gps();

		compile_data();

		printf("Current Frame: %03d Gain: %03d\n", frame_counter, valid_gain_values[(int)current_gain_index]);

		frame_counter++;
		if(frame_counter >= number_frames_per_file){
			printf("FILE: %06d\n", currentRunFile);
			store_data();
			frame_counter = 0; 
			update_meta(); 
			currentRunFile++; 
		}

		adjust_gain();
	}
	
	return TRUE;
}

void compile_data(){

	// SDR data
	for (int i = 0; i < time_buffer_len/2; i++){
		rawFileBuffer[frame_counter*(time_buffer_len+EXTRA_DATA_SIZE)+2*i] 		= time_buffer[2*i];
		rawFileBuffer[frame_counter*(time_buffer_len+EXTRA_DATA_SIZE)+2*i+1]	= time_buffer[2*i+1];
	}
	
	// GPS data
	for (int i = 0; i < 4; i++){
		rawFileBuffer[frame_counter*(time_buffer_len+EXTRA_DATA_SIZE)+time_buffer_len+i] 	= (lat>>8*i)&0xFF;
		rawFileBuffer[frame_counter*(time_buffer_len+EXTRA_DATA_SIZE)+time_buffer_len+4+i] 	= (lon>>8*i)&0xFF;
		rawFileBuffer[frame_counter*(time_buffer_len+EXTRA_DATA_SIZE)+time_buffer_len+8+i] 	= (alt>>8*i)&0xFF;
	}
	
	// Gain
	rawFileBuffer[frame_counter*(time_buffer_len+EXTRA_DATA_SIZE)+time_buffer_len+EXTRA_DATA_SIZE-1] = current_gain_index;
}

void store_data(){
	sprintf(auxString, "%s%06d_%06d", RAW_DATA_FILENAME, currentRun, currentRunFile);
	fileStream = fopen (auxString, "wb");
	fwrite(rawFileBuffer, sizeof(unsigned char), raw_file_buffer_len*sizeof(unsigned char), fileStream);
	fclose(fileStream);
}

void init_memmory(){
	time_buffer		= malloc(time_buffer_len * sizeof(unsigned char));
	rawFileBuffer	= malloc(raw_file_buffer_len * sizeof(unsigned char));
}

void update_meta(){

	sprintf(meta_file_path, "%s%06d", META_FILE_PREFIX, currentRun);
	fileStream = fopen (meta_file_path, "wb");
	fprintf(fileStream, "center_freq: %d \nsamp_rate: %d \ntimeout_interrupt: %d \ngoal_signal_amplitude: %d \ncontroller_coef: %d \nnumber_frames_per_file: %d \ncurrentRunFile: %d \n ",
	 center_freq, samp_rate, timeout_interrupt, goal_signal_amplitude, controller_coef, number_frames_per_file, currentRunFile);
	fclose(fileStream);

}

void clean_up_memmory(){
	rtlsdr_close(dev);
	free(time_buffer);
	free(rawFileBuffer);
}

void checkRunNumber(){ 
	fileStream	= fopen(FILE_COUNTER_PATH, "r+");
	currentRun	= loadParameter();
	currentRun++; 
	rewind(fileStream); 
	fprintf(fileStream, "currentRun: %d \n", currentRun);
 	fclose(fileStream);
}

void load_files(){ 

	fileStream 						= fopen(CONFIG_FILE_PATH, "r");
	center_freq 					= loadParameter();
	samp_rate 						= loadParameter();
	timeout_interrupt 				= loadParameter();
	goal_signal_amplitude 			= loadParameter();
	controller_coef 				= loadParameter();
	number_frames_per_file 			= loadParameter();
 	fclose(fileStream);

	time_buffer_len 				= (int)(((float)timeout_interrupt*samp_rate)/500); 
	raw_file_buffer_len 			= number_frames_per_file*(time_buffer_len+EXTRA_DATA_SIZE);
	gps_serial_mult 				= timeout_interrupt/GPS_SERIAL_PERIOD; 

}

int loadParameter(){
    if (fgets(fileBuffer, FILE_BUFFER_SIZE, fileStream) == NULL) {
        fprintf(stderr, "ERROR Reading Data Config File!\n");
		exit(1);
    }
    intAux = 0; 
    while(fileBuffer[intAux] != ':')
    	intAux++;
    if(intAux >= FILE_BUFFER_SIZE){
        fprintf(stderr, "ERROR Reading Data Config File!\n");
		exit(1);
    }
    return atoi(fileBuffer+intAux+1); 
}

gboolean read_rtlsdr(){

    int n_read;
	
	for (int i = 0; i < time_buffer_len; i++)
		*(time_buffer+i) = 0; 

    if (rtlsdr_read_sync(dev, time_buffer, time_buffer_len, &n_read) < 0) {
        fprintf(stderr, "WARNING: sync read failed. time_buffer_len: %d.\n", time_buffer_len);
    	return TRUE;
    }

    if ((unsigned int)n_read < time_buffer_len) {
        fprintf(stderr, "Short read (%d / %d), samples lost, exiting!\n", n_read, time_buffer_len);
    	return TRUE;
    }

    return FALSE;
}

void adjust_gain(){
    
    //  Finding max signal value
    maxFindAuxInt = 0; 
    for (int i = 0; i < time_buffer_len; i+= 2)
    	if(time_buffer[i] > maxFindAuxInt)
    		maxFindAuxInt = time_buffer[i]; 
    maxFindAuxInt -= 128; 

	// Gain controller
	current_gain_index += (goal_signal_amplitude - abs(maxFindAuxInt))/controller_coef; 
		
	// Ensuring array bounds
	if(current_gain_index > max_gain_index)
		current_gain_index = max_gain_index; 
	else if(current_gain_index < 0)
		current_gain_index = 0; 
	// Setting gain value
	if (rtlsdr_set_tuner_gain(dev, valid_gain_values[(int)current_gain_index]) < 0)
		fprintf(stderr, "WARNING: Failed to set up fixed gain.\n");

}

void setup_rtlsdr(){

	device_count = rtlsdr_get_device_count();
	if (!device_count){
		fprintf(stderr, "No supported devices found.\n");
		exit(1);
	}

	r = rtlsdr_open(&dev, dev_index);
	if (r < 0){
		fprintf(stderr, "Failed to open rtlsdr device #%d.\n", dev_index);
		exit(1);
	}

	r = rtlsdr_set_sample_rate(dev, samp_rate);
	if (r < 0)
		fprintf(stderr, "WARNING: Failed to set sample rate.\n");

	r = rtlsdr_set_center_freq(dev, center_freq);
	if (r < 0)
		fprintf(stderr, "WARNING: Failed to set center freq.\n");

	// Setting gain mode (auto(0) or manual(1))
	r = rtlsdr_set_tuner_gain_mode(dev, 1);
	if (r < 0)
		fprintf( stderr, "WARNING: Failed to enable manual gain.\n");

	// Setting gain value
	r = rtlsdr_set_tuner_gain(dev, valid_gain_values[(int)current_gain_index]);
	if (r < 0)
		fprintf(stderr, "WARNING: Failed to set up fixed gain.\n");

	r = rtlsdr_reset_buffer(dev);
	if (r < 0)
		fprintf(stderr, "WARNING: Failed to reset buffers.\n");

}

void populate_gps(){
	alt	= return_alti();
	lon	= return_long();
	lat	= return_lati();
}

void siginthandler(int sig)
{
	printf("Got SIGINT!!!\n");
	exit(-1);
}
