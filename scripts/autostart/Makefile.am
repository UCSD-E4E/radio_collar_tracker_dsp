bin_SCRIPTS = rct_parser rctstart rctrun rct_mount_sd rct_umount_sd
CLEANFILES = $(bin_SCRIPTS)

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

install-exec-hook: rctstart 10-rct_store.rules
	cp $(srcdir)/rctstart /etc/init.d/
	update-rc.d rctstart defaults 98 02
	$(SED) -e "s|&INSTALL_PREFIX|$(prefix)|g" 10-rct_store.rules > /etc/udev/rules.d/10-rct_store.rules

uninstall-hook:
	update-rc.d rctstart remove
	rm /etc/init.d/rctstart
	rm /etc/udev/rules.d/10-rct_store.rules

maintainer-clean-local:
	rm Makefile.in

rctrun: $(srcdir)/rctrun.py
	rm -f $@
	echo "#!" $(PYTHON) > $@
	$(SED) -e "s|&INSTALL_PREFIX|$(prefix)|g" $^ >> $@
	chmod +x $@

rct_mount_sd: $(srcdir)/mount_sd.sh
	rm -rf $@
	echo "#!" $(BASH) > $@
	$(SED) -e "s|&INSTALL_PREFIX|\'$(prefix)\'|g" $^ >> $@
	chmod +x $@

rct_umount_sd: $(srcdir)/umount_sd.sh
	rm -rf $@
	echo "#!" $(BASH) > $@
	$(SED) -e "s|&INSTALL_PREFIX|\'$(prefix)\'|g" $^ >> $@
	chmod +x $@