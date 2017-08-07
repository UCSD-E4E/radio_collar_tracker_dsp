# Operation

The autostart module should accomplish the following:
* When the switch is moved to the "on" position, execute `rct_sdr_starter`.
* Monitor the state of `rct_sdr_starter` to ensure correct operation.
* When the switch is moved to the "off" position, send `SIGTERM` to `rct_sdr_starter`.
* Enable multiple runs without rebooting the system.

# Configuring autostart

If the file `/usr/local/etc/rct_autostart` exists, autostart will be enabled.

# Pinout:
5V0 -> J13-4

1V8 -> J13-10

GND -> J13-9

GPIO1 -> J13-11

GPIO2 -> J13-13

GPIO3 -> J13-15

GPIO4 -> J13-17

GPIO5 -> J13-19

SW1 -> J13-21

# Structure
`rctstart` is the Linux Standard Base init script that starts the `rctrun` service at boot.

`rctrun` is the state machine that manages initializing and starting all devices.

`rct_blink` is the LED mapping program that drives the LEDs.

## rctstart

`rctstart` is the init script that runs at boot.  At current, this only starts and stops the `rctrun` process.  It uses the `/var/lock/rctstart` lock to ensure that only one instance of `rctrun` exists.  The source for this is `rctstart.sh`

## rctrun

`rctrun` is the state machine that manages the initialization and starting of all devices and recording.  There are several state machines for each of the GPS, SDR, and output directory.  These initialize and sanity check the devices to ensure that the next run will operate properly.  `rctrun` will automatically launch and stop the necessary programs.

`rct_blink` is the program that drives the LEDs based on the state of `rctrun`.  State information is stored in `/var/local/rct.dat`.