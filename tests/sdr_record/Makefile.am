check_PROGRAMS = testDSPV2 testIQdata testUtility testPingFIR testMixer \
	testResampler testPingClassifier testProcessor testSDRTest testGPStest
	# testRemixer
	# testLocalization testRemixer

TESTS = $(check_PROGRAMS)

AM_CXXFLAGS=-std=gnu++14 -Wall $(BOOST_CPPFLAGS) -I../../src/sdr_record/
# AM_CXXFLAGS=-std=gnu++14 -Wall $(BOOST_CPPFLAGS) -I../../src/sdr_record/ $(DLIB_CPPFLAGS)
LDADD= -lstdc++ $(BOOST_THREAD_LIB) $(BOOST_SYSTEM_LIB)
# LDADD=-lfftw3_threads -lfftw3 -lstdc++ $(BOOST_THREAD_LIB) $(BOOST_SYSTEM_LIB)
AM_LDFLAGS=-pthread $(BOOST_LDFLAGS) 
# AM_LDFLAGS=-pthread $(BOOST_LDFLAGS) $(DLIB_LDFLAGS)

test_src = ../../src/sdr_record

testDSPV2_SOURCES = testDSPV2.cpp $(test_src)/dspv2.cpp \
	$(test_src)/iq_data.cpp $(test_src)/ping_fir.cpp $(test_src)/utility.cpp \
	$(test_src)/ping_classifier.cpp $(test_src)/Processor.cpp \
	$(test_src)/remixer.cpp $(test_src)/sdr_test.cpp $(test_src)/sdr.cpp

testDSPV2_LDADD = $(uhd_LIBS)

testIQdata_SOURCES = testIQdata.cpp $(test_src)/iq_data.cpp

testUtility_SOURCES = testUtility.cpp $(test_src)/utility.cpp

testPingFIR_SOURCES = testPingFIR.cpp $(test_src)/ping_fir.cpp \
	$(test_src)/utility.cpp

testMixer_SOURCES = testMixer.cpp $(test_src)/mixer.cpp $(test_src)/utility.cpp

testResampler_SOURCES = testResampler.cpp $(test_src)/resampler.cpp

testPingClassifier_SOURCES = testPingClassifier.cpp \
	$(test_src)/ping_classifier.cpp

testProcessor_SOURCES = testProcessor.cpp $(test_src)/utility.cpp \
	$(test_src)/ping_classifier.cpp $(test_src)/ping_fir.cpp \
	$(test_src)/Processor.cpp $(test_src)/remixer.cpp

testGPStest_SOURCES = testGPStest.cpp $(test_src)/gps_test.cpp \
	$(test_src)/gps.cpp $(test_src)/timeblock.cpp

maintainer-clean-local:
	rm Makefile.in
testSDRTest_LDADD=$(uhd_LIBS)
testSDRTest_SOURCES = testSDRTest.cpp $(test_src)/sdr_test.cpp \
	$(test_src)/iq_data.cpp $(test_src)/sdr.cpp 

# testLocalization_SOURCES = testLocalization.cpp $(test_src)/localization.cpp \
# 	$(test_src)/sdr_test.cpp $(test_src)/iq_data.cpp $(test_src)/sdr.cpp  \
# 	$(test_src)/dspv2.cpp $(test_src)/mixer.cpp $(test_src)/resampler.cpp \
# 	$(test_src)/ping_fir.cpp $(test_src)/utility.cpp \
# 	$(test_src)/ping_classifier.cpp $(test_src)/Processor.cpp \
# 	$(test_src)/gps_test.cpp $(test_src)/gps.cpp $(test_src)/timeblock.cpp \
# 	$(test_src)/remixer.cpp

testLocalization_LDADD = $(uhd_LIBS)

testRemixer_SOURCES = testRemixer.cpp $(test_src)/remixer.cpp \
	$(test_src)/utility.cpp