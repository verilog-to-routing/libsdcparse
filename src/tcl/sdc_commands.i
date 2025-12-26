/* sdc_commands.i */
%module sdc_commands
%include "std_string.i"
%include "std_vector.i"
%{
#include "sdc_commands.h"
extern int Sdc_commands_Init(Tcl_Interp* interp);
%}

namespace std {
    %template(StringVector) vector<std::string>;
    %template(DoubleVector) vector<double>;
}

// TODO: Update the prototype of this.
void create_clock_internal(double period,
                           const std::string& name,
                           const std::vector<double>& waveform,
                           bool add,
                           const std::vector<std::string>& targets);

void set_input_delay_internal(bool max_delay_flag,
                              bool min_delay_flag,
                              const std::string& clock_name,
                              double delay,
                              const std::vector<std::string>& targets);

// TODO: Investigate if this is too poor for performance
std::vector<std::string> all_ports_internal();

// TODO: Add more commands.
