#include <Arduino.h>
#include <stdio.h>
#include "LED.hpp"

#define SLEEP_TIME 100
LED blue, red, orange, yellow, green;

inline void blink(uint8_t pin, unsigned long t = SLEEP_TIME){
	digitalWrite(pin, HIGH);
	delay(t);
	digitalWrite(pin, LOW);
}

void setup(){
	Serial.begin(9600); // via USB
	Serial1.begin(9600); // GPS
	// Set up LEDs
	blue.pin = 4;
	red.pin = 12;
	orange.pin = 6;
	yellow.pin = 8;
	green.pin = 9;
	pinMode(green.pin, OUTPUT);
	pinMode(yellow.pin, OUTPUT);
	pinMode(orange.pin, OUTPUT);
	pinMode(red.pin, OUTPUT);
	pinMode(blue.pin, OUTPUT);
	
	
	blink(green.pin);
	blink(yellow.pin);
	blink(orange.pin);
	blink(red.pin);
	blink(blue.pin);
}

int state = LOW;

void loop(){
	Serial.print("{\"lat\": 327054113, \"hdg\":270, \"lon\": -1171710165,"
		" \"tme\": ");
	Serial.print(170655 + (int)(millis() / 1e3));
	Serial.print(", \"run\": true, \"fix\": 1, \"sat\": 14, \"dat\":"
		" 280419}");
	blink(blue.pin, 5);
	delay(1000);
}