#include <systemc.h>
#ifndef __FFT_H__
#define __FFT_H__
#define L 23

//La structure pour les données d'entrée
typedef struct  {
	sc_int<L> real;
	sc_int<L> imag;
} complex_in;
//La structure pour les données de sortie
typedef struct  {
	sc_int<L+4> real;
	sc_int<L+4> imag;
} complex_out;

void fft(complex_in in[8], complex_out out[8]);

#endif
