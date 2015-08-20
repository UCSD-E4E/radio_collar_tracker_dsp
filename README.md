radio_collar_tracker
====================
Airborne Wildlife Radio Collar Tracker

Engineers for Exploration, UCSD Project


Running the Post-Process Code
=============================
1.	Make the make_bin.sh file executable by running `chmod +x make_bin.sh`
2.	Move the file `bin/run.tar` to a working directory of your choice.
3.	Extract the binaries from `run.tar` by running `tar -xf run.tar`
4.	Run the post-process code using any of the run scripts.
	1.	`run.sh` needs to have the raw data from the SD card in the same working
		directory.  Usage: `run.sh NUM_COLLARS ALT_AGL`
	2.	`run2.sh` takes an additional argument for where the raw data is.
		Usage: `run2.sh NUM_COLLARS ALT_AGL DATA_DIR`
	3.	`runcli.sh` is an interactive shell script.  Usage: `runcli.sh`
5.	Note: if you run the PostProcessC code without using the integration
	scripts, ensure that all paths are fixed paths.

Installing sdr_record
=====================
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
2.	Make sdr_record
	1.	`cd <radio_collar_tracker>`
	2.	`cd sdr_record`
	3.	`make`

Running sdr_record
==================
usage: sdr_starter.sh [-r <runNum>] [-f <center_frequency>] [-g <sdr_gain>]
[-o <output_directory>] [-s <sampling_frequency>] [-p <autopilot_serial_port>]
