## Process this file with automake to produce Makefile.in
###############################################################################
##     Radio Collar Tracker Paylod Software
##     Copyright (C) 2020  Nathan Hui
##
##     This program is free software: you can redistribute it and/or modify
##     it under the terms of the GNU General Public License as published by
##     the Free Software Foundation, either version 3 of the License, or
##     (at your option) any later version.
##
##     This program is distributed in the hope that it will be useful,
##     but WITHOUT ANY WARRANTY; without even the implied warranty of
##     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##    GNU General Public License for more details.
##
##     You should have received a copy of the GNU General Public License
##     along with this program.  If not, see <http://www.gnu.org/licenses/>.
##
###############################################################################
##
## DATE        Name  Description
## -----------------------------------------------------------------------------
## 04/15/20    NH    Initial history, updated sed
##
###############################################################################

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
	$(SED) -i '/LABEL=RCT_DATA \/mnt\/RAW_DATA exfat/d' /etc/fstab
	echo 'LABEL=RCT_DATA /mnt/RAW_DATA exfat dmask=000,fmask=0111,user,auto,nofail 0 0' >> /etc/fstab

uninstall-hook:
	update-rc.d rctstart remove
	rm /etc/init.d/rctstart
	$(SED) -i '/LABEL=RCT_DATA \/mnt\/RAW_DATA exfat/d' /etc/fstab

maintainer-clean-local:
	rm Makefile.in
	-rm *.pyc

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