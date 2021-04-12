#include <systemc.h>
#ifndef __FFT_H__
#define __FFT_H__

#define W {\
	{1, 0}, \
	{0.7071067812, -0.7071067812}, \
	{0, -1}, \
	{-0.7071067812, -0.7071067812}\
}

typedef struct complex_s complex_t;

struct complex_s {
	double real;
	double imag;
};

void fft(complex_t in[8], complex_t out[8]);

#endif
