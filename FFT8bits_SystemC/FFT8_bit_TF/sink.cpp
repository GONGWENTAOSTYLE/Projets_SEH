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

    while(true){

        if(real.num_available() > 0 && imagine.num_available() > 0){
            tmp_real = real.read();
            tmp_imag = imagine.read();
            file << tmp_real << " " << tmp_imag << endl;
            cout << "writing" << endl;
        }
        wait();
    }
}
