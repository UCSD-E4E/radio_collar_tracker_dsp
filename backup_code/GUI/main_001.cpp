#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>

#define WINDOW_W 		640
#define WINDOW_H 		480
#define MARGIN 			4
#define LABEL_SPACE  	48
#define ELEMENT_H  		20
#define BUTTON_W  		64
#define TEXT_W  		128
#define TEXT_S  		12
#define TEXT_BUFFER_S 	256
#define WINDOW_TITLE 	"Radio Collar Tracker: Pre-Flight Config"

using namespace std;

char str_buffer[TEXT_BUFFER_S];

class Elements{
public:	
	Fl_Window 	*window;
	Fl_Button 	*button;
	Fl_Input 	*input;
	Fl_Output 	*output;
	Elements(){

		window 	= new Fl_Window(WINDOW_W,WINDOW_H,WINDOW_TITLE);
		window->begin();
			button 	= new Fl_Button	(LABEL_SPACE,MARGIN,BUTTON_W,ELEMENT_H,"OK");
			input 	= new Fl_Input	(LABEL_SPACE,2*MARGIN+ELEMENT_H,TEXT_W,ELEMENT_H,"In:"); 
			output 	= new Fl_Output	(LABEL_SPACE,3*MARGIN+2*ELEMENT_H,TEXT_W,ELEMENT_H,"Out:"); 
		window->end();

		button 	->labelfont(FL_HELVETICA);
		input 	->labelfont(FL_HELVETICA);
		output 	->labelfont(FL_HELVETICA);

		button 	->labelsize(TEXT_S);
		input 	->labelsize(TEXT_S);
		output 	->labelsize(TEXT_S);

		button 	->box(FL_UP_BOX);
		input 	->box(FL_DOWN_BOX);
		output 	->box(FL_DOWN_BOX);
	}
};

void button_cb(Fl_Widget *w, void *v){
	Elements* e = (Elements*)v;
	e->output->value(e->input->value());
	cout << e->input->value() << endl;
}

int main(int argc, char **argv) {

	Elements *elements = new Elements();

	elements->button->callback(button_cb,elements);

	elements->window->show(argc, argv);
	return Fl::run();
}