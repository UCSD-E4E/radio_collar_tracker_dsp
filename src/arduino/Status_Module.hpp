#ifndef __STATUS_MODULE__
#define __STATUS_MODULE__

#include "Status_Packet.hpp"

/**
 * Status Module for interpreting status information from the OBC.
 */
class Status_Module{
public:
	/**
	 * Returns a const reference to the status packet representing the current
	 * state of all RTT systems.
	 * @return const reference to a StatusPacket
	 */
	const StatusPacket& getStatus() const;

	/**
	 * Default constructor.  This initializes the internal state machines. This
	 * decoder is ready to operate immediately after this constructor.  This
	 * constructor will use an internally owned StatusPacket instance.
	 */
	Status_Module();

	/**
	 * Alternate constructor to use a specific StatusPacket instance.  This
	 * decoder is ready to operate immediately after this constructor.
	 */
	Status_Module(StatusPacket* packet);

	/**
	 * Destructor.
	 */
	~Status_Module();

	/**
	 * Decodes each status packet per character - feed each character of the
	 * stream into this function to decode the entire message.  This function
	 * will return 1 iff a full message has been received, 0 otherwise.
	 * @param  c Next character in the status packet.
	 * @return   1 if a full message has been received, 0 otherwise.
	 */
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