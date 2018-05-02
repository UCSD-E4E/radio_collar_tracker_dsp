bin_PROGRAMS = sdr_record
sdr_record_SOURCES = sdr_record.cpp dspv1.cpp localization.cpp sdr.cpp
sdr_record_LDFLAGS=-pthread
sdr_record_LDADD=$(uhd_LIBS) -lstdc++
sdr_record_CXXFLAGS=-std=c++11 -Wall $(uhd_CFLAGS)

maintainer-clean-local:
	rm Makefile.in
