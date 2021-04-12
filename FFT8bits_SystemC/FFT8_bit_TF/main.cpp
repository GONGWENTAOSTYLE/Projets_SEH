#include <systemc.h>
#include "TOP.h"


int sc_main(int ac, char *av[]){
    
    //Instanciation du TOP
    TOP mainTop (" top ");
    //Creation des traces 
    sc_trace_file *tf = sc_create_vcd_trace_file("trace");
    tf->set_time_unit(1, SC_NS);
    
    mainTop.fifo_FFT_input_real.trace(tf);
    mainTop.fifo_FFT_input_imag.trace(tf);
    mainTop.fifo_FFT_output_real.trace(tf);
    mainTop.fifo_FFT_output_imag.trace(tf);
    
    sc_start(2000, SC_NS);
    cout<< "Finished at" << sc_time_stamp() << "\n";
    //Fermeture du fichier de traces
	sc_close_vcd_trace_file(tf);
    
    return 0;
}
