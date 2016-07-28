EXTRA_DIST = blinky.sh mount_RAW_DATA.sh parser.sh rctstart.sh run.sh rctrun.sh
bin_SCRIPTS = rct_blinky rct_mount_RAW_DATA rct_parser rctstart rctrun
CLEANFILES = $(bin_SCRIPTS)
rct_blinky: $(srcdir)/blinky.sh
	rm -f $@
	echo "#!" $(BASH) > $@
	cat $^ >> $@
	chmod +x $@

rct_mount_RAW_DATA: $(srcdir)/mount_RAW_DATA.sh
	rm -f $@
	echo "#!" $(BASH) > $@
	$(SED) -e "s|&INSTALL_PREFIX|\'$(prefix)\'|g" $^ >> $@
	chmod +x $@

rct_parser: $(srcdir)/parser.sh
	rm -f $@
	echo "#!" $(BASH) > $@
	$(SED) -e "s|&INSTALL_PREFIX|\'$(prefix)\'|g" $^ >> $@
	chmod +x $@

rctstart: $(srcdir)/rctstart.sh
	rm -f $@
	echo "#!" $(BASH) > $@
	$(SED) -e "s|&INSTALL_PREFIX|\'$(prefix)\'|g" $^ >> $@
	chmod +x $@

install-exec-hook: rctstart
	-mkdir /media/RAW_DATA
	cp $(srcdir)/rct_mount_RAW_DATA /etc/init.d/
	update-rc.d rct_mount_RAW_DATA defaults 97 03
	service rct_mount_RAW_DATA start
	cp $(srcdir)/rctstart /etc/init.d/
	update-rc.d rctstart defaults 98 02

uninstall-hook:
	update-rc.d rctstart remove
	rm /etc/init.d/rctstart
	service rct_mount_RAW_DATA stop
	update-rc.d rct_mount_RAW_DATA remove
	rm /etc/init.d/rct_mount_RAW_DATA
	rmdir /media/RAW_DATA

maintainer-clean-local:
	rm Makefile.in

rctrun: $(srcdir)/rctrun.sh
	rm -f $@
	echo "#!" $(BASH) > $@
	$(SED) -e "s|&INSTALL_PREFIX|\'$(prefix)\'|g" $^ >> $@
	chmod +x $@
