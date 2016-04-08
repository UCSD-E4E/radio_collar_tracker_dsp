bin_PROGRAMS = sdr_record
sdr_record_SOURCES = sdr_record.c queue.c queue.h
sdr_record_LDFLAGS=-pthread
sdr_record_LDADD=$(rtlsdr_LIBS) -lrt
sdr_record_CPPFLAGS=-std=gnu99 -Wall $(rtlsdr_CFLAGS)
