EXTRA_DIST = gps_keep_alive.py gps_logger.py
bin_SCRIPTS = rct_gps_keep_alive.py rct_gps_logger.py
CLEANFILES = $(bin_SCRIPTS)
rct_gps_keep_alive.py: $(srcdir)/gps_keep_alive.py
	rm -f $@
	echo "#!" $(PYTHON) > $@
	cat $^ >> $@
	chmod +x $@

rct_gps_logger.py: $(srcdir)/gps_logger.py
	rm -f $@
	echo "#!" $(PYTHON) > $@
	cat $^ >> $@
	chmod +x $@

maintainer-clean-local:
	rm Makefile.in
