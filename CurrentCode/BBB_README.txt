// ---------------------------------------
//	BBB Radio Recorder.
// ---------------------------------------

// ---------------------------------------
// Runtime Instructions:
The contents of this folder must be inside
the user folder on theBBB:
~/
To start the recorder trigger, start ./main under the gpio folder
// ---------------------------------------

// ---------------------------------------
// Compiling/building Instructions:

// Updating:
sudo apt-get update
sudo apt-get upgrade
sudo apt-get dist-upgrade
sudo apt-get install build-essential
sudo reboot

// Installing GTK+2:
sudo apt-get install gtk+-2.0
sudo apt-get install libgtk2.0-dev
sudo apt-get install libcairo2-dev

// Making sure there wont be DVB problems
sudo su 	
echo blacklist dvb_usb_rtl28xxu > /etc/modprobe.d/rtlsdr.conf
exit

// Cmake Installation
sudo apt-get install git cmake

// LibUSB Installation 
sudo apt-get install libusb-1.0-0-dev

// Download and build rtl-sdr:
sudo reboot
git clone https://github.com/steve-m/librtlsdr
cd ~/librtlsdr
mkdir build
cd build
cmake ../ -DINSTALL_UDEV_RULES=ON
make
sudo make install
sudo ldconfig

// Testing rtl device (Reboot may be necessary):
sudo rtl_test -t

// If it works, there will be a message like this:
Found 1 device(s):
  0:  Realtek, RTL2838UHIDIR, SN: 00000001
Using device 0: Generic RTL2832U OEM
Found Rafael Micro R820T tuner
Supported gain values (29): 0.0 0.9 1.4 2.7 3.7 7.7 8.7 12.5 14.4 15.7 16.6 19.7 20.7 22.9 25.4 28.0 29.7 32.8 33.8 36.4 37.2 38.6 40.2 42.1 43.4 43.9 44.5 48.0 49.6 
Sampling at 2048000 S/s.
No E4000 tuner found, aborting.

// Setting  the SDcard to automount at startup:
sudo mkdir /media/RAW_DATA
sudo nano /etc/rc.local
// Add this line to the file:
sudo mount /dev/mmcblk0p1 /media/RAW_DATA -o dmask=000,fmask=111
// Check if it automounts:
sudo reboot
cd /media
ls

// Copy the code from BBB to ~/ :
sudo cp -r gpio ~/
sudo cp -r xcode ~/
// Change permission:
sudo chmod -R 775 gpio
sudo chmod -R 775 xcode

// Remove Python to free space:
sudo apt-get remove python
sudo apt-get clean
sudo apt-get autoclean

// Fix Clock to 1GHz:
sudo apt-get install cpufrequtils sysfsutils
sudo reboot
sudo nano /etc/init.d/cpufrequtils
GOVERNOR="performance"
sudo reboot
cpufreq-info

// Compile codes:
cd ~/gpio/
sudo ./build
cd ~/xcode/
sudo rm serialGPS.o
sudo make
sudo ./build

// Disable eth0 check:
sudo nano /etc/network/interfaces
// Make sure everything is commented out
// exept for:
auto lo
iface lo inet loopback
// ---------------------------------------