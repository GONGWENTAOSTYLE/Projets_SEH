#include <systemc.h>
#ifndef SINK_H
#define SINK_H
#define L 23 // Longueur de bit pour des données entrées
SC_MODULE(Sink)
{

 public :
  sc_in_clk clk;
  //Parties réelles et imaginaires des données d'entrée le fifo de sortie après l'opération de FFT. Longueur est de L+4.
  sc_in<sc_int<L+4>> real, imagine; 
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
