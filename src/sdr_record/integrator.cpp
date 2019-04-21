#include "integrator.hpp"
#include "utility.hpp"
#include <vector>
#include <complex>

// #define DEBUG

#ifdef DEBUG
#include <fstream>
#include <iostream>
#endif

namespace RTT{
	Integrator::Integrator(const std::size_t factor) : _input_cv(nullptr), 
		_thread(nullptr), _decimation(factor){
	}
	Integrator::~Integrator(){
	}
	void Integrator::start(std::queue<TaggedSignal*>& i_q, std::mutex& i_m,
		std::condition_variable& i_c, std::queue<TaggedSignal*>& o_q,
		std::mutex& o_m, std::condition_variable& o_c){
		_input_cv = &i_c;
		_run = true;
		_thread = new std::thread(&Integrator::_process, this, std::ref(i_q),
			std::ref(i_m), std::ref(i_c), std::ref(o_q), std::ref(o_m), 
			std::ref(o_c));
	}
	void Integrator::stop(){
		_input_cv->notify_all();
		_run = false;
		_thread->join();
		delete _thread;
	}
	void Integrator::_process(std::queue<TaggedSignal*>& input_queue, 
		std::mutex& input_mutex, std::condition_variable& input_cv, 
		std::queue<TaggedSignal*>& output_queue, std::mutex& output_mutex, 
		std::condition_variable& output_cv){
		#ifdef DEBUG
		std::ofstream _ostr1{"int_in.log"};
		std::ofstream _ostr2{"int_out.log"};
		std::ofstream _ostr4{"int_sigs_in.log"};
		std::ofstream _ostr3{"int_sigs_out.log"};
		#endif

		// Local vars
		std::size_t count = 0;
		std::size_t out_count = 0;
		std::size_t data_idx = 0;
		TaggedSignal* in_sig;
		double acc = 0;
		auto sig_acc = new std::vector<std::complex<double>>();
		sig_acc->reserve(_decimation);

		// Main loop
		while(_run || !input_queue.empty()){
			// get lock
			std::unique_lock<std::mutex> in_lock(input_mutex);

			// get data
			if(input_queue.empty()){
				#ifdef DEBUG
				std::cout << "Integrator is waiting" << std::endl;
				#endif
				input_cv.wait(in_lock);
				#ifdef DEBUG
				std::cout << "Integrator woke up" << std::endl;
				#endif
			}
			if(input_queue.size() >= 1){
				#ifdef DEBUG
				std::cout << "Integrator has data!" << std::endl;
				#endif
				for(;data_idx < _decimation && !input_queue.empty(); data_idx++){
					
					in_sig = input_queue.front();
					#ifdef DEBUG
					_ostr1 << input_queue.front()->val << std::endl;
					std::vector<std::complex<double>>& sig = *input_queue.front()->sig;
					for (std::size_t i = 0; i < sig.size(); i++){
						_ostr4 << sig[i].real();
						if(sig[i].imag() >= 0){
							_ostr4 << "+";
						}	
						_ostr4 << sig[i].imag() << "i" << ",";
					}
					_ostr4 << std::endl;
					#endif
					input_queue.pop();
					acc += in_sig->val;
					sig_acc->insert(sig_acc->end(), in_sig->sig->begin(), in_sig->sig->end());
					delete in_sig->sig;
					delete in_sig;
					count++;

				}
			}
			if(data_idx == _decimation){
				// accumulated full buffer!
				TaggedSignal* out_sig = new TaggedSignal(log10(acc), *sig_acc);
				std::unique_lock<std::mutex> out_lock(output_mutex);
				output_queue.push(out_sig);
				out_lock.unlock();
				output_cv.notify_all();

				#ifdef DEBUG
				_ostr2 << log10(acc) << std::endl;
				std::vector<std::complex<double>>& sig = *sig_acc;
				for (std::size_t i = 0; i < _decimation; i++){
					_ostr3 << sig[i].real();
					if(sig[i].imag() >= 0){
						_ostr3 << "+";
					}
					_ostr3 << sig[i].imag() << "i" << ",";
				}
				_ostr3 << std::endl;
				#endif

				sig_acc = new std::vector<std::complex<double>>();
				sig_acc->reserve(_decimation);

				acc = 0;
				data_idx = 0;
				out_count++;
			}
		}
		#ifdef DEBUG
		std::cout << "Integrator out!" << std::endl;
		_ostr4.close();
		_ostr3.close();
		_ostr2.close();
		_ostr1.close();
		#endif
		delete sig_acc;
	}
}