bin_PROGRAMS = sdr_record
sdr_record_SOURCES = sdr_record.c fifo.c
sdr_record_LDFLAGS=-pthread
sdr_record_LDADD=$(uhd_LIBS) -lrt
sdr_record_CPPFLAGS=-std=gnu99 -Wall $(uhd_CFLAGS)

maintainer-clean-local:
	rm Makefile.in
