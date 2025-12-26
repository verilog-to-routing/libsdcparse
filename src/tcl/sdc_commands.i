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

// TODO: Add more commands.
