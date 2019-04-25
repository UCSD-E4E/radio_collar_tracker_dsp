#include <queue>
#include <iostream>
#include <location.hpp>

#define private public
#define protected public
#include "gps_test.hpp"
#include "gps.hpp"
#undef private
#undef protected

#include <cassert>
#include <string>

void testRead(){
	RTT::GPSTest* testCore = new RTT::GPSTest("/home/ntlhui/workspace/tmp/testD"
		"ata/GPS_000001");
	assert(*(testCore->data_source));
	std::string line;
	std::getline(*(testCore->data_source), line);
	assert(!line.empty());
}

void testParse(){
	RTT::GPSTest* testCore = new RTT::GPSTest("/home/ntlhui/workspace/tmp/testD"
		"ata/GPS_000001");
	std::string line("1502770060.460, 328871143, -1172354981, 1518628714.000, 0"
		", -1, 1, 2, -1, 120");
	const RTT::Location& point = testCore->parseLocation(line);
	assert(point.ltime == 1502770060460);
	assert(point.lat == 328871143);
	assert(point.lon == -1172354981);
	assert(point.gtime == 1518628714000);
	assert(point.alt == 0);
	assert(point.rel_alt == -1);
	assert(point.vx == 1);
	assert(point.vy == 2);
	assert(point.vz == -1);
	assert(point.hdg == 120);
}

void testReadAll(){
	RTT::GPSTest* testCore = new RTT::GPSTest("/home/ntlhui/workspace/tmp/testD"
		"ata/GPS_000001");
	std::queue<RTT::Location*> out_queue;
	std::mutex out_mutex;
	std::condition_variable out_var;

	testCore->start(out_queue, out_mutex, out_var);
	testCore->stop();
	std::cout << out_queue.size() << std::endl;
	assert(out_queue.size() == 730);
}

void testComposition(){
	RTT::GPS* newGPS = new RTT::GPS(RTT::GPS::Protocol::TEST_FILE, "/home/ntlhui"
		"/workspace/tmp/testData/GPS_000001");
	newGPS->start();
	newGPS->stop();
	assert(newGPS->pointLookup.size() == 730);
}

void testLookup(){
	RTT::GPS* newGPS = new RTT::GPS(RTT::GPS::Protocol::TEST_FILE, "/home/ntlhui"
		"/workspace/tmp/testData/GPS_000001");
	newGPS->start();
	const RTT::Location* point = newGPS->getPositionAt(1502770095000);
	assert(point->lat == 328871181);
	assert(point->lon == -1172355018);
}

void testUsage(){
	RTT::GPS gps(RTT::GPS::Protocol::TEST_FILE, "/home/ntlhui/workspace/tmp/testData/GPS_000001");
	gps.start();
	gps.stop();
	std::cout << "First location: " << gps.pointLookup.begin()->first.start << " to " << gps.pointLookup.begin()->first.stop << std::endl;
}

int main(int argc, char const *argv[]){
	testRead();
	testParse();
	testReadAll();
	testComposition();
	testUsage();
	return 0;
}