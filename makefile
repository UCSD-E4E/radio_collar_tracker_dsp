.PHONY: PI_install PI_uninstall all BBB_install BBB_uninstall

all:
	$(MAKE) -C sdr_record

PI_install: sdr_record/sdr_record autostart/rctstart gps_logger/gps_logger.py autostart/parser.sh getRunNum.py sdr_starter.sh
	cp autostart/rctstart /etc/init.d/
	update-rc.d rctstart defaults

PI_uninstall:
	-update-rc.d rctstart remove
	-rm /etc/init.d/rctstart

BBB_install: sdr_record/sdr_record autostart/rctstart gps_logger/gps_logger.py autostart/parser.sh getRunNum.py
	cp autostart/mount_RAW_DATA /etc/init.d/
	update-rc.d mount_RAW_DATA defaults
	cp autostart/rctstart /etc/init.d/
	update-rc.d rctstart defaults
	-mkdir /media/RAW_DATA/
	
BBB_uninstall:
	-update-rc.d rctstart remove
	-update-rc.d mount_RAW_DATA remove
	-rm /etc/init.d/rctstart
	-rm /etc/init.d/mount_RAW_DATA
	-umount /dev/mmcblk0p1
	-rmdir /media/RAW_DATA
