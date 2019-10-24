check_PROGRAMS = testIQdata testUtility \
	testSDRTest testGPStest \
	testDSPV3 \
	testSerialGPS

TESTS = $(check_PROGRAMS)

# AM_CXXFLAGS=-std=gnu++14 -Wall $(BOOST_CPPFLAGS) -I../../src/sdr_record/
AM_CXXFLAGS=-std=gnu++14 -Wall $(BOOST_CPPFLAGS) -I../../src/sdr_record/ $(DLIB_CPPFLAGS)
# LDADD=-lstdc++ $(BOOST_THREAD_LIB) $(BOOST_SYSTEM_LIB)
LDADD=-lfftw3_threads -lfftw3 -lstdc++ $(BOOST_THREAD_LIB) $(BOOST_SYSTEM_LIB)
# AM_LDFLAGS=-pthread $(BOOST_LDFLAGS) 
AM_LDFLAGS=-pthread $(BOOST_LDFLAGS) $(DLIB_LDFLAGS)

test_src = ../../src/sdr_record

testIQdata_SOURCES = testIQdata.cpp $(test_src)/iq_data.cpp

testUtility_SOURCES = testUtility.cpp $(test_src)/utility.cpp

testGPStest_SOURCES = testGPStest.cpp $(test_src)/gps_test.cpp \
	$(test_src)/gps.cpp $(test_src)/timeblock.cpp $(test_src)/serial_gps.cpp

maintainer-clean-local:
	rm Makefile.in
clean-local: clean-local-check

.PHONY: clean-local-check

clean-local-check:
	-rm -f *.log*
	
testSDRTest_LDADD=$(uhd_LIBS)
testSDRTest_SOURCES = testSDRTest.cpp $(test_src)/sdr_test.cpp \
	$(test_src)/iq_data.cpp $(test_src)/sdr.cpp 

testLocalization_SOURCES = testLocalization.cpp $(test_src)/localization.cpp \
	$(test_src)/sdr_test.cpp $(test_src)/iq_data.cpp \
	$(test_src)/dspv3.cpp $(test_src)/utility.cpp \
	$(test_src)/gps_test.cpp $(test_src)/gps.cpp $(test_src)/timeblock.cpp \
	$(test_src)/integrator.cpp \
	$(test_src)/serial_gps.cpp

testIntegrator_SOURCES = testIntegrator.cpp $(test_src)/integrator.cpp \
	$(test_src)/utility.cpp

testDSPV3_SOURCES = testDSPV3.cpp $(test_src)/dspv3.cpp \
	$(test_src)/integrator.cpp $(test_src)/utility.cpp \
	$(test_src)/iq_data.cpp

testSerialGPS_SOURCES = testSerialGPS.cpp $(test_src)/serial_gps.cpp