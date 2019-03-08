# docker build -t rct_avr .
# docker run -it --rm --name rct_avr rct_avr

FROM ubuntu:16.04

ARG dev_tools="git vim"
ARG build_env="build-essential gcc-avr binutils-avr avr-libc gdb-avr avrdude libelf-dev"

RUN apt-get update && apt-get install -y $dev_tools $build_env

RUN mkdir /code
WORKDIR /code
RUN git clone https://github.com/UCSD-E4E/radio_collar_tracker_drone.git
WORKDIR /code/radio_collar_tracker_drone
RUN git checkout ui_board
RUN git submodule init
RUN git submodule update
WORKDIR /code/radio_collar_tracker_drone/src/arduino
RUN make ui_core.hex