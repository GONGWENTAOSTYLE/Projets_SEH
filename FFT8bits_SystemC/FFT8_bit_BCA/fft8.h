#include <systemc.h>
#ifndef __FFT8_H__
#define __FFT8_H__

SC_MODULE(FFT8)
{

 public :
  sc_in_clk clk;
  sc_in<float> in_real, in_imag;
  sc_in<bool> in_valid, in_req;
  sc_out<float> out_real,out_imag;
  sc_out<bool> out_req,out_valid;
  SC_CTOR(FFT8)
    {
      SC_THREAD (comportement);
      sensitive << clk.pos();
    }
 private :
  void comportement();

};
#endif
