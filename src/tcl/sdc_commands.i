/**
 * @file
 * @author  Alex Singer
 * @date    January 2026
 * @brief   SWIG interface file, used to define the TCL interface.
 */
%module sdc_commands

// Include some SWIG built-in libraries.
%include "std_string.i"
%include "std_vector.i"
%include "exception.i"

// Headers necessary.
%{
#include "sdc_commands.h"
extern int Sdc_commands_SafeInit(Tcl_Interp* interp);
%}

// Wrap all functions in a try-catch block to allow errors to be thrown from
// within the interface functions.
%exception {
    try {
        $function;
    } catch (const std::out_of_range& oor) {
        SWIG_exception(SWIG_IndexError, oor.what());
    } catch (const std::exception& e) {
        SWIG_exception(SWIG_RuntimeError, e.what());
    }
}

// Declare some standard types that are used within the interface.
namespace std {
    %template(StringVector) vector<std::string>;
    %template(DoubleVector) vector<double>;
    %template(IntVector) vector<int>;
}

// Include the header file of the internal commands which will be made into
// interface functions.
%include "sdc_commands.h"
