# radio_collar_tracker_drone
Airborne Wildlife Radio Collar Tracker - UAS Component

Engineers for Exploration, UCSD Project

# Setting up the Up Core
## Required Materials
1. Integrated Payload
2. Wall Power Adapter
3. HDMI Monitor
4. USB Hub
5. USB Keyboard
7. Linux Computer
8. Ubuntu 18.04 ISO
9. AVR Dragon ISP
10. USB Drive
## Procedure
1. Load Ubuntu ISO onto USB drive from the Linux computer
2. Plug the USB Hub into the USB port on the UIB.
3. Plug the USB Keyboard and USB Drive into the USB Hub.
4. Plug the HDMI Monitor into the UP Core.
5. Power on the system using the Wall Power Adapter
6. When prompted, enter the UP Core BIOS using the `ESC` key.
7. Ensure the boot priority is set to the USB drive.
8. Continue to boot from the USB drive.
9. Select the option to ...
10. Reboot into Ubuntu
11. Open a terminal using `Ctrl-Alt-t`
12. Run the following commands:
	1. `sudo apt-get update`
	2. `sudo apt-get upgrade -y`
	3. `sudo apt-get install python3 python3-pip libboost-all-dev python-mako libusb-1.0-0-dev cmake build-essential git`
	4. 

# Installing the payload software
1.	Install the required dependencies
	1.	python3
		1.	`apt-get install python3`
	2.  boost
		1.	`apt-get install libboost-all-dev`
	3.  pythom-mako
		1.	`apt-get install python-mako`
	4.  six
		1.	`pip install six`
	5.  requests
		1.	`pip install requests`
	7.  enum
		1.	`pip install enum`
	8.  pyserial
		1.  `pip3 install pyserial`
	5.	libusb-dev
		1.	`apt-get install libusb-1.0-0-dev`
	9.	libuhd 3.11.01
		1.	`apt-get install cmake build-essential`
		2.	`git clone git://github.com/EttusResearch/uhd.git`
		3.	`cd <uhd_repo>/host`
		4.	`git checkout v3.11.0.1`
		5.	`mkdir build`
		6.	`cd build`
		7.	`cmake -DENABLE_B100=OFF -DENABLE_X300=OFF -DENABLE_N230=OFF -DENABLE_USRP1=OFF -DENABLE_USRP2=OFF -DENABLE_OCTOCLOCK=OFF -DENABLE_RFNOC=OFF -DENABLE_MPMD=OFF -DENABLE_EXAMPLES=OFF -DENABLE_MANUAL=OFF -DENABLE_TESTS=OFF ../`
		8.	`make`
		9.	`make install`
		10.	`ldconfig`
		11.	`/usr/local/lib/uhd/utils/uhd_images_downloader.py -t b2xx*`
	10.	fftw
		1.	`wget http://www.fftw.org/fftw-3.3.8.tar.gz`
		2.	`tar -xzf fftw-3.3.8.tar.gz`
		3.	`cd <fftw>`
		4.	`./bootstrap.sh && ./configure --enable-threads --enable-generic-simd128 --enable-generic-simd256`
		5.	`make`
		6.	`make install`
2.	Install the software
	1.	`cd <radio_collar_tracker_drone>`
	2.	`git checkout online_proc`
	2.  `./autogen.sh`
	3.  `./configure`
	4.	`make`
	5.	`sudo make install`

# Compiling `sdr_record`
1. Dependencies
	1. `apt-get install libboost-all-dev libusb-1.0-0-dev cmake build-essential python-mako python-pip automake python3-pip git`
	2. `pip2 install six requests enum`
	3. `pip3 install mako numpy`
	3. libuhd
		2.	`git clone git://github.com/EttusResearch/uhd.git`
		3.	`cd <uhd_repo>/host`
		4.	`git checkout v3.11.0.1`
		5.	`mkdir build`
		6.	`cd build`
		7.	`cmake -DENABLE_B100=OFF -DENABLE_X300=OFF -DENABLE_N230=OFF -DENABLE_USRP1=OFF -DENABLE_USRP2=OFF -DENABLE_OCTOCLOCK=OFF -DENABLE_RFNOC=OFF -DENABLE_MPMD=OFF -DENABLE_EXAMPLES=OFF -DENABLE_MANUAL=OFF -DENABLE_TESTS=OFF ../`
		8.	`make`
		9.	`make install`
		10.	`ldconfig`
		11.	`/usr/local/lib/uhd/utils/uhd_images_downloader.py -t b2xx*`
	4.	fftw
		1.	`wget http://www.fftw.org/fftw-3.3.8.tar.gz`
		2.	`tar -xzf fftw-3.3.8.tar.gz`
		3.	`cd <fftw>`
		4.	`./bootstrap.sh && ./configure --enable-threads -enable-generic-simd128 --enable-generic-simd256`
		5.	`make`
		6.	`make install`
2. Compile
	1.	`cd <radio_collar_tracker_drone>`
	2.	`git v5.0a`
	2.  `./autogen.sh`
	3.  `./configure`
	4.	`make`

# Installing `sdr_record`
1.	`sudo make install`


# Using the Docker image
The docker image exists to provide a containerized development environment for
those that might be doing additional work with GnuRadio and/or UHD.  It also
provides a testing container to evaluate architectural changes to the payload
environment.

To build the docker image, use `docker build -t rct .`.

To run the docker image with hardware support, use `docker run -it --rm --name rct --device=/dev/bus/usb/ rct`.

For debugging and running unit tests, use `docker run -it --rm --name rct -v radio_collar_tracker_dsp/:/root/code -v testData/:/tmp/testData --privileged rct`.  Replace `radio_collar_tracker_dsp` with the absolute path to the local code
repository and `testData` with the absolute path to the test data repository.

## tl;dr
1.	`sudo apt-get update`
2.	`sudo apt-get install -y git vim htop gdb valgrind cmake build-essential python3 libboost-all-dev python-mako python3-pip libusb-1.0-0-dev autoconf pkg-config picocom python-pip zip wget tmux`
3.	`sudo pip2 install six requests`
4.	`sudo pip3 install pyserial`
5.	`git clone git://github.com/EttusResearch/uhd.git`
6.	`cd uhd`
7.	`git checkout v3.11.0.1`
8.	`mkdir build`
9.	`cd build`
10.	`cmake -DENABLE_B100=OFF -DENABLE_X300=OFF -DENABLE_N230=OFF -DENABLE_USRP1=OFF -DENABLE_USRP2=OFF -DENABLE_OCTOCLOCK=OFF -DENABLE_RFNOC=OFF -DENABLE_MPMD=OFF -DENABLE_EXAMPLES=OFF -DENABLE_MANUAL=OFF -DENABLE_TESTS=OFF ../`
11.	`make -j8`
12.	`sudo make install`
13.	`sudo ldconfig`
14.	`sudo /usr/local/lib/uhd/utils/uhd_images_downloader.py -t b2xx*`
15.	`cd ../..`
16.	`wget http://www.fftw.org/fftw-3.3.8.tar.gz`
17.	`tar -xzf fftw-3.3.8.tar.gz`
18.	`cd fftw-3.3.8`
19.	`./boostrap.sh`
20.	`./configure --enable-threads --enable-generic-simd128 --enable-generic-simd256`
21.	`make -j8`
22.	`sudo make install`
23.	`cd ../`
24.	`git clone git://github.com/UCSD-E4E/radio_collar_tracker_drone.git`
25.	`cd radio_collar_tracker_drone`
26.	`git checkout online_proc`
27.	`./autogen.sh`
28.	`./configure`
29.	`make -j8`
30.	`sudo make install`

# Running the payload software (standalone)
`sudo service rctrun start`

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
Output data location is specified in `/usr/local/etc/rct_config`.  Each run
consists of a number of IQ (raw) data files, GPS data files, and a metadata
file.  The name format for each type of file is `[type]_[run]_[file_num]`.  
The `type` field specifies, in capital letters, the type of file (i.e. RAW_DATA,
GPS, or META).  The `run` field specifies a numerical identifier for each run,
assigned sequentially. This field is always 6 characters wide, zero padded.  The
`file_num` field specifies the ordering of the IQ data files.  The first file
recorded would have a file number of 1, sequential files having sequential
numbers.  This field is always 6 characters wide, zero padded.

The raw data files contain the raw IQ data recorded as pairs of 8-bit unsigned
integers.  Each pair represents the in-phase and quadrature components of the
recorded signal in sequence.  See
https://en.wikipedia.org/wiki/In-phase_and_quadrature_components for an
explanation of IQ signal representation.

The GPS data files contain timestamped snapshots of autopilot telemetry.

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

# Information for Developers
## Connecting to the payload
1.	Connect to the `ubnt` GROUND WIFI LINK.
2.	Look for the sticker with the device hostname on the payload.
3.	SSH in using `e4e@hostname.local`
