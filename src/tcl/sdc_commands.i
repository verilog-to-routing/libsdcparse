/* sdc_commands.i */
%module sdc_commands
%include "std_string.i"
%include "std_vector.i"
%include "exception.i"
%{
#include "sdc_commands.h"
extern int Sdc_commands_SafeInit(Tcl_Interp* interp);
%}

%exception {
    try {
        $function;
    } catch (const std::out_of_range& oor) {
        SWIG_exception(SWIG_IndexError, oor.what());
    } catch (const std::exception& e) {
        SWIG_exception(SWIG_RuntimeError, e.what());
    }
}

namespace std {
    %template(StringVector) vector<std::string>;
    %template(DoubleVector) vector<double>;
    %template(IntVector) vector<int>;
}

%include "sdc_commands.h"
