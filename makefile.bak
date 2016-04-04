.PHONY: PI_install PI_uninstall all clean sdr_record/sdr_record

all:
	$(MAKE) -C sdr_record all

install: sdr_record/sdr_record autostart/rctstart gps_logger/gps_logger.py autostart/parser.sh getRunNum.py sdr_starter.sh
	$(MAKE) -C sdr_record clean
	$(MAKE) -C sdr_record
	cp autostart/rctstart /etc/init.d/
	update-rc.d rctstart defaults 98 02

uninstall:
	-update-rc.d rctstart remove
	-rm /etc/init.d/rctstart

clean:
	$(MAKE) -C sdr_record clean

sdr_record/sdr_record:
	$(MAKE) -C sdr_record
