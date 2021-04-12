#include"fft.h"
#include"fft8.h"
#include<stdio.h>
/**/
void FFT8::comportement(){

  sc_int<L> tmp_real_in[8], tmp_imag_in[8];
  sc_int<L+4> tmp_real_out[8], tmp_imag_out[8];
  int i = 0;
  complex_in in[8];
  complex_out out[8];

  while(true){
	i = 0;
	out_req = false;
	out_valid = false;
  //chaque fois on manipule 8 bits de donnée.
    while(i < 8){
        if(in_valid && out_req){
            tmp_real_in[i] = in_real.read();
            tmp_imag_in[i] = in_imag.read();
            i++;
            cout << i << endl;
			      out_req = false;
        }
        else out_req = true;
		wait();
    }
  // On met les données lues dans le tableau in.
	for(int j = 0; j<8; j++){
		in[j].real = tmp_real_in[j];
		in[j].imag = tmp_imag_in[j];
	}
  // L'opération FFT
	fft(in, out);
  
	for(int j = 0; j<8; j++){
		tmp_real_out[j] = out[j].real;
		tmp_imag_out[j] = out[j].imag;
    cerr << tmp_real_out[j] << " " << tmp_imag_out[j] << endl;
	}
  // On transmettre les données obtenues à un fifo de sortie
    i = 0;
    while(i < 8 ){      
      if(!out_valid){

              out_real.write(tmp_real_out[i]);
              out_imag.write(tmp_imag_out[i]);
              i++;
              out_valid = true;
          }
      else  if(in_req) out_valid = false;
          wait();
      }
  }
}
// Le template du papillon
template <unsigned int N> void papillon(  sc_int<N> Wr, sc_int<N> Wi, 
                sc_int<N> ar, sc_int<N> ai,
                sc_int<N> br, sc_int<N> bi,
                sc_int<N+2> &new_ar, sc_int<N+2> &new_ai,
                sc_int<N+2> &new_br, sc_int<N+2> &new_bi)
// le papillon sur 2 echantillons complexes peut ramener l'energie
// des 2 echantillons sur une seule composante (reelle ou imaginaire)
// d'un seul echantillon en sortie,
// donc si entree sur N bits, sortie sur N+2
// Mais si l'on cascade S etages, si entree sur N bits,
// sortie sur N+S+1 bits (2^S entrees combinent au pire toute leur
// energie sur une unique composante d'un echantillon de sortie, 
// donc il faudra recadrer!
    {
      sc_int<2*N> mult_brr;
      sc_int<2*N> mult_bri;
      sc_int<2*N> mult_bir;
      sc_int<2*N> mult_bii;
      
      sc_int<N> mult_brr_trunc;
      sc_int<N> mult_bri_trunc;
      sc_int<N> mult_bir_trunc;
      sc_int<N> mult_bii_trunc;
      
      mult_brr = (br * Wr);
      mult_bri = (br * Wi);
      mult_bir  = (bi * Wr);
      mult_bii  = (bi * Wi);

      mult_brr_trunc =  mult_brr.range(2*N-2,N-1)+mult_brr[N-2];

      // car les W sont codes sur 1 bit de signe et N-1 bits pour la partie fractionnaire
      // et ont rajoute le bit de precision pour l'arrondi
      mult_bri_trunc =  mult_bri.range(2*N-2,N-1)+mult_bri[N-2];
      mult_bir_trunc =  mult_bir.range(2*N-2,N-1)+mult_bir[N-2];
      mult_bii_trunc =  mult_bii.range(2*N-2,N-1)+mult_bii[N-2];
            
      new_ar = (ar + ((mult_brr_trunc) - (mult_bii_trunc)));
      new_ai = (ai + ((mult_bri_trunc) + (mult_bir_trunc)));
      new_br = (ar - ((mult_brr_trunc) - (mult_bii_trunc)));
      new_bi = (ai - ((mult_bri_trunc) + (mult_bir_trunc)));               
    }

void fft(complex_in in[8], complex_out out[8]) {
  
  sc_int<27> W[4][2] ;//= {67108863, 0, 47453133, -47453133, 0, -67108864, -47453133, -47453133};
  W[0][0] = 67108863; //2^26-1
  W[0][1] = 0;
  W[1][0] = 47453133;
  W[1][1] = -47453133;
  W[2][0] = 0;
  W[2][1] = -67108864;//-2^26
  W[3][0] = -47453133; 
  W[3][1] = -47453133; 

  sc_int<L+2> sample_real_modif_1[8],sample_imag_modif_1[8]; //La sortie de l'étape 1
  sc_int<L+4> sample_real_modif_2[8],sample_imag_modif_2[8]; //La sortie de l'étape 2
  sc_int<L+6> sample_real_modif_3[8],sample_imag_modif_3[8]; //La sortie de l'étape 3
 
 // Etape 1
  papillon<L> (W[0][0].range(26,4), W[0][1].range(26,4), 
            in[0].real, in[0].imag, in[4].real, in[4].imag,
            sample_real_modif_1[0], sample_imag_modif_1[0], sample_real_modif_1[1], sample_imag_modif_1[1]);

  papillon<L> (W[0][0].range(26,4), W[0][1].range(26,4), 
            in[2].real, in[2].imag, in[6].real, in[6].imag,
            sample_real_modif_1[2], sample_imag_modif_1[2], sample_real_modif_1[3], sample_imag_modif_1[3]);

  papillon<L> (W[0][0].range(26,4), W[0][1].range(26,4), 
            in[1].real, in[1].imag, in[5].real, in[5].imag,
            sample_real_modif_1[4], sample_imag_modif_1[4], sample_real_modif_1[5], sample_imag_modif_1[5]);

  papillon<L> (W[0][0].range(26,4), W[0][1].range(26,4), 
            in[3].real, in[3].imag, in[7].real, in[7].imag,
            sample_real_modif_1[6], sample_imag_modif_1[6], sample_real_modif_1[7], sample_imag_modif_1[7]);

// Etape 2
  papillon<L+2> (W[0][0].range(26,2), W[0][1].range(26,2), 
            sample_real_modif_1[0], sample_imag_modif_1[0], sample_real_modif_1[2], sample_imag_modif_1[2],
            sample_real_modif_2[0], sample_imag_modif_2[0], sample_real_modif_2[2], sample_imag_modif_2[2]);

  papillon<L+2> (W[2][0].range(26,2), W[2][1].range(26,2), 
            sample_real_modif_1[1], sample_imag_modif_1[1], sample_real_modif_1[3], sample_imag_modif_1[3],
            sample_real_modif_2[1], sample_imag_modif_2[1], sample_real_modif_2[3], sample_imag_modif_2[3]);

  papillon<L+2> (W[0][0].range(26,2), W[0][1].range(26,2), 
            sample_real_modif_1[4], sample_imag_modif_1[4], sample_real_modif_1[6], sample_imag_modif_1[6],
            sample_real_modif_2[4], sample_imag_modif_2[4], sample_real_modif_2[6], sample_imag_modif_2[6]);

  papillon<L+2> (W[2][0].range(26,2), W[2][1].range(26,2), 
            sample_real_modif_1[5], sample_imag_modif_1[5], sample_real_modif_1[7], sample_imag_modif_1[7],
            sample_real_modif_2[5], sample_imag_modif_2[5], sample_real_modif_2[7], sample_imag_modif_2[7]);    
// Etape 3
  papillon<L+4> (W[0][0], W[0][1], 
            sample_real_modif_2[0], sample_imag_modif_2[0], sample_real_modif_2[4], sample_imag_modif_2[4],
            sample_real_modif_3[0], sample_imag_modif_3[0], sample_real_modif_3[4], sample_imag_modif_3[4]);

  papillon<L+4> (W[1][0], W[1][1], 
            sample_real_modif_2[1], sample_imag_modif_2[1], sample_real_modif_2[5], sample_imag_modif_2[5],
            sample_real_modif_3[1], sample_imag_modif_3[1], sample_real_modif_3[5], sample_imag_modif_3[5]);

  papillon<L+4> (W[2][0], W[2][1], 
            sample_real_modif_2[2], sample_imag_modif_2[2], sample_real_modif_2[6], sample_imag_modif_2[6],
            sample_real_modif_3[2], sample_imag_modif_3[2], sample_real_modif_3[6], sample_imag_modif_3[6]);

  papillon<L+4> (W[3][0], W[3][1], 
            sample_real_modif_2[3], sample_imag_modif_2[3], sample_real_modif_2[7], sample_imag_modif_2[7],
            sample_real_modif_3[3], sample_imag_modif_3[3], sample_real_modif_3[7], sample_imag_modif_3[7]);
            
  // Les données sorties du fft_8bit auront une précision de (18,5).
  for (int i=0; i<8; i++)
  {    
      out[i].real = sample_real_modif_3[i].range(26,0);
      out[i].imag = sample_imag_modif_3[i].range(26,0);
  }
 }
