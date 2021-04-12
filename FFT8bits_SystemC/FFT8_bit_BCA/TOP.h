#include "source.h"
#include "sink.h"
#include "fft8.h"
#include <systemc.h>
#ifndef TOP_H
#define TOP_H

SC_MODULE(TOP)
{

 public :
  sc_clock clk;
  sc_signal<float> FFT_input_real, FFT_input_imag, FFT_output_real, FFT_output_imag;    
  sc_signal<bool> FFT_input_req, FFT_input_valid, FFT_output_req, FFT_output_valid;
  
  Source *input;
  Sink *output;
  FFT8 *fft;

  SC_CTOR(TOP):  clk("clk", 2, SC_NS, 0.5)
    {
        input = new Source("SOURCE");
        output = new Sink("SINK");
        fft = new FFT8("fft8");
        (*input)(clk,FFT_input_req,FFT_input_real,FFT_input_imag,FFT_input_valid);
        (*output)(clk,FFT_output_real,FFT_output_imag,FFT_output_valid,FFT_output_req);
        (*fft)(clk,FFT_input_real,FFT_input_imag,FFT_input_valid,FFT_output_req,FFT_output_real,FFT_output_imag,FFT_input_req,FFT_output_valid);
    }
};
#endif