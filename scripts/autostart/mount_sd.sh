#!/bin/bash
if [ ! -d "/mnt/RAW_DATA" ]; then
	mkdir /mnt/RAW_DATA
fi
mount /dev/sda1 /mnt/RAW_DATA
