#include <systemc.h>
#ifndef __FFT8_H__
#define __FFT8_H__

SC_MODULE(FFT8)
{

 public :
  sc_in_clk clk;
  sc_fifo_in<float> in_real,in_imag;
  sc_fifo_out <float> out_real,out_imag;
  SC_CTOR(FFT8)
    {
      SC_THREAD (comportement);
      sensitive << clk.pos();
    }
 private :
  void comportement();

};
#endif
