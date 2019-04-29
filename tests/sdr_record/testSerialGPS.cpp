#include <gps_core.hpp>
#include <string>
#include <thread>
#include <termios.h>

#define private public
#define protected public
#include <serial_gps.hpp>
#undef private
#undef protected

#include <iostream>
#include <assert.h>
#include <chrono>
#include <stdlib.h>
#include <pty.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <poll.h>

void testParsing();
void testTimingPTY();
void testTimingSerial();
void testWrite(int, const char*, int, int, int);
void testConstructor();

void testParsing(){
	RTT::SerialGPS testObj{"."};

	std::string line = "{\"lat\": 327054113, \"hdg\":270, \"lon\": -1171710165,"
		" \"tme\": 170655, \"run\": true, \"fix\": 1, \"sat\": 14, \"dat\": 280419}";

	auto parseTime = std::chrono::system_clock::now().time_since_epoch() / 
		std::chrono::milliseconds(1);
	RTT::Location& point = testObj.parseLocation(line);

	assert(std::abs((std::int64_t)point.ltime - (std::int64_t)parseTime) < 100);
	assert(point.gtime == 1556471215);
	assert(point.lat == 327054113);
	assert(point.lon == -1171710165);
	assert(point.hdg == 270);

	std::string line2 = "{\"lat\": 1024, \"hdg\": -90, \"lon\": -135468321, "
	"\"tme\": 164726, \"run\": true, \"fix\": 3, \"sat\": 2, \"dat\": 280419}";

	parseTime = std::chrono::system_clock::now().time_since_epoch() / 
		std::chrono::milliseconds(1);
	RTT::Location& point2 = testObj.parseLocation(line2);
	assert(std::abs((std::int64_t)point2.ltime - (std::int64_t)parseTime) < 100);
	assert(point2.gtime == 1556470046);
	assert(point2.lat == 1024);
	assert(point2.lon == -135468321);
	assert(point2.hdg == -90);
}

void testTimingPTY(){
	// create pty

	int fdm = posix_openpt(O_RDWR | O_NOCTTY);
	if(fdm < 0){
		std::cerr << "Error " << errno << " on posix_openpt()" << std::endl;
		assert(false);
	}

	int rc = grantpt(fdm);
	if(rc != 0){
		std::cerr << "Error " << errno << " on grantpt()" << std::endl;
		assert(false);
	}

	rc = unlockpt(fdm);
	if(rc != 0){
		std::cerr << "Error " << errno << " on unlockpt()" << std::endl;
		assert(false);
	}

	char* name = ptsname(fdm);
	
	std::cout << name << std::endl;

	struct termios tty;
	memset(&tty, 0, sizeof(tty));

	if(tcgetattr(fdm, &tty) != 0){
		std::cerr << "Error << " << errno << " from tcgetattr! " << 
			strerror(errno) << std::endl;
		assert(false);
	}
	tty.c_oflag &= ~PARENB;
	tty.c_oflag &= ~CSTOPB;
	tty.c_oflag |= CS8;
	tty.c_oflag &= ~CRTSCTS;
	tty.c_oflag |= CREAD | CLOCAL;
	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO;
	tty.c_lflag &= ~ECHOE;
	tty.c_lflag &= ~ECHONL;
	tty.c_lflag &= ~ISIG;
	tty.c_iflag &= ~(IXON | IXOFF | IXANY);
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
	tty.c_oflag &= ~OPOST;
	tty.c_oflag &= ~ONLCR;
	tty.c_cc[VMIN] = 0;
	tty.c_cc[VTIME] = 10;
	cfsetispeed(&tty, B9600);
	cfsetospeed(&tty, B9600);

	if (tcsetattr(fdm, TCSANOW, &tty) != 0) {
		std::cerr << "Error " << errno << " from tcsetattr: " << 
			strerror(errno) << std::endl;
	}

	testWrite(fdm, name, 5, 1, 5);

	close(fdm);
}

void testWrite(int master, const char* slave_device, int init_sleep_s, int sleep_s, int n){
	RTT::SerialGPS testObj{slave_device};
	std::queue<RTT::Location*> o_q{};
	std::mutex o_m{};
	std::condition_variable o_v{};

	std::string testData = "{\"lat\": 327054113, \"hdg\":270, \"lon\": "
		"-1171710165, \"tme\": 164753, \"run\": true, \"fix\": 1, \"sat\": "
		"14, \"dat\": 280419}";

	testObj.start(o_q, o_m, o_v);
	sleep(init_sleep_s);
	for(int i = 0; i < n; i++){
		if(write(master, testData.c_str(), testData.length()) < (int)testData.length()){
			std::cerr << "Warning! didn't write as much into the ptty as we wanted!" << std::endl;
			testObj.stop();
			return;
		}
		sleep(1);
		assert(o_q.size() == i + 1);
	}
	testObj.stop();
}

void testConstructor(){
	char path[] = "test";
	std::string path2{"test"};
	RTT::SerialGPS testObj{path};
	assert(testObj.dev_path->compare(std::string{"test"}) == 0);
	RTT::SerialGPS testObj2{path2};
	assert(testObj2.dev_path->compare(std::string{"test"}) == 0);
}

void testTimingSerial(){
	std::string serial_loopback = "/host-dev/ttyUSB0";


	int master = open(serial_loopback.c_str(), O_RDWR | O_NOCTTY);
	if(master < 0){
		std::cerr << "Error " << errno << " on posix_openpt()" << std::endl;
		assert(false);
	}

	struct termios tty;
	memset(&tty, 0, sizeof(tty));

	if(tcgetattr(master, &tty) != 0){
		std::cerr << "Error << " << errno << " from tcgetattr! " << 
			strerror(errno) << std::endl;
		assert(false);
	}
	tty.c_oflag &= ~PARENB;
	tty.c_oflag &= ~CSTOPB;
	tty.c_oflag |= CS8;
	tty.c_oflag &= ~CRTSCTS;
	tty.c_oflag |= CREAD | CLOCAL;
	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO;
	tty.c_lflag &= ~ECHOE;
	tty.c_lflag &= ~ECHONL;
	tty.c_lflag &= ~ISIG;
	tty.c_iflag &= ~(IXON | IXOFF | IXANY);
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
	tty.c_oflag &= ~OPOST;
	tty.c_oflag &= ~ONLCR;
	tty.c_cc[VMIN] = 0;
	tty.c_cc[VTIME] = 10;
	cfsetispeed(&tty, B9600);
	cfsetospeed(&tty, B9600);

	if (tcsetattr(master, TCSANOW, &tty) != 0) {
		std::cerr << "Error " << errno << " from tcsetattr: " << 
			strerror(errno) << std::endl;
	}

	testWrite(master, serial_loopback.c_str(), 0, 1, 5);
	close(master);
}

int main(int argc, char const *argv[]){
	std::cout << "Testing constructor" << std::endl;
	testConstructor();
	std::cout << "Testing parsing" << std::endl;
	testParsing();
	std::cout << "Testing timing" << std::endl;
	testTimingPTY();
	return 0;
}