radio_collar_tracker_drone
====================
Airborne Wildlife Radio Collar Tracker - UAS Component

Engineers for Exploration, UCSD Project

Installing the payload software
===============================
1.	Install the required dependencies
	1.	python
	2.	pymavlink
		1.	`pip install pymavlink`
	3.  pyserial
		1.  `sudo apt-get install python-serial`
	<!-- 3.	rtl-sdr
		1.	`git clone https://github.com/steve-m/librtlsdr`
		2.	`mkdir librtlsdr/build`
		3.	`cd librtlsddr/build`
		4.	`sudo cmake ../ -DINSTALL_UDEV_RULES=ON -DDETACH_KERNEL_DRIVER=ON`
		5.	`make`
		6.	`sudo make install`
		7.	`ldconfig` -->
	4.	libuhd
		1.	`sudo apt-get install libboost-all-dev libusb-1.0-0-dev python-mako doxygen python-docutils cmake build-essential`
		2.	`git clone git://github.com/EttusResearch/uhd.git`
		3.	`cd uhd/host`
		4.	`mkdir build`
		5.	`cd build`
		6.	`cmake ../`
		7.	`make`
		8.	`sudo make install`
		9.	`sudo ldconfig`
2.	Install the software
	1.	`cd <radio_collar_tracker_drone>`
	2.  `./autogen.sh`
	3.  `./configure`
	4.	`make`
	5.	`sudo make install`

Running the payload software (standalone)
=========================================
`rctstart`

#Running the payload software (hardware-based initialization)
##Payload Power On Procedure
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
