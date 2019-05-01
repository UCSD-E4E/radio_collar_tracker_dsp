#ifndef __LED__
#define __LED__
enum LEDState {
	OFF = 0,
	SLOW = 1,
	FAST = 2,
	ON = 3
};
typedef struct LED {
	uint8_t pin;
	volatile LEDState ledstate;
	LED(){
		ledstate = OFF;
		pin = -1;
	}
} LED;

#endif
