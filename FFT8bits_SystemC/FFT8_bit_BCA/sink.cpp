#include "sink.h"
#include <fstream>
#include <iostream>
using std:: cout;
using std:: endl;

void Sink::comportement(){

    std::ofstream file;
    file.open("output.txt");

    float tmp_real, tmp_imag;
    if(!file)
		cout<<"Can not write the output"<<endl;
    wait();
    data_req = false;

    while(true){

        if(data_req && data_valid){
            data_req = false;
            tmp_real = real.read();
            tmp_imag = imagine.read();
            file << tmp_real << " " << tmp_imag << endl;
            cout << "writing" << endl;
        }
        else data_req = true;
        wait();
    }
}
