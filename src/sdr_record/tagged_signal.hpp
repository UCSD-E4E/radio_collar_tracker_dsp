#ifndef __TAGGED_SIGNAL__
#define __TAGGED_SIGNAL__

#include <vector>
#include <complex>
namespace RTT{
	/**
	 * Represents a sequence of complex doubles tagged with a particular value.
	 */
	struct TaggedSignal{
	private:
	protected:

	public:
		/**
		 * Creates a TaggedSignal object with the specific tag and signal.  This
		 * object will only store a pointer to the signal - the signal must be
		 * allocated on the heap.  The user is responsible for freeing the
		 * signal object at the end of use.
		 *
		 * @param	value	Value to tag the signal with.
		 * @param	signal	Reference to a signal represented as a vector of
		 *               	complex doubles.
		 */
		TaggedSignal(double value, std::vector<std::complex<double>>& signal) :
			val(value), sig(&signal){};

		/**
		 * Tag value
		 */
		double val;

		/**
		 * Signal pointer
		 */
		std::vector<std::complex<double>>* sig;
	};
}

#endif