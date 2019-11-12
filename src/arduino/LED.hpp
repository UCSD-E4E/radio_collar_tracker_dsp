#ifndef __LED__
#define __LED__
/*! \file */
/**
 * Permissible LED states.
 */
enum LEDState {
	OFF = 0,
	SLOW = 1,
	FAST = 2,
	ON = 3
};
typedef struct LED {
	/**
	 * Arduino pin assigned to this LED.
	 */
	uint8_t pin;

	/**
	 * Current desired state of this LED.
	 */
	volatile LEDState ledstate;

	/**
	 * Default constructor.
	 */
	LED(){
		ledstate = OFF;
		pin = -1;
	}
} LED;

#endif
