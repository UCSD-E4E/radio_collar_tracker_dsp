#!/bin/bash
if [ ! -d "/mnt/RAW_DATA" ]; then
	mkdir /mnt/RAW_DATA
fi
mount /dev/mmcblk0p1 /mnt/RAW_DATA
