#include <systemc.h>
#ifndef SINK_H
#define SINK_H

SC_MODULE(Sink)
{

 public :
  sc_in_clk clk;
  sc_in<float> real, imagine;
  sc_in<bool>  data_valid;
  sc_out<bool> data_req;

  SC_CTOR(Sink)
    {
      SC_THREAD (comportement);
      sensitive << clk.pos();
    }
 private :
  void comportement();

};
#endif
