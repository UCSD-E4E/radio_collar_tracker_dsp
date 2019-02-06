# Dockerfile for RCT

# Usage: docker build -t rct .
# docker run -t -d --rm --name rct --device=/dev/bus/usb/ rct
# docker exec -it rct /bin/bash
# docker container stop rct
FROM ubuntu:16.04
RUN apt-get update && apt-get install -y git vim htop gdb valgrind cmake \
	build-essential python libboost-all-dev python-mako python-pip \
	libusb-1.0-0-dev autoconf pkg-config
RUN pip install --upgrade pip
RUN pip install six requests pynmea2 serial enum
#  exfat-fuse exfat-utils autoconf

RUN git clone git://github.com/EttusResearch/uhd.git /root/uhd
WORKDIR /root/uhd/
RUN git checkout v3.11.0.1
RUN mkdir /root/uhd/host/build
WORKDIR /root/uhd/host/build
# RUN cmake ../
RUN cmake -DENABLE_B100=FALSE -DENABLE_X300=FALSE -DENABLE_E320=FALSE \
	-DENABLE_N320=FALSE -DENABLE_N300=FALSE -DENABLE_N230=FALSE \
	-DENABLE_USRP1=FALSE -DENABLE_USRP2=FALSE -DENABLE_OCTOCLOCK=FALSE \
	-DENABLE_RFNOC=FALSE -DENABLE_MPMD=FALSE ../
RUN make -j7
RUN make install
RUN ldconfig

# RUN apt-get install -y 
# RUN add-apt-repository -y ppa:ettusresearch/uhd
# RUN apt-get update
# RUN export DEBIAN_FRONTEND=noninteractive
# RUN apt-get install -y libuhd-dev libuhd003 uhd-host
# RUN ln -fs /usr/share/zoneinfo/America/Los_Angeles /etc/localtime
# RUN dpkg-reconfigure --frontend noninteractive tzdata

RUN /usr/local/lib/uhd/utils/uhd_images_downloader.py -t b2xx*

RUN git clone git://github.com/davisking/dlib.git /root/dlib

RUN git clone git://github.com/UCSD-E4E/radio_collar_tracker_drone.git /root/radio_collar_tracker_drone
WORKDIR /root/radio_collar_tracker_drone
RUN git checkout online_proc
RUN ./autogen.sh
RUN ./configure DLIB_INCLUDEDIR=/root/dlib
RUN make
