/* sdc_commands.i */
%module sdc_commands
%include "std_string.i"
%{
#include "sdc_commands.h"
extern int Sdc_commands_Init(Tcl_Interp* interp);
%}

// TODO: Update the prototype of this.
void create_clock_internal(double period, const std::string& name);

// TODO: Add more commands.
