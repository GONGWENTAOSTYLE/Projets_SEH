#include "source.h"
#include <fstream>
#include <iostream>
using std:: cout;
using std:: endl;

void Source::comportement(){

    std::ifstream file;
    file.open("input_samples.txt");

    float tmp_real, tmp_imag;
    if(!file.is_open())
        cerr<<"The file is not opened"<<endl;
    wait();

    while(true){
        if(!file.eof()){
            if (real.num_free()>0 && imagine.num_free()>0){
                file >> tmp_real >> tmp_imag;
                real.write(tmp_real);
                imagine.write(tmp_imag);
                cout << "reading" << endl;
            }
            else{
                cout << "No space available" << endl;
            }
        }
        else{
            cout << "End of File" << endl;
        }
        wait();
    }

    //file.close("input_samples.txt");

}
