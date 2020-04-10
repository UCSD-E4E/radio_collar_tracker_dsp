# SDR Record Application
## Command Line Options
| Flag					| Required	| Args 		| Description									|
| --------------------- | :-------:	| --------- | --------------------------------------------- |
| -h, --help			| 			| None		| Displays the help text						|
| -g, --gain			|     X		| double	| Sets the SDR LNA gain in dB					|
| -s, --sampling_freq	|     		| int		| Sets the SDR sampling frequency (Hz)			|
| -c, --center_freq		|     		| int		| Sets the SDR center frequency (Hz)			|
| -r, --run				|     X		| int		| Sets the sdr_record run number				|
| -o, --output_dir		|			| string	| Sets the sdr_record output directory			|
| -v, --verbose			|			| int		| Sets the verbosity level (1-8)				|
| --test_config			|			| None		| Sets sdr_record into test mode				|
| --test_data			|			| string	| Specifies the test data path					|
| --config_file			|			| string	| Specifies the configuration file to override	|

## Configuration File Options
| Flag				| Args		| Description								|
| ----------------- | --------- | ----------------------------------------- |
| gps_target		| string	| Path to GPS serial port					|
| gps_baud			| int		| GPS serial port baud rate					|
| gps_mode			| bool		| GPS Test mode								|
| ping_width_ms		| int		| Nominal ping width (ms)					|
| ping_max_len_mult	| double	| Maximum acceptable ping width multiplier	|
| ping_min_len_mult	| double	| Minimum acceptable ping width multiplier	|
| ping_min_snr		| double	| Minimum acceptable ping SNR (dB?)			|
| sampling_freq		| int		| SDR sampling frequency (Hz)				|
| center_freq		| int		| SDR center frequency (Hz)					|
| output_dir		| string	| sdr_record output directory				|
| autostart			| bool		| Autostart flag							|
| frequencies		| int		| Target frequencies (Hz)					|

## Sample Configuration File
```
# Length of the ping in milliseconds.
ping_width_ms=27

# Minimum permissible SNR.  Pings with an amplitude less than this will
# be rejected.
ping_min_snr=0.1

# Maximum and minimum permissible ping lengths.  Pings will only be
# accepted if their duration is in the range [ping_min_len_mult *
# ping_width_ms, ping_max_len_mult * ping_width_ms].
ping_max_len_mult=1.5
ping_min_len_mult=0.5

# GPS Testing mode.  Set this to true to disable use of GPS.  In this
# mode, GPS data will always be 0, 0.
gps_mode=false
                                                                     
# GPS Device handle.  This device must send JSON packets of GPS data.
gps_target=/dev/ttyACM0
gps_baud=9600

# Transmitter Frequencies.  Enter each transmitter frequency on a new
# line.  For example:
#     frequencies=173500000
#     frequencies=173600000
frequencies=173965000

# Autostart flag.  Set to "true" to enable autostart, otherwise, set to "false"
autostart=true

# Output directory.  This should be the root of the removable storage.
output_dir='/mnt/RAW_DATA'

# Sampling Frequency in Hz
sampling_freq=1500000

# Center Frequency in Hz
center_freq = 173500000
```

## Common Configurations
### Running old data
#### Configuration File:
```
# Length of the ping in milliseconds.
ping_width_ms=27

# Minimum permissible SNR.  Pings with an amplitude less than this will
# be rejected.
ping_min_snr=0.1

# Maximum and minimum permissible ping lengths.  Pings will only be
# accepted if their duration is in the range [ping_min_len_mult *
# ping_width_ms, ping_max_len_mult * ping_width_ms].
ping_max_len_mult=1.5
ping_min_len_mult=0.5

# GPS Testing mode.  Set this to true to disable use of GPS.  In this
# mode, GPS data will always be 0, 0.
gps_mode=false
                                                                     
# GPS Device handle.  This device must send JSON packets of GPS data.
gps_target=/dev/ttyACM0
gps_baud=9600

# Transmitter Frequencies.  Enter each transmitter frequency on a new
# line.  For example:
#     frequencies=173500000
#     frequencies=173600000
frequencies=172951000
frequencies=172051000

# Autostart flag.  Set to "true" to enable autostart, otherwise, set to "false"
autostart=true

# Output directory.  This should be the root of the removable storage.
output_dir='/tmp/RAW_DATA'

# Sampling Frequency in Hz
sampling_freq=1500000

# Center Frequency in Hz
center_freq = 173500000
```

Note: Only the frequencies and ping parameters are relevant.  `sdr_record` will
ignore `autostart`, `output_dir`.  `sampling_freq` and `center_freq` still need
to be provided.

#### Command Line Args
```
sdr_record --test_config --test_data /tmp/RUN_000014 -c 172500000 -s 2000000
```

Note that the center frequency and sampling frequencies are provided here - this
is required if the sampling frequency and/or center frequency is not correct in
the configuration file.

#### Dataset

The path passed to `sdr_record` as `sdr_record --test_config --test_data .../data_dir` should have the following files at a minimum:

```
.../data_dir/
		|
		ALT
		GPS_000001
		META_000001
		RAW_DATA_000001_000001
		RAW_DATA_000001_000002
		.
		.
		.
		RAW_DATA_000001_XXXXXX
		RUN

```