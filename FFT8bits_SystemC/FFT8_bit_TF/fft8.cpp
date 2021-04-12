#include"fft.h"
#include"fft8.h"
#include<stdio.h>
/**/
void FFT8::comportement(){

  double tmp_real[8], tmp_imag[8];
  int i = 0;
  complex_t in[8],out[8];

  while(true){
	i = 0;
    while(i < 8){
        while(in_real.num_available()>0 && in_imag.num_available()>0){

            tmp_real[i] = in_real.read();
            tmp_imag[i] = in_imag.read();
            i++;
            if(i==8) break;
            cout << i << endl;
        }
        wait();
    }



	for(int j = 0; j<8; j++){
		in[j].real = tmp_real[j];
		in[j].imag = tmp_imag[j];
	}
	fft(in, out);
	for(int j = 0; j<8; j++){
		tmp_real[j] = out[j].real;
		tmp_imag[j] = out[j].imag;
	}

    i = 0;
    while(i < 8 ){
        while(out_real.num_free()>0 && out_imag.num_free()>0){

            out_real.write(tmp_real[i]);
            out_imag.write(tmp_imag[i]);
            i++;
            if(i==8) break;
        }
         wait();
    }

  }
}

complex_t weights[4] = W;

void but(complex_t *weight,
					complex_t *in0,
					complex_t *in1,
					complex_t *out0,
					complex_t *out1)
{
      out0->real = (in0->real + ((in1->real * weight->real) - (in1->imag * weight->imag)));
      out0->imag = (in0->imag + ((in1->real * weight->imag) + (in1->imag * weight->real)));
      out1->real = (in0->real - ((in1->real * weight->real) - (in1->imag * weight->imag)));
      out1->imag = (in0->imag - ((in1->real * weight->imag) + (in1->imag * weight->real)));
}


void fft(complex_t in[8], complex_t out[8])
{
	complex_t stage1[8], stage2[8];

	// First stage
	but(&weights[0], &in[0], &in[4], &stage1[0], &stage1[1]);
	but(&weights[0], &in[2], &in[6], &stage1[2], &stage1[3]);
	but(&weights[0], &in[1], &in[5], &stage1[4], &stage1[5]);
	but(&weights[0], &in[3], &in[7], &stage1[6], &stage1[7]);

	// Second stage
	but(&weights[0], &stage1[0], &stage1[2], &stage2[0], &stage2[2]);
	but(&weights[2], &stage1[1], &stage1[3], &stage2[1], &stage2[3]);
	but(&weights[0], &stage1[4], &stage1[6], &stage2[4], &stage2[6]);
	but(&weights[2], &stage1[5], &stage1[7], &stage2[5], &stage2[7]);

	// Etape 3
	but(&weights[0], &stage2[0], &stage2[4], &out[0], &out[4]);
	but(&weights[1], &stage2[1], &stage2[5], &out[1], &out[5]);
	but(&weights[2], &stage2[2], &stage2[6], &out[2], &out[6]);
	but(&weights[3], &stage2[3], &stage2[7], &out[3], &out[7]);
}
