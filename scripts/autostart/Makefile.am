EXTRA_DIST = parser.sh rctstart.sh run.sh rctrun.sh
bin_SCRIPTS = rct_parser rctstart rctrun
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

install-exec-hook: rctstart
	cp $(srcdir)/rctstart /etc/init.d/
	update-rc.d rctstart defaults 98 02

uninstall-hook:
	update-rc.d rctstart remove
	rm /etc/init.d/rctstart

maintainer-clean-local:
	rm Makefile.in

rctrun: $(srcdir)/rctrun.sh
	rm -f $@
	echo "#!" $(BASH) > $@
	$(SED) -e "s|&INSTALL_PREFIX|\'$(prefix)\'|g" $^ >> $@
	chmod +x $@
