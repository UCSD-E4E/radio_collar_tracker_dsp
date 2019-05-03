#include "serial_gps.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <chrono>
#include <poll.h>
#include <termios.h>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace pt = boost::property_tree;
namespace ptme = boost::posix_time;

// #define DEBUG

#ifdef DEBUG
#include <fstream>
#include <iostream>
#endif

namespace RTT{
	Location& SerialGPS::parseLocation(const std::string line){
		Location* retval = new Location{};

		std::stringstream data{line, std::ios_base::in};
		pt::ptree root;
		pt::read_json(data, root);

		retval->ltime = std::chrono::system_clock::now().time_since_epoch() / 
			std::chrono::milliseconds(1);
		std::string tme = root.get<std::string>("tme");
		std::size_t hr = std::stoi(tme.substr(0, 2));
		std::size_t min = std::stoi(tme.substr(2, 2));
		std::size_t sec = std::stoi(tme.substr(4, 2));
		std::size_t dsec = 0;
		if(tme.length() > 6){
			dsec = std::stoi(tme.substr(5, 2));
		}
		std::string dat = root.get<std::string>("dat");
		std::size_t day = std::stoi(dat.substr(0, 2));
		std::size_t mon = std::stoi(dat.substr(2, 2));
		std::size_t yr = std::stoi(dat.substr(4, 2));

		std::stringstream datestream;
		datestream << "20" << yr << "/" << mon << "/" << day << " " << hr << ":" << 
			min << ":" << sec << "." << dsec;

		ptme::ptime epoch = ptme::time_from_string("1970-01-01 00:00:00.000");
		ptme::ptime timeObj = ptme::time_from_string(datestream.str());

		ptme::time_duration const diff = timeObj - epoch;
		
		retval->gtime = diff.total_seconds();
		retval->lat = root.get<int64_t>("lat");
		retval->lon = root.get<int64_t>("lon");
		retval->hdg = root.get<int16_t>("hdg");

		return *retval;
	}

	void SerialGPS::start(std::queue<Location*>& o_q, std::mutex& o_m,
		std::condition_variable& o_v){
		_run = true;

		_thread = new std::thread(&SerialGPS::_process, this, std::ref(o_q),
			std::ref(o_m), std::ref(o_v));
	}

	void SerialGPS::stop(){
		_run = false;
		_thread->join();
		delete _thread;
	}

	SerialGPS::SerialGPS(const char* path){
		dev_path = new std::string{path};
	}

	SerialGPS::SerialGPS(const std::string path){
		dev_path = new std::string{path};
	}

	void SerialGPS::_process(std::queue<Location*>& o_q, std::mutex& o_m,
		std::condition_variable& o_v){
		int serial_device = open(dev_path->c_str(), O_RDWR | O_NOCTTY);

		if(serial_device < 0){
			return;
		}

		struct termios tty;
		memset(&tty, 0, sizeof(tty));

		if(tcgetattr(serial_device, &tty) != 0){
			return;
		}

		tty.c_oflag &= ~(PARENB | CSTOPB | CRTSCTS | OPOST | ONLCR);
		tty.c_oflag |= (CS8 | CREAD | CLOCAL);

		tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHONL | ISIG);

		tty.c_iflag &= ~(IXON | IXOFF | IXANY | IGNBRK | BRKINT | PARMRK | 
			ISTRIP | INLCR | IGNCR | ICRNL);

		tty.c_cc[VMIN] = 0;
		tty.c_cc[VTIME] = 10;

		cfsetispeed(&tty, B9600);
		cfsetospeed(&tty, B9600);

		if (tcsetattr(serial_device, TCSANOW, &tty) != 0) {
			return;
		}

		std::size_t read_count;

		char* read_buf = new char[1024];

		struct pollfd fds[1];
		fds[0].fd = serial_device;
		fds[0].events = POLLIN;

		int pollrc = 0;

		std::string message_buf = "";

		bool gotMessage = false;
		bool gotLine = false;

		#ifdef DEBUG
		std::ofstream _ostr1{"serial_gps_rx.log"};
		std::ofstream _ostr2{"serial_gps_out.log"};
		#endif

		while(_run){
			pollrc = poll(fds, 1, 1000);
			if(pollrc == 1){
				read_count = read(serial_device, read_buf, 1024);
				if(read_count == 0){
					// std::cout << "Read nothing" << std::endl;
					// continue;
				}else{
					// std::cout << "Read: " << read_buf << std::endl;
					for(std::size_t i = 0; i < read_count; i++){
						#ifdef DEBUG
						_ostr1 << read_buf[i] << std::flush;
						#endif
						if(read_buf[i] == '{'){
							gotMessage = true;
							message_buf += read_buf[i];
							read_buf[i] = 0;
						}else if(read_buf[i] == '}'){
							gotMessage = false;
							gotLine = true;
							message_buf += read_buf[i];
							read_buf[i] = 0;
						}else if(gotMessage){
							message_buf += read_buf[i];
							read_buf[i] = 0;
						}


						if(gotLine){
							Location& packet = SerialGPS::parseLocation(message_buf);
							if(packet.ltime != 0 &&
								packet.gtime != 0 &&
								packet.lon != 0 &&
								packet.lat != 0){
								std::unique_lock<std::mutex> guard{o_m};
								o_q.push(&packet);
								guard.unlock();
								o_v.notify_all();
								#ifdef DEBUG
								std::cout << "Pushed point at  " << packet.ltime << std::endl;
								_ostr2 << "Got packet at " << packet.ltime << std::endl;
								#endif
							}
							message_buf = "";
							gotLine = false;
						}
					}
				}
			}else{
				// std::cout << "Nothing to read" << std::endl;
			}
		}

		#ifdef DEBUG
		_ostr1.close();
		_ostr2.close();
		#endif
		close(serial_device);
	}
}