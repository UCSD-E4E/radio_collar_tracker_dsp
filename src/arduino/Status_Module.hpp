#ifndef __STATUS_MODULE__
#define __STATUS_MODULE__

#include "Status_Packet.hpp"

class Status_Module{
public:

	

	const StatusPacket& getStatus() const;


	Status_Module();
	Status_Module(StatusPacket* packet);
	~Status_Module();
	int decode(char c);
private:
	enum ParserState{
		CHECK_FOR_START,
		GET_START_QUOTE,
		GET_1_CHAR,
		GET_2_CHAR,
		GET_3_CHAR,
		GET_END_QUOTE,
		GET_COLON,
		GET_VALUE
	};

	char char1;
	char char2;
	char char3;
	unsigned int value;

	ParserState state;
	StatusPacket* status;

	void commit_value();

	StatusPacket _status;

	int _own_status;
};

#endif