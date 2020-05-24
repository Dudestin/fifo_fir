#include <iostream>
#include <fstream>
using namespace std;

int main(){
    ofstream fout;
    fout.open("params.bin", ios::out|ios::binary|ios::trunc);

    if (!fout) {
        cout << "ファイル params.bin が開けません";
        return -1;
    }

    int N = 31;
    fout.write((char*)&N,sizeof(int));

    std::initializer_list<float> params {
    -0.031945116,
      4.01E-16,
      -0.026041749,
      -5.16E-16,
      -0.037311807,
      4.58E-16,
      -0.053100017,
      -2.87E-16,
      -0.076676152,
      2.29E-16,
      -0.116859934,
      -1.72E-16,
      -0.205803118,
      1.43E-16,
      -0.63446077,
      0,
      0.63446077,
      -1.43E-16,
      0.205803118,
      1.72E-16,
      0.116859934,
      -2.29E-16,
      0.076676152,
      2.87E-16,
      0.053100017,
      -4.58E-16,
      0.037311807,
      5.16E-16,
      0.026041749,
      -4.01E-16,
      0.031945116,
    };

    for(const auto x: params){
      fout.write((char*)&x,sizeof(float));
    }
    fout.close();
    return 0;
}
