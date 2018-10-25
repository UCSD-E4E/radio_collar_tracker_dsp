AM_CXXFLAGS=-std=gnu++11 -Wall $(BOOST_CPPFLAGS)
AM_LDFLAGS=-pthread $(BOOST_LDFLAGS)
LDADD=-lstdc++ $(BOOST_THREAD_LIB) $(BOOST_SYSTEM_LIB)

bin_PROGRAMS = sdr_record 
sdr_record_SOURCES = sdr_record.cpp dspv2.cpp localization.cpp sdr.cpp \
	iq_data.cpp ping_classifier.cpp ping_fir.cpp resampler.cpp Processor.cpp \
	mixer.cpp utility.cpp dspv1.cpp sdr_test.cpp gps.cpp gps_test.cpp \
	timeblock.cpp

sdr_record_LDADD=$(uhd_LIBS)
sdr_record_CXXFLAGS= -std=gnu++11 -Wall $(BOOST_CPPFLAGS) $(uhd_CFLAGS)



maintainer-clean-local:
	rm Makefile.in
