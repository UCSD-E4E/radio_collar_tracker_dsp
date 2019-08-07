# Dockerfile for RCT

# Usage: docker build -t rct .
# docker run -it --rm --name rct --device=/dev/bus/usb/ rct

# For debugging, use:
# docker run -it --rm --name rct -v /home/ntlhui/workspace/radio_collar_tracker_drone/:/root/code -v /home/ntlhui/workspace/tmp/testData/:/home/ntlhui/workspace/tmp/testData --privileged rct
FROM ubuntu:16.04
RUN apt-get update && apt-get install -y git vim htop gdb valgrind cmake \
	build-essential python3 libboost-all-dev python-mako python3-pip \
	libusb-1.0-0-dev autoconf pkg-config picocom sudo python-pip zip
RUN pip3 install --upgrade pip
RUN pip2 install --upgrade pip
RUN pip2 install requests
RUN pip2 install six requests pynmea2 pyserial
RUN pip3 install pyserial
#  exfat-fuse exfat-utils autoconf

RUN git clone git://github.com/EttusResearch/uhd.git /root/uhd
WORKDIR /root/uhd/
RUN git checkout v3.11.0.1
RUN mkdir /root/uhd/host/build
WORKDIR /root/uhd/host/build
# RUN cmake ../
RUN cmake -DENABLE_B100=OFF -DENABLE_X300=OFF \
	-DENABLE_N230=OFF \
	-DENABLE_USRP1=OFF -DENABLE_USRP2=OFF -DENABLE_OCTOCLOCK=OFF \
	-DENABLE_RFNOC=OFF -DENABLE_MPMD=OFF -DENABLE_EXAMPLES=OFF \
	-DENABLE_MANUAL=OFF -DENABLE_TESTS=OFF ../
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

WORKDIR /root/
RUN apt-get update && apt-get install -y wget
RUN wget http://www.fftw.org/fftw-3.3.8.tar.gz
RUN tar -xzf fftw-3.3.8.tar.gz
WORKDIR /root/fftw-3.3.8
RUN ./bootstrap.sh && ./configure --enable-threads --enable-generic-simd128 --enable-generic-simd256 && make -j7 && make install

RUN git clone git://github.com/UCSD-E4E/radio_collar_tracker_drone.git /root/radio_collar_tracker_drone
WORKDIR /root/radio_collar_tracker_drone
RUN git checkout online_proc
RUN ./autogen.sh
RUN ./configure
# RUN make -j7
# RUN make install

RUN mkdir /host-dev/

RUN mkdir /root/code

WORKDIR /root/code/

RUN apt-get install -y tmux