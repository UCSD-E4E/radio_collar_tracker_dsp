# Operation

The autostart module should accomplish the following:
* When the switch is moved to the "on" position, execute `rct_sdr_starter`.
* Monitor the state of `rct_sdr_starter` to ensure correct operation.
* When the switch is moved to the "off" position, send `SIGTERM` to `rct_sdr_starter`.
* Enable multiple runs without rebooting the system.

# Configuring autostart

If the file `/usr/local/etc/rct_autostart` exists, autostart will be enabled.

# Pinout:
5V0 -> 

1V8 ->

GND ->

GPIO1 ->

GPIO2 ->

GPIO3 ->

GPIO4 ->

GPIO5 ->

SW1 ->

# Structure
`rctstart.sh` 