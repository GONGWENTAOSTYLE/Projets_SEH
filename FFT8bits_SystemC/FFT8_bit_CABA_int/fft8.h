#include <systemc.h>
#ifndef __FFT8_H__
#define __FFT8_H__
#define L 23

SC_MODULE(FFT8)
{

 public :
  sc_in_clk clk;
  sc_in<sc_int<L>> in_real, in_imag;
  sc_in<bool> in_valid, in_req;
  sc_out<sc_int<L+4>> out_real,out_imag;
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
