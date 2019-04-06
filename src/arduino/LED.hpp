#ifndef __LED__
#define __LED__
enum LEDState {
	OFF = 0,
	SLOW = 1,
	FAST = 2,
	ON = 3
};
typedef struct LED {
	int pin;
	volatile LEDState ledstate;
} LED;

#endif
