Positive Collar Identification
==============================

One of the most important features of the radio collar tracking software is its ability to differentiate between noise and a radio collar.  Due to the physics of radio transmissions, there tends to be a very sharp falloff in signal power as one moves away from the collar.  That is, if the receiver is within a reasonable proximity to the transmitter, the signal to noise ratio should be a great deal better than 0 dB.

There are two parts to this feature.  The first is the addNoiseCh program.  This program analyzes the specified collar frequencies and selects a frequency to use as a noise baseline.  The way it does this is by selecting a frequency that is at least 2.5 kHz away from collar frequencies and roughly in the middle of the frequency band occupied by the collar frequencies.  This gives a rough approximation of the noise environment of the frequency band, which allows us to positively identify channels that actually have collars on them because the average signal strength will be much higher than the noise floor.

The second part of this feature is the spectraCollarID program.  This program analyzes the output from the spectrunAnalysis program and finalAnalysis program in order to identify collars and properly output their positions.