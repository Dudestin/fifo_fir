#include <iterator>
#include <iostream>
#include <fstream>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <boost/circular_buffer.hpp>
#include <vector>
#include <limits>
#include <cstdio>

#define INPUT_FIFO "/tmp/fir_fifoin"
#define OUTPUT_FIFO "/tmp/fir_fifoout"

class fir_filter{
public:
  int N; // dims
  std::vector<float> Params; // params x N
  boost::circular_buffer<float> Register; // z^-n+1

  // Constructor
  fir_filter(std::vector<float> init): Params(init) {
    N = init.size();
    Register.set_capacity(N-1);
  }
  explicit fir_filter(std::initializer_list<float> init): Params(init.begin(), init.end()) {
    N = init.size();
    Register.set_capacity(N-1);
  }

  float  // その時点での応答を取得し、タイムステップを進める
  step(float input){
    float y = Params[0] * input; // h_0 * x(n)
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

int
main(int argc,char** argv){
  int inputFd,dummyFd,outputFd;
  umask(0);
  
  if(mkfifo(INPUT_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1
     && errno != EEXIST)
    fprintf(stderr,"mkdifo %s", INPUT_FIFO);
  inputFd = open(INPUT_FIFO, O_RDONLY);
  if(inputFd == -1){
    fprintf(stderr,"open %s", INPUT_FIFO);
  }

  dummyFd = open(INPUT_FIFO, O_WRONLY);
  if(dummyFd == -1){
    fprintf(stderr,"open %s", INPUT_FIFO);
  }

  if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
    fprintf(stderr,"signal");
  FILE* fp = fdopen(inputFd, "r");

  // Start Initialize Procedure
  // Initialize Phase 1: get Parameter "N"
  int param_N;
  if(read(inputFd, &param_N, sizeof(int)) // Need Binary Format!!!
     != sizeof(int)){
    fprintf(stderr, "Error reading request[Phase 1: Parameter N(int)]; discarding\n");
    exit(-1);
  }
  std::cout << "N:" <<param_N << std::endl;
  
  // Initialize Phase 2: get Parameters "h_i"
  float* param_tmp = new float[param_N];
  if(fread(param_tmp, sizeof(float), param_N, fp) < 0){
    fprintf(stderr, "Error reading request[Phase 2: Parameter h_i(float[N])]; discarding\n");
    exit(-1);
  }

  fir_filter filter {std::vector<float>(param_tmp, param_tmp + param_N)};
  delete[](param_tmp);

  // Await data & process them
  float x_n;
  for(int i = 0;;++i){
    if(read(inputFd, &x_n, sizeof(float)) // Need Binary Format!!!
       != sizeof(float)){
      fprintf(stderr, "Error occurred on proceeding request[x_n(float)]; discarding\n");
      exit(-1);
    }
    std::cout << i << ": " << filter.step(x_n) << std::endl;
    continue;
  }

  fclose(fp);
  return 0;
}

