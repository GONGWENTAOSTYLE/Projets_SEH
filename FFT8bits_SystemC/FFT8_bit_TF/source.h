#include <systemc.h>
#ifndef SOURCE_H
#define SOURCE_H

SC_MODULE(Source)
{

 public :
  sc_in_clk clk;
  sc_fifo_out<float> real, imagine;

  SC_CTOR(Source)
    {
      SC_THREAD (comportement);
      sensitive << clk.pos();
    }
 private :
  void comportement();

};
#endif
