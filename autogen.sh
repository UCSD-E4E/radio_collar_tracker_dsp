#!/bin/sh
aclocal -I m4 --install
autoheader
automake --gnu --add-missing
autoconf
