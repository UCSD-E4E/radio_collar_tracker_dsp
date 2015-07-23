#!/bin/bash
# Remove extraneous packages
apt-get remove apache2 openbox xscreensaver mesa-common-dev aspell gksu gnome-keyring gsettings-desktop-schemas hicolor-icon-theme hunspell-en-us javascript-common leafpad lxde-common lxmenu-data lxpanel lxsession lxterminal whiptail --purge -y

# Update and upgrad existing software
apt-get update -y
apt-get upgrade -y
apt-get dist-upgrade -y
apt-get clean -y
apt-get autoremove -y

# Install dependencies
apt-get install build-essential -y
apt-get install cpufrequtils -y
apt-get install sysfsutils -y
apt-get install gtk+-2.0 -y
apt-get install libgtk2.0-dev -y
apt-get install libcairo2-dev -y
apt-get install git -y
apt-get install cmake -y
apt-get install libusb-1.0-0-dev -y
apt-get install lightdm -y

# Set CPU governor
mv /etc/init.d/cpufrequtils /etc/init.d/cpufrequtils.old
chmod -x /etc/init.d/cpufrequtils.old
cat /etc/init.d/cpufrequtils.old | sed 's_^GOVERNOR=\"[a-zA-Z]*\"$_GOVERNOR=\"performance\"_' > /etc/init.d/cpufrequtils

echo blacklist dvb_usb_rtl28xxu > /etc/modprobe.d/rtlsdr.conf

cd ~
git clone https://github.com/steve-m/librtlsdr
mkdir ~/librtlsdr/build
cd ~/librtlsdr/build
cmake ../ -DINSTALL_UDEV_RULES=ON
make
make install
ldconfig
cd ~
rm -rf ~/librtlsdr

mkdir /media/RAW_DATA

echo "sudo mount /dev/mmcblk0p1 /media/RAW_DATA -o dmask=000,fmask=111" >> /etc/rc.local

cd /home/debian
git clone https://github.com/UCSD-E4E/radio_collar_tracker
cp -r radio_collar_tracker/CurrentCode/BBB/gpio/ .
cp -r radio_collar_tracker/CurrentCode/BBB/xcode/ .
chmod +x gpio/build
cd gpio
./build
cd ../xcode
chmod +x build
rm *.o
make
chmod +x run

