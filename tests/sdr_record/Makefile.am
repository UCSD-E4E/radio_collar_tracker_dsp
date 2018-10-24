check_PROGRAMS = testDSPV2 testIQdata testUtility testPingFIR testMixer \
	testResampler testPingClassifier testProcessor testSDRTest

TESTS = $(check_PROGRAMS)

AM_CXXFLAGS=-std=gnu++14 -Wall $(BOOST_CPPFLAGS) -I../../src/sdr_record/
LDADD=-lstdc++ $(BOOST_THREAD_LIB) $(BOOST_SYSTEM_LIB)
AM_LDFLAGS=-pthread $(BOOST_LDFLAGS)

test_src = ../../src/sdr_record

testDSPV2_SOURCES = testDSPV2.cpp $(test_src)/dspv2.cpp \
	$(test_src)/iq_data.cpp $(test_src)/mixer.cpp $(test_src)/resampler.cpp \
	$(test_src)/ping_fir.cpp $(test_src)/utility.cpp \
	$(test_src)/ping_classifier.cpp $(test_src)/Processor.cpp

testIQdata_SOURCES = testIQdata.cpp $(test_src)/iq_data.cpp

testUtility_SOURCES = testUtility.cpp $(test_src)/utility.cpp

testPingFIR_SOURCES = testPingFIR.cpp $(test_src)/ping_fir.cpp \
	$(test_src)/utility.cpp

testMixer_SOURCES = testMixer.cpp $(test_src)/mixer.cpp $(test_src)/utility.cpp

testResampler_SOURCES = testResampler.cpp $(test_src)/resampler.cpp

testPingClassifier_SOURCES = testPingClassifier.cpp \
	$(test_src)/ping_classifier.cpp

testProcessor_SOURCES = testProcessor.cpp $(test_src)/mixer.cpp \
	$(test_src)/utility.cpp $(test_src)/ping_classifier.cpp \
	$(test_src)/resampler.cpp $(test_src)/ping_fir.cpp \
	$(test_src)/Processor.cpp

maintainer-clean-local:
	rm Makefile.in
testSDRTest_LDADD=$(uhd_LIBS)
testSDRTest_SOURCES = testSDRTest.cpp $(test_src)/sdr_test.cpp \
	$(test_src)/iq_data.cpp $(test_src)/sdr.cpp 