#include "Status_Module.hpp"

Status_Module::Status_Module() : state(CHECK_FOR_START){
	status = &_status;
	_own_status = 1;
	status->storage = STR_INIT;
	status->sdr = SDR_INIT;
	status->system = SYS_INIT;
	status->gps = GPS_INIT;
}

Status_Module::Status_Module(StatusPacket* packet) : state(CHECK_FOR_START){
	status = packet;
	_own_status = 0;
	status->storage = STR_INIT;
	status->sdr = SDR_INIT;
	status->system = SYS_INIT;
}


Status_Module::~Status_Module(){
}

int is_whitespace(char c){
	switch(c){
		case ' ':
		case '\t':
			return 1;
		default:
			return 0;
	}
}

int is_digit(char c){
	switch(c){
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '0':
			return 1;
		default:
			return 0;
	}
}

int parse_digit(char c){
	return (int)(c - '0');
}

void Status_Module::commit_value(){
	switch(char2){
		case 'T':
			// STR
			status->storage = (StorageState)value;
			return;
		case 'Y':
			status->system = (SystemState)value;
			return;
		case 'D':
			status->sdr = (SDRState)value;
			return;
		default:
			return;
	}
}

int Status_Module::decode(char c){
	// {"STR": 3 , "SYS": 3 , "SDR": 4}
	switch(state){
		case CHECK_FOR_START:
			if(c == '{')
				state = GET_START_QUOTE;
			else
				state = CHECK_FOR_START;
			return 0;
		case GET_START_QUOTE:
			if(c == '"')
				state = GET_1_CHAR;
			else if(is_whitespace(c)){
				state = GET_START_QUOTE;
			}else
				state = CHECK_FOR_START;
			return 0;
		case GET_1_CHAR:
			char1 = c;
			state = GET_2_CHAR;
			return 0;
		case GET_2_CHAR:
			char2 = c;
			state = GET_3_CHAR;
			return 0;
		case GET_3_CHAR:
			char3 = c;
			state = GET_END_QUOTE;
			return 0;
		case GET_END_QUOTE:
			if(c == '"'){
				state = GET_COLON;
			}else{
				state = GET_START_QUOTE;
			}
			return 0;
		case GET_COLON:
			if(c == ':'){
				state = GET_VALUE;
				value = 0;
			}else if(is_whitespace(c)){
				state = GET_COLON;
			}else{
				state = GET_START_QUOTE;
			}
			return 0;
		case GET_VALUE:
			if(is_digit(c)){
				state = GET_VALUE;
				value = value * 10 + parse_digit(c);
			}else if(is_whitespace(c)){
				state = GET_VALUE;
			}else if(c == ','){
				commit_value();
				state = GET_START_QUOTE;
			}else if(c == '}'){
				commit_value();
				state = CHECK_FOR_START;
				// complete!
				return 1;
			}else{
				state = CHECK_FOR_START;
			}
			return 0;
		default:
			return 0;
	}
}

const StatusPacket& Status_Module::getStatus() const{
	return *status;
}
