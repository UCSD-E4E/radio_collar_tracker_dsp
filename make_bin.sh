if [ -e "bin" ]
	then
	echo ""
	else
		mkdir bin
	fi
cd CurrentCode/PostProcessC/
chmod +x build*
./build-finalAnalysis
./build-spectrumAnalysis
cd ../../collarDetect
make all
cd ../bin
cp ../collarDetect/addNoiseCh .
cp ../collarDetect/altFilter.py .
cp ../CurrentCode/PostProcessC/COL .
cp ../CurrentCode/PostProcessC/finalAnalysis .
cp ../collarDetect/spectraCollarID .
cp ../CurrentCode/PostProcessC/spectrumAnalysis .
cp ../CLI_GUI/runcli.sh .
tar -cf run.tar addNoiseCh altFilter.py COL finalAnalysis spectraCollarID spectrumAnalysis run2.sh runcli.sh run.sh
cd ..
