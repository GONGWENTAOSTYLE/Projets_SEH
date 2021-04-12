#include <systemc.h>
#ifndef SOURCE_H
#define SOURCE_H
#define L 23 // Longueur de bit pour des données entrées

SC_MODULE(Source)
{

 public :
  sc_in_clk clk;
  sc_in<bool> data_req;
  sc_out<sc_int<L>> real, imagine; //Parties réelles et imaginaires des données de sortie
  sc_out<bool> data_valid; 

  SC_CTOR(Source)
    {
      SC_THREAD (comportement);
      sensitive << clk.pos();
    }
 private :
  void comportement();

};
#endif
