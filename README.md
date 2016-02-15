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
	3.	rtl-sdr
		1.	`git clone https://github.com/steve-m/librtlsdr`
		2.	`mkdir librtlsdr/build`
		3.	`cd librtlsddr/build`
		4.	`sudo cmake ../ -DINSTALL_UDEV_RULES=ON`
		5.	`make`
		6.	`sudo make install`
		7.	`ldconfig`
2.	Install the software
	1.	`cd <radio_collar_tracker_drone>`
	2.	`make`
	3.	`sudo make install`

Running the payload software (standalone)
=========================================
`<radio_collar_tracker_drone>/autostart/rctstart`

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
