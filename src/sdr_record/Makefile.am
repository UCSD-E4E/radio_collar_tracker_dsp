AM_CXXFLAGS=-std=gnu++11 -Wall $(BOOST_CPPFLAGS) $(DLIB_CPPFLAGS)
AM_LDFLAGS=-pthread $(BOOST_LDFLAGS) $(DLIB_LDFLAGS)
LDADD=-lstdc++ $(BOOST_THREAD_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_PROGRAM_OPTIONS_LIB)

bin_PROGRAMS = sdr_record 
sdr_record_SOURCES = sdr_record.cpp localization.cpp sdr.cpp \
	iq_data.cpp \
	utility.cpp dspv1.cpp sdr_test.cpp gps.cpp gps_test.cpp \
	timeblock.cpp fir.cpp integrator.cpp classifier.cpp dspv3.cpp \
	serial_gps.cpp

sdr_record_LDADD=$(uhd_LIBS) $(DLIB_LDFLAGS) $(BOOST_LDFLAGS) $(BOOST_THREAD_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_PROGRAM_OPTIONS_LIB)
sdr_record_CXXFLAGS= -std=gnu++11 -Wall $(BOOST_CPPFLAGS) $(uhd_CFLAGS) $(DLIB_CPPFLAGS)



maintainer-clean-local:
	rm Makefile.in

clean-local: clean-local-check

.PHONY: clean-local-check

clean-local-check:
	-rm -f *.log*