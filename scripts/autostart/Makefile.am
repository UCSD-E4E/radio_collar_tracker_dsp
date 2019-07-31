bin_SCRIPTS = rctstart rctrun rct_udp_command.py
CLEANFILES = $(bin_SCRIPTS)
sysconf_DATA=rct_config

rctstart: $(srcdir)/rctstart.sh
	rm -f $@
	echo "#!" $(BASH) > $@
	$(SED) -e "s|&INSTALL_PREFIX|\'$(prefix)\'|g" $^ >> $@
	chmod +x $@

install-exec-hook: rctstart 10-rct_store.rules
	cp $(srcdir)/rctstart /etc/init.d/
	update-rc.d rctstart defaults 98 02
	sed -i '/LABEL=RCT_DATA \/mnt\/RAW_DATA exfat/d' /etc/fstab
	echo 'LABEL=RCT_DATA /mnt/RAW_DATA exfat dmask=000,fmask=0111,user,auto,nofail 0 0' >> /etc/fstab

uninstall-hook:
	update-rc.d rctstart remove
	rm /etc/init.d/rctstart
	sed -i '/LABEL=RCT_DATA \/mnt\/RAW_DATA exfat/d' /etc/fstab

maintainer-clean-local:
	rm Makefile.in

rctrun: $(srcdir)/rctrun.py
	rm -f $@
	$(PYTHON) -m py_compile $^
	$(SED) -e "s|&INSTALL_PREFIX|$(prefix)|g" $^ >> $@
	chmod +x $@

rct_udp_command.py: $(srcdir)/udp_command.py
	rm -f $@
	$(PYTHON) -m py_compile $^
	$(SED) -e "s|&INSTALL_PREFIX|$(prefix)|g" $^ >> $@
	chmod +x $@