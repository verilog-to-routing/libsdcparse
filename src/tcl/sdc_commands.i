/**
 * @file
 * @author  Alex Singer
 * @date    January 2026
 * @brief   SWIG interface file, used to define the TCL interface.
 */
%module sdc_commands

// %typemap(in) sdcparse::ObjectId {
//     int val;
//     // 1. Convert the TCL object (from the script) to a C int
//     if (Tcl_GetIntFromObj(interp, $input, &val) != TCL_OK) {
//         return TCL_ERROR;
//     }
//     // 2. Construct the C++ object on the fly
//     // This assumes your struct has a constructor like ObjectId(int) 
//     // or you can set a member like {val}
//     $1 = sdcparse::ObjectId(val); 
// }

// %typemap(out) sdcparse::ObjectId {
//     // Replace .id with the actual member name or accessor method
//     Tcl_SetObjResult(interp, Tcl_NewIntObj(static_cast<size_t>($1.value)));
// }

// %typemap(out) std::vector<sdcparse::ObjectId> {
//     Tcl_Obj *listPtr = Tcl_NewListObj(0, NULL);
//     for (const auto& id : $1) {
//         // Assuming ObjectId has a .name() or can be cast to int
//         Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewStringObj(id.c_str(), -1));
//     }
//     Tcl_SetObjResult(interp, listPtr);
// }

// %typemap(out) std::vector<sdcparse::ObjectId> {
//     Tcl_Obj *listPtr = Tcl_NewListObj(0, NULL);
    
//     // Use the SWIG pointer/reference macro $1
//     // We cast it to the actual vector type to bypass the wrapper
//     const std::vector<sdcparse::ObjectId>& vec = (const std::vector<sdcparse::ObjectId>&)$1;
    
//     for (size_t i = 0; i < vec.size(); i++) {
//         // Accessing the ID member (replace .id with your actual member name)
//         size_t id_val = static_cast<size_t>(vec[i]); 
//         Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewIntObj(id_val));
//     }
//     Tcl_SetObjResult(interp, listPtr);
// }

// Include some SWIG built-in libraries.
%include "std_string.i"
%include "std_vector.i"
%include "exception.i"

// Headers necessary.
%{
#include "sdc_commands.h"
#include "sdc_timing_object.h"
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

// %include "../sdc_timing_object.h"

// Declare some standard types that are used within the interface.
namespace std {
    %template(StringVector) vector<std::string>;
    %template(DoubleVector) vector<double>;
    %template(IntVector) vector<int>;
    %template(ObjectIdVector) vector<sdcparse::ObjectId>;
}

// Include the header file of the internal commands which will be made into
// interface functions.
%include "sdc_commands.h"
