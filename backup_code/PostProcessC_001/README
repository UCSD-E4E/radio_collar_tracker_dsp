// ---------------------------------------
//	Post-Process Analysis Software Folder
// ---------------------------------------

// ---------------------------------------
// Runtime Instructions:
Make sure the path to .../rct/ in the SD card
is on the first line of the JOB file and
that the configs in JOB are consistent
First run ./spectrumAnalysis
The partial results will be in RUN_*.csv
Then run ./finalAnalysis
The final results will be in META_*.csv
// ---------------------------------------

// ---------------------------------------
// Compiling/building Instructions:
Install and configure FFTW:
Download and unpack FFTW (aval at:)
http://www.fftw.org/fftw-3.3.4.tar.gz
cd ~/Downloads/fftw-3.3.4/
sudo chmod 775 configure
./configure --enable-threads
sudo apt-get remove make
sudo apt-get install make
sudo make
sudo make install
sudo ./build-finalAnalysis
sudo ./build-spectrumAnalysis
// ---------------------------------------