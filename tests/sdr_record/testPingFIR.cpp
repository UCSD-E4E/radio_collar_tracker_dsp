#include <complex>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <limits>
#define private public
#define protected public
#include <ping_fir.hpp>
#undef private
#undef protected

#include <cassert>
#include <iostream>

void test_constructor(){
	RTT::PingFIR test_obj(172500000, 2000000, 200);
	assert(test_obj._num_taps == 200);
	assert(test_obj._filter_taps != nullptr);
	
}

int main(int argc, char const *argv[])
{
	test_constructor();
	return 0;
}