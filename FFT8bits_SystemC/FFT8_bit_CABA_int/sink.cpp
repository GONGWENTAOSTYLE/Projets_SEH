#include "sink.h"
#include <fstream>
#include <iostream>
using std:: cout;
using std:: endl;

//L’objectif est derécupérer l’ensemble des données dans le fichier output.txt.
void Sink::comportement(){

    std::ofstream file;
    file.open("output.txt");// Ouvrir le fichier input_samples.txt

    sc_int<L+4> tmp_real, tmp_imag;
     if(!file)
		cout<<"Can not write the output"<<endl;// Si le fichier n'est pas ouvert, retourne une erreur.
    wait();
    data_req = false;

    while(true){
        // S'il y a une demande de données et les donnéesdans le fifo sont disponibles alors on lit les données.
        if(data_req && data_valid){
            data_req = false;
            tmp_real = real.read();
            tmp_imag = imagine.read();
            file << tmp_real << " " << tmp_imag << endl;
            cout << "writing" << endl;
        }
        else data_req = true; // La demande est toujours valide
        wait();
    }
}
