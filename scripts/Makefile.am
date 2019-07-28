EXTRA_DIST = sdr_starter.sh getRunNum.py
SUBDIRS = autostart gps_logger
bin_SCRIPTS = rct_sdr_starter rct_getRunNum.py
sysconf_DATA=rct_config

CLEANFILES = $(bin_SCRIPTS)
rct_sdr_starter: $(srcdir)/sdr_starter.sh
	rm -f $@
	echo "#!" $(BASH) > $@
	$(SED) -e "s|&INSTALL_PREFIX|\'$(prefix)\'|g" $^ >> $@
	chmod +x $@

rct_getRunNum.py: $(srcdir)/getRunNum.py
	rm -f $@
	echo "#! " $(PYTHON) > $@
	cat $^ >> $@
	chmod +x $@


maintainer-clean-local:
	rm Makefile.in
