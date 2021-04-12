#include <systemc.h>
#include "TOP.h"


int sc_main(int ac, char *av[]){

    //Instanciation du TOP
    TOP mainTop (" top ");
    sc_clock clk ("ID", 10, SC_NS, 0.5, 10, SC_NS, true);
    //mainTop.clk(clk);
    //Creation des traces 
    sc_trace_file *tf = sc_create_vcd_trace_file("trace");
    tf->set_time_unit(1, SC_NS);
    
    mainTop.FFT_input_req.trace(tf);
    mainTop.FFT_input_valid.trace(tf);
    mainTop.FFT_output_req.trace(tf);
    mainTop.FFT_output_valid.trace(tf);
    
    
    sc_start(2000, SC_NS);
    cout<< "Finished at" << sc_time_stamp() << "\n";
    //Fermeture du fichier de traces
	sc_close_vcd_trace_file(tf);
    
    return 0;
}
