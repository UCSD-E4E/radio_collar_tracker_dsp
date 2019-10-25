SUBDIRS = src doc scripts tests

maintainer-clean-local:
	rm aclocal.m4 Makefile.in config.h.in configure depcomp install-sh missing
	-rm compile
	-rm config.h-in~ config.sub config.guess
	-find . -name '*.log' -delete
	-rm -rf oprofile_data
	-rm -rf src/sdr_record/run
	-rm -rf doxygen-doc

if HAVE_DOXYGEN
@DX_RULES@
endif