#include <concepts>
#include <iterator>
#include <iostream>
#include <vector>
#include <boost/circular_buffer.hpp>

template <std::floating_point T>
class fir_filter{
public:
  int N; // dims
  std::vector<T> Params; // params x N
  boost::circular_buffer<T> Register; // z^-n+1

  // Constructor
  fir_filter(std::vector<T> init): Params(init) {
    N = init.size();
    Register.set_capacity(N-1);
  }

  explicit fir_filter(std::initializer_list<T> init): Params(init.begin(), init.end()) {
    N = init.size();
    Register.set_capacity(N-1);
  }

  T  // その時点での応答を取得し、タイムステップを進める
  step(T input){
    T y = Params[0] * input; // h_0 * x(n)
    for(int n=0; n<N;++n){
      y += Params[n+1] * Register[n]; // h_i * z^-(i-1)
    }
    Register.push_front(input);
    return y;
  }

  void
  printParameterList() const{
    std::cout << "+++++++++++++Parameters List+++++++++++++" << std::endl;
    for (int i = 0;i < N;++i){
      std::cout << i << ":" << Params[i];
      if(i%2)
	std::cout << std::endl;
      else
	std::cout << "\t";
    }
  }
  
  void   // impulseもはや人智を超えた板倉
  impulseResponse(){
    std::cout << std::endl << "+++++++++++++Impulse Response+++++++++++++" << std::endl;
    std::cout << "0:" << this->step(99999999.f) << std::endl;
    for(int n = 1;n < N;++n){
      std::cout << n << ':' << this->step(0);
      if(n%2)
	std::cout << std::endl;
      else
	std::cout << "\t";
    }
  }
};
