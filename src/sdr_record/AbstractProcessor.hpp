#ifndef __ABSTRACT_PROCESSOR_H__
#define __ABSTRACT_PROCESSOR_H__

namespace RTT{
	class AbstractProcessor{
	public:
		virtual AbstractProcessor(std::size_t frequency);
		virtual double process(std::complex<double> data[]);
	}
}

#endif