radio_collar_tracker_drone
====================
Airborne Wildlife Radio Collar Tracker - UAS Component

Engineers for Exploration, UCSD Project

Installing the payload software
===============================
1.	Install the required dependencies
	1.	python
		1.	`apt-get install python`
	2.  boost
		1.	`apt-get install libboost-all-dev`
	3.  pythom-mako
		1.	`apt-get instlal python-mako`
	4.  six
		1.	`pip install six`
	5.  requests
		1.	`pip install requests`
	6.  pynmea2
		1.	`pip install pynmea2`
	7.  enum
		1.	`pip install enum`
	8.  pyserial
		1.  `apt-get install python-serial`
	9.	libuhd 3.11.01
		1.	`apt-get install libboost-all-dev libusb-1.0-0-dev python-mako`
		2.	`apt-get install cmake build-essential`
		2.	`git clone git://github.com/EttusResearch/uhd.git`
		3.	`cd <uhd_repo>/host`
		4.	`git checkout v3.11.0.1`
		4.	`mkdir build`
		5.	`cd build`
		6.	`cmake -DENABLE_B100=OFF -DENABLE_X300=OFF -DENABLE_N230=OFF -DENABLE_USRP1=OFF -DENABLE_USRP2=OFF -DENABLE_OCTOCLOCK=OFF -DENABLE_RFNOC=OFF -DENABLE_MPMD=OFF -DENABLE_EXAMPLES=OFF -DENABLE_MANUAL=OFF -DENABLE_TESTS=OFF ../`
		7.	`make`
		8.	`make install`
		9.	`ldconfig`
		10.	`/usr/local/lib/uhd/utils/uhd_images_downloader.py -t b2xx*`
	10.	dlib v19.16
		1.	`git clone git://github.com/davisking/dlib.git`
		2.	`cd <dlib_repo>/`
		3.	`git checkout v19.16`
2.	Install the software
	1.	`cd <radio_collar_tracker_drone>`
	2.  `./autogen.sh`
	3.  `./configure`
	4.	`make`
	5.	`sudo make install`
3.	Configure the USRP for the first time
	1.	`/usr/local/lib/uhd/utils/uhd_images_downloader.py`

tl;dr
-----
1.	`sudo add-apt-repository -y ppa:mraa/mraa`
2.	`sudo apt-get update`
3.	`sudo apt-get install -y python-serial libmraa1 libmraa-dev mraa-tools python-mraa python3-mraa libboost-all-dev libusb-1.0-0-dev python-mako doxygen python-docutils cmake build-essential exfat-fuse exfat-utils python-pip git autoconf`
4.	`sudo pip install pynmea2 enum34`
5.	`cd $HOME`
6.	`git clone git://github.com/EttusResearch/uhd.git`
7.	`cd uhd/host`
8.	`mkdir build`
9.	`cd build`
10.	`cmake ../`
11.	`make -j5`
12.	`sudo make install`
13.	`sudo ldconfig`
14.	`cd $HOME`
15.	`git clone git://github.com/UCSD-E4E/radio_collar_tracker_drone.git`
16.	`cd radio_collar_tracker_drone`
17.  `./autogen.sh`
18.  `./configure`
19.	`make -j5`
20.	`sudo make install`

Running the payload software (standalone)
=========================================
`rct_sdr_starter [-h] [-r run_num] [-f center_freq] [-s sample_rate] [-g gain] [-o output_dir]`

# Running the payload software (hardware-based initialization)

## Payload Power On Procedure
1.	Disconnect power to AUTOPILOT, PAYLOD
2.	Connect power to PAYLOAD
3.	Connect power to AUTOPILOT
4.	Flip PAYLOAD SWITCH to the ON position.
5.	Wait for the PAYLOAD STATUS LIGHT to turn green for at least 10 seconds.
6.	Flip PAYLOAD SWITCH to the OFF position.
7.	Wait for the PAYLOAD STATUS LIGHT to turn off within 5 seconds.
8.	Connect the GROUND CONTROL STATION to the AUTOPILOT.
9.	Payload Power On Procedure complete.

## Payload Start Procedure
1.	Complete the Payload Power On Procedure.
2.	Flip PAYLOAD SWITCH to the ON position.
3.	Wait for the PAYLOAD STATUS LIGHT to turn green for at least 10 seconds.
4.	Payload Start Procedure complete.

## Payload Stop Procedure
1.	Flip PAYLOAD SWITCH to the OFF position.
2.	Wait for the PAYLOAD STATUS LIGHT to turn off within 5 seconds.
3.	Payload Stop Procedure complete.

## Payload Power Off Procedure
1.	Complete the Payload Stop Procedure.
2.	Disconnect power to the PAYLOAD.
3.	Payload Power Off Procedure complete.

Output Data Format
==================
Output data is located in `/home/pi/rct/`.  Each run consists of a number of IQ
(raw) data files, GPS data files, and a metadata file.  The name format for each
type of file is `[type]_[run]_[file_num]`.  The `type` field specifies, in
capital letters, the type of file (i.e. RAW_DATA, GPS, or META).  The `run`
field specifies a numerical identifier for each run, assigned sequentially.
This field is always 6 characters wide, zero padded.  The `file_num` field
specifies the ordering of the IQ data files.  The first file recorded would have
a file number of 1, sequential files having sequential numbers.  This field is
always 6 characters wide, zero padded.

The raw data files contain the raw IQ data recorded as pairs of 8-bit unsigned
integers.  Each pair represents the in-phase and quadrature components of the
recorded signal in sequence.  See
https://en.wikipedia.org/wiki/In-phase_and_quadrature_components for an
explanation of IQ signal representation.  Each file contains at most 10485760
samples, or 5.12 seconds of data.  IQ data is recorded at 2.048 MSps.

The GPS data files contain timestamped snapshots of autopilot telemetry.  Fields
are comma separated, and are: local capture time (UTC in seconds), lat (degrees
\* 1e7), lon (degrees * 1e7), time since autopilot boot (ms), absolute altitude
(meters MSL), relative altitude (meters AGL), x axis velocity (m/s), y axis
velocity (m/s), z axis velocity (m/s), heading (degrees).  Note: I do not know
whether the heading is relative to true north or magnetic north.  The NED frame
ideally is relative to true north, but I don't know if the heading is reported
before or after declination offsets are taken into account.

The metadata file contains metadata pertaining to the configuration of the
payload.  Each line contains one field.  The key for each field is followed by a
colon, then the value for that field, followed by a newline character:
```
[field_name]: [field_value]
```
The whitespace between the colon and field value is optional, and preferred for
readability, but cannot be a newline.  The metadata file contains the following
fields: local IQ data start time (UTC in seconds), SDR center frequency (Hz),
SDR sampling frequency (Hz), SDR gain (dB).
