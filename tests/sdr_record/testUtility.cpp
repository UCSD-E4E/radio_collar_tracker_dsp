#include <utility.hpp>
#include <cassert>
#include <list>
#include <complex>

#include <iostream>

void testConvolve(){

	const std::size_t TAP_LENGTH = 128;
	const std::size_t TEST_CASE_LENGTH = 1024;

	std::list<std::complex<double>> testCase1;
	for(std::size_t i = 0; i < TEST_CASE_LENGTH; i++){
		testCase1.push_back(std::complex<double>(1, 0));
	}
	std::complex<double> testTaps1[TAP_LENGTH];
	for(std::size_t i = 0; i < TAP_LENGTH; i++){
		testTaps1[i] = std::complex<double>(1, 0);
	}
	std::complex<double> retval = RTT::convolve(testCase1, testTaps1, TAP_LENGTH, 0);
	assert(std::abs(retval) == 128);
}

void testDB(){
	assert(RTT::amplitudeToDB(1) == 0);
	assert(RTT::amplitudeToDB(10) == 20.0);
	assert(RTT::amplitudeToDB(100) == 40.0);
	assert(RTT::powerToDB(1) == 0);
	assert(RTT::powerToDB(10) == 10.0);
	assert(RTT::powerToDB(100) == 20.0);
}

void testSine(){
	std::complex<double>* testSignal = RTT::generateSinusoid(1000, 2000000, 
		2000000);
	assert(testSignal[0].real() == 1);
	assert(testSignal[0].imag() == 0);
	assert(std::abs(testSignal[500].real() - 0) < 0.001);
	assert(std::abs(testSignal[500].imag() - 1) < 0.001);
	assert(std::abs(testSignal[1000].real() - -1) < 0.001);
	assert(std::abs(testSignal[1000].imag() - 0) < 0.001);
	assert(std::abs(testSignal[1500].real() - 0) < 0.001);
	assert(std::abs(testSignal[1500].imag() - -1) < 0.001);
	assert(std::abs(testSignal[2000].real() - 1) < 0.001);
	assert(std::abs(testSignal[2000].imag()) < 0.001);
}

void testVSine(){
	std::vector<std::complex<double>>& testSignal = *RTT::generateVectorSinusoid(1000, 2000000, 
		2000000);
	assert(testSignal[0].real() == 1);
	assert(testSignal[0].imag() == 0);
	assert(std::abs(testSignal[500].real() - 0) < 0.001);
	assert(std::abs(testSignal[500].imag() - 1) < 0.001);
	assert(std::abs(testSignal[1000].real() - -1) < 0.001);
	assert(std::abs(testSignal[1000].imag() - 0) < 0.001);
	assert(std::abs(testSignal[1500].real() - 0) < 0.001);
	assert(std::abs(testSignal[1500].imag() - -1) < 0.001);
	assert(std::abs(testSignal[2000].real() - 1) < 0.001);
	assert(std::abs(testSignal[2000].imag()) < 0.001);
}


int main(int argc, char const *argv[])
{
	testConvolve();
	testDB();
	testSine();
	testVSine();
	return 0;
}