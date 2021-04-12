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
  sc_fifo<float> fifo_FFT_input_real, fifo_FFT_input_imag,fifo_FFT_output_real,fifo_FFT_output_imag;
  
  Source *input;
  Sink *output;
  FFT8 *fft;

  SC_CTOR(TOP): 
    clk("clk", 2, SC_NS, 0.5),
    fifo_FFT_input_real(8), 
    fifo_FFT_input_imag(8), 
    fifo_FFT_output_real(8), 
    fifo_FFT_output_imag(8)
    {
        input = new Source("SOURCE");
        output = new Sink("SINK");
        fft = new FFT8("fft8");
        (*input)(clk,fifo_FFT_input_real,fifo_FFT_input_imag);
        (*output)(clk,fifo_FFT_output_real,fifo_FFT_output_imag);
        (*fft)(clk,fifo_FFT_input_real,fifo_FFT_input_imag,fifo_FFT_output_real,fifo_FFT_output_imag);
    }
};
#endif