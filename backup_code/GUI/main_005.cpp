#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_File_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/fl_ask.H>

#define MARGIN 							8
#define LABEL_SPACE  					128
#define ELEMENT_H  						20
#define BUTTON_W  						64
#define TEXT_W  						192
#define TEXT_S  						12
#define BUFFER_SIZE 					256
#define WINDOW_TITLE 					"RCT: Pre-Flight Configuration"
#define FILE_PATH_NAME 				 	"file_path"

#define CENTER_FREQ_LABEL 				"Center Frequency (Hz):"
#define SAMP_RATE_LABEL 				"Sampling Rate (Hz):"
#define TIMEOUT_INTERRUPT_LABEL 		"Timeout Interrupt (ms):"
#define GOAL_SIGNAL_AMPLITUDE_LABEL 	"Goal Signal Amplitude (%):"
#define CONTROLLER_COEF_LABEL 			"Gain Controller Coeficient:"
#define NUM_FRAMES_PER_FILE_LABEL 		"Number of Frames per File:"
#define CFG_PATH_LABEL 					"Configuration Path:"

#define BUTTON_LOAD_LABEL 				"Load"
#define BUTTON_SAVE_LABEL 				"Save"
#define NUM_STR_FORMAT 					"%d"
#define FILE_NOT_FOUND_EM				"Configuration File not Found!"
#define LOADED_SUCCESSFULLY_M			"Configuration File Loaded Successfully!"
#define SAVED_SUCCESSFULLY_M		 	"Configuration File Saved Successfully!"
#define INTERNAL_FILE_ERROR		 		"Internal File Error!"
#define FILE_READ_ERROR_EM				"ERROR Reading Data Config File!"

using namespace std;

int aux_x 		= 0; 
int aux_y 		= 0; 
int aux_w 		= 0; 
int aux_h 		= 0; 
int window_w 	= 640;
int window_h 	= 480;
int intAux 		= 0;


FILE *fileStream;
char fileBuffer[BUFFER_SIZE]; 
char aux_str[BUFFER_SIZE];

class Elements{
public:	
	Fl_Window  		*window;
	Fl_Button 		*button_load;
	Fl_Button 		*button_save;
	Fl_File_Input 	*cfg_path;
	Fl_Int_Input 	*center_freq;
	Fl_Int_Input 	*samp_rate;
	Fl_Int_Input 	*timeout_interrupt;
	Fl_Int_Input 	*goal_signal_amplitude;
	Fl_Int_Input 	*controller_coef;
	Fl_Int_Input 	*num_frames_per_file;
	Elements(){

		window 	= new Fl_Window(window_w,window_h,WINDOW_TITLE);
		window->begin();
			aux_x = MARGIN;
			aux_y = ELEMENT_H+MARGIN;
			aux_h = ELEMENT_H;
			aux_w = TEXT_W;
			center_freq 			= new Fl_Int_Input 	(aux_x,aux_y,aux_w,aux_h,CENTER_FREQ_LABEL);			
			aux_x += aux_w+MARGIN;
			samp_rate 				= new Fl_Int_Input 	(aux_x,aux_y,aux_w,aux_h,SAMP_RATE_LABEL);

			aux_x = MARGIN;	
			aux_y += 2*ELEMENT_H+MARGIN;
			timeout_interrupt 		= new Fl_Int_Input 	(aux_x,aux_y,aux_w,aux_h,TIMEOUT_INTERRUPT_LABEL);		
			aux_x += aux_w+MARGIN;
			goal_signal_amplitude 	= new Fl_Int_Input 	(aux_x,aux_y,aux_w,aux_h,GOAL_SIGNAL_AMPLITUDE_LABEL);

			aux_x = MARGIN;	
			aux_y += 2*ELEMENT_H+MARGIN;
			controller_coef 		= new Fl_Int_Input 	(aux_x,aux_y,aux_w,aux_h,CONTROLLER_COEF_LABEL);	
			aux_x += aux_w+MARGIN;
			num_frames_per_file 	= new Fl_Int_Input 	(aux_x,aux_y,aux_w,aux_h,NUM_FRAMES_PER_FILE_LABEL);

			aux_y += 2*ELEMENT_H+MARGIN;
			aux_h = ELEMENT_H+2*MARGIN;
			aux_x = MARGIN;
			aux_w = 2*TEXT_W+MARGIN;
			cfg_path 				= new Fl_File_Input (aux_x,aux_y,aux_w,aux_h,CFG_PATH_LABEL);

			aux_h = ELEMENT_H;
			aux_y += ELEMENT_H+MARGIN+2*MARGIN;
			aux_x = aux_w-2*BUTTON_W;
			aux_w = BUTTON_W;
			button_load 			= new Fl_Button		(aux_x,aux_y,aux_w,aux_h,BUTTON_LOAD_LABEL); 
			aux_x += BUTTON_W+MARGIN;
			button_save 			= new Fl_Button		(aux_x,aux_y,aux_w,aux_h,BUTTON_SAVE_LABEL); 
		window->end();
		window_w = 2*TEXT_W + 3*MARGIN;
		window_h = aux_y + aux_h + MARGIN;
		window->resize(1,0,window_w,window_h);

		center_freq 			->labelfont(FL_HELVETICA);
		samp_rate 				->labelfont(FL_HELVETICA);
		timeout_interrupt 		->labelfont(FL_HELVETICA);
		goal_signal_amplitude 	->labelfont(FL_HELVETICA);
		controller_coef 		->labelfont(FL_HELVETICA);
		num_frames_per_file 	->labelfont(FL_HELVETICA);
		cfg_path 				->labelfont(FL_HELVETICA);
		button_load 			->labelfont(FL_HELVETICA);
		button_save 			->labelfont(FL_HELVETICA);

		center_freq 			->labelsize(TEXT_S);
		samp_rate 				->labelsize(TEXT_S);
		timeout_interrupt 		->labelsize(TEXT_S);
		goal_signal_amplitude 	->labelsize(TEXT_S);
		controller_coef 		->labelsize(TEXT_S);
		num_frames_per_file 	->labelsize(TEXT_S);
		cfg_path 				->labelsize(TEXT_S);
		button_load 			->labelsize(TEXT_S);
		button_save 			->labelsize(TEXT_S);

		center_freq 			->box(FL_DOWN_BOX);
		samp_rate 				->box(FL_DOWN_BOX);
		timeout_interrupt 		->box(FL_DOWN_BOX);
		goal_signal_amplitude 	->box(FL_DOWN_BOX);
		controller_coef 		->box(FL_DOWN_BOX);
		num_frames_per_file 	->box(FL_DOWN_BOX);
		cfg_path 				->box(FL_DOWN_BOX);
		button_load 			->box(FL_UP_BOX);
		button_save 			->box(FL_UP_BOX);

		button_save 			->shortcut(FL_Enter);

		center_freq 			->align(FL_ALIGN_TOP_LEFT);
		samp_rate 				->align(FL_ALIGN_TOP_LEFT);
		timeout_interrupt 		->align(FL_ALIGN_TOP_LEFT);
		goal_signal_amplitude 	->align(FL_ALIGN_TOP_LEFT);
		controller_coef 		->align(FL_ALIGN_TOP_LEFT);
		num_frames_per_file 	->align(FL_ALIGN_TOP_LEFT);
		cfg_path 				->align(FL_ALIGN_TOP_LEFT);
	}
};

char* loadParameter();

void load(void *v);

void save(void *v);

void load_cfg_path(void *v);

void save_cfg_path(void *v);

void button_load_cb(Fl_Widget *w, void *v);

void button_save_cb(Fl_Widget *w, void *v);

int main(int argc, char **argv) {

	Elements *elements = new Elements();

	load_cfg_path(elements); 
	load(elements); 

	elements->button_load->callback(button_load_cb,elements);
	elements->button_save->callback(button_save_cb,elements);

	elements->window->show(argc, argv);
	return Fl::run();
}

char* loadParameter(){
    if (fgets(fileBuffer, BUFFER_SIZE, fileStream) == NULL) {
		fl_alert(FILE_READ_ERROR_EM);
		exit(1);
    }
    intAux = 0; 
    while(fileBuffer[intAux] != ':')
    	intAux++;
    if(intAux >= BUFFER_SIZE){
		fl_alert(FILE_READ_ERROR_EM);
		exit(1);
    }
	sprintf(aux_str,NUM_STR_FORMAT,atoi(fileBuffer+intAux+1));
    return aux_str; 
}

void load(void *v){
	Elements* e = (Elements*)v;
	fileStream 	= fopen(e->cfg_path->value(), "r");
	if (fileStream == NULL){
		fl_alert(FILE_NOT_FOUND_EM);
	}else{
		e->center_freq 				->value(loadParameter());
		e->samp_rate 				->value(loadParameter());
		e->timeout_interrupt 		->value(loadParameter());
		e->goal_signal_amplitude 	->value(loadParameter());
		e->controller_coef 			->value(loadParameter());
		e->num_frames_per_file 		->value(loadParameter());
 		fclose(fileStream);
		save_cfg_path(v);
		fl_message(LOADED_SUCCESSFULLY_M);
	}
}

void save(void *v){
	Elements* e = (Elements*)v;
	fileStream = fopen (e->cfg_path->value(), "r+");
	if (fileStream == NULL){
		fl_alert(FILE_NOT_FOUND_EM);
	}else{
		fprintf(fileStream, "center_freq: %s \nsamp_rate: %s \ntimeout_interrupt: %s \ngoal_signal_amplitude: %s \ncontroller_coef: %s \nnum_frames_per_file: %s \n ",
		 e->center_freq->value(), e->samp_rate->value(), e->timeout_interrupt->value(), e->goal_signal_amplitude->value(), e->controller_coef->value(), e->num_frames_per_file->value());
		fclose(fileStream);
		save_cfg_path(v);
		fl_message(SAVED_SUCCESSFULLY_M);
	}
}

void load_cfg_path(void *v){
	Elements* e = (Elements*)v;
	fileStream = fopen (FILE_PATH_NAME, "r");
	if (fileStream == NULL || fscanf(fileStream, "%s", aux_str) == 0) {
		fl_alert(INTERNAL_FILE_ERROR);
    }else{
		e->cfg_path->value(aux_str);
		fclose(fileStream);
	}
}

void save_cfg_path(void *v){
	Elements* e = (Elements*)v;
	fileStream = fopen (FILE_PATH_NAME, "w");
	if (fileStream == NULL) {
		fl_alert(INTERNAL_FILE_ERROR);
    }else{
		fprintf(fileStream,"%s",e->cfg_path->value());
		fclose(fileStream);
	}
}

void button_load_cb(Fl_Widget *w, void *v){
	load(v);
}

void button_save_cb(Fl_Widget *w, void *v){
	save(v);
}