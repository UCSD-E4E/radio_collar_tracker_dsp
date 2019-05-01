#include <cassert>
#include <iostream>
#include "Status_Packet.hpp"

#define private public
#define protected public
#include "Status_Module.hpp"
#undef private
#undef public

using namespace std;

const std::string decode_state(Status_Module::ParserState state){
	switch(state){
		case Status_Module::CHECK_FOR_START:
			return "CHECK_FOR_START";
		case Status_Module::GET_START_QUOTE:
			return "GET_START_QUOTE";
		case Status_Module::GET_1_CHAR:
			return "GET_1_CHAR";
		case Status_Module::GET_2_CHAR:
			return "GET_2_CHAR";
		case Status_Module::GET_3_CHAR:
			return "GET_3_CHAR";
		case Status_Module::GET_END_QUOTE:
			return "GET_END_QUOTE";
		case Status_Module::GET_COLON:
			return "GET_COLON";
		case Status_Module::GET_VALUE:
			return "GET_VALUE";
		default:
			return "Unknown";
	}
}

void testSelfPacket(const char* testString){
	Status_Module testModule;
	// std::cout << decode_state(testModule.state) << std::endl;

	for(size_t i = 0; testString[i] != '\0'; i++){
		int retval = testModule.decode(testString[i]);
		// std::cout << testString[i] << std::endl;
		// std::cout << decode_state(testModule.state) << ", retval = " << retval << ", value: " << testModule.value << std::endl;
	}
	const StatusPacket status = testModule.getStatus();
	// std::cout << "Storage: " << status.storage << std::endl;
	// std::cout << "System: " << status.system << std::endl;
	// std::cout << "SDR: " << status.sdr << std::endl;
	assert(status.storage == STR_READY);
	assert(status.system == SYS_RETRY);
	assert(status.sdr == SDR_FAIL);
}

void testGlobalPacket(const char* testString){
	StatusPacket packet;

	Status_Module testModule(&packet);
	// std::cout << decode_state(testModule.state) << std::endl;

	for(size_t i = 0; testString[i] != '\0'; i++){
		int retval = testModule.decode(testString[i]);
		// std::cout << testString[i] << std::endl;
		// std::cout << decode_state(testModule.state) << ", retval = " << retval << ", value: " << testModule.value << std::endl;
	}
	// std::cout << "Storage: " << packet.storage << std::endl;
	// std::cout << "System: " << packet.system << std::endl;
	// std::cout << "SDR: " << packet.sdr << std::endl;
	assert(packet.storage == STR_READY);
	assert(packet.system == SYS_RETRY);
	assert(packet.sdr == SDR_FAIL);
}

int main(int argc, char const *argv[]){
	const char* testString = "{ \"STR\" : 1 , \"SYS\" : 3 , \"SDR\" : 2  }  ";
	testSelfPacket(testString);
	testGlobalPacket(testString);
	return 0;
}