#include "source.h"
#include <fstream>
#include <iostream>
using std:: cout;
using std:: endl;

//L’objectif est de lire le fichier d’entrée input_samples.txt
void Source::comportement(){

    std::ifstream file;
    file.open("input_samples.txt"); // Ouvrir le fichier input_samples.txt

    sc_int<L> tmp_real, tmp_imag;
    data_valid = false;

    if(!file.is_open())
        cerr<<"The file is not opened"<<endl; // Si le fichier n'est pas ouvert, retourne une erreur.
    wait();


    while(true){
        // S'il y a une demande de données et le fichier n'est pas vide, alors on écrit les donées dans le fifo.
        if (data_req) {
            if (!file.eof()){ 
                file >> tmp_real >> tmp_imag;
                real.write(tmp_real);
                imagine.write(tmp_imag);
                // Si les données sont disponibles, on les valide.
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
