#include <systemc.h>
#ifndef SINK_H
#define SINK_H

SC_MODULE(Sink)
{

 public :
  sc_in_clk clk;
  sc_fifo_in<float> real, imagine;

  SC_CTOR(Sink)
    {
      SC_THREAD (comportement);
      sensitive << clk.pos();
    }
 private :
  void comportement();

};
#endif
