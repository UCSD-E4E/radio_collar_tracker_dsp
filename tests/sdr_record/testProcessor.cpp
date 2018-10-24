#include "mixer.hpp"
#include "resampler.hpp"
#include "ping_fir.hpp"
#include "ping_classifier.hpp"

#define private public
#define protected public
#include <Processor.hpp>
#undef private
#undef protected

#include <cassert>
#include <iostream>

void testSimulated(){

}

void testRecorded(){

}

int main(int argc, char const *argv[])
{
	testSimulated();
	testRecorded();
	return 0;
}