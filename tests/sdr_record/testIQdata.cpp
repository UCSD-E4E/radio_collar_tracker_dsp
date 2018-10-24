#include "iq_data.hpp"
#include <cassert>
#include <complex.h>

void testCopy(){
	RTT::IQdata* data1 = new RTT::IQdata(1024);
	data1->data->at(0) = std::complex<short>(0xf00d, 0);
	RTT::IQdata* data2 = new RTT::IQdata(*data1);
	assert(data1->data != data2->data);
	assert(data2->data->at(0) == std::complex<short>(0xf00d, 0));
}

int main(int argc, char const *argv[])
{
	testCopy();
	return 0;
}