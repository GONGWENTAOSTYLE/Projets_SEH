#include "source.h"
#include <fstream>
#include <iostream>
using std:: cout;
using std:: endl;


void Source::comportement(){

    std::ifstream file;
    file.open("input_samples.txt");

    float tmp_real, tmp_imag;
    data_valid = false;

    if(!file.is_open())
        cerr<<"The file is not opened"<<endl;
    wait();

    while(true){
        if (data_req) {
            if (!file.eof()){
                file >> tmp_real >> tmp_imag;
                real.write(tmp_real);
                imagine.write(tmp_imag);
                data_valid = true;
                cout << "reading" << endl;
            }
            else  {
                data_valid = false;
            }
        }
        wait();
    }

}
