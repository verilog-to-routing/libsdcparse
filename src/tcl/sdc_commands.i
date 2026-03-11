/**
 * @file
 * @author  Alex Singer
 * @date    January 2026
 * @brief   SWIG interface file, used to define the TCL interface.
 */
%module sdc_commands

%typemap(in) sdcparse::ObjectId {
    int val;
    // 1. Convert the TCL object (from the script) to a C int
    if (Tcl_GetIntFromObj(interp, $input, &val) != TCL_OK) {
        return TCL_ERROR;
    }
    // 2. Construct the C++ object on the fly
    // This assumes your struct has a constructor like ObjectId(int) 
    // or you can set a member like {val}
    $1 = sdcparse::ObjectId(val); 
}

%typemap(out) sdcparse::ObjectId {
    // Replace .id with the actual member name or accessor method
    Tcl_SetObjResult(interp, Tcl_NewIntObj(static_cast<size_t>($1.value)));
}

// %typemap(out) std::vector<sdcparse::ObjectId> {
//     Tcl_Obj *listPtr = Tcl_NewListObj(0, NULL);
//     for (const auto& id : $1) {
//         // Assuming ObjectId has a .name() or can be cast to int
//         Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewStringObj(id.c_str(), -1));
//     }
//     Tcl_SetObjResult(interp, listPtr);
// }

%typemap(in) std::vector<sdcparse::ObjectId> (std::vector<sdcparse::ObjectId> temp) {
    int listLen;
    Tcl_Obj **listPtr;

    // 1. Convert the TCL input into an array of Tcl_Obj pointers
    if (Tcl_ListObjGetElements(interp, $input, &listLen, &listPtr) != TCL_OK) {
        return TCL_ERROR;
    }

    // 2. Prepare our temporary C++ vector
    temp.reserve(listLen);

    for (int i = 0; i < listLen; i++) {
        int val;
        // 3. Extract the integer from the current list element
        if (Tcl_GetIntFromObj(interp, listPtr[i], &val) != TCL_OK) {
            // This provides a helpful error if the user passes a non-integer
            Tcl_SetObjResult(interp, Tcl_NewStringObj("Vector must contain integers only", -1));
            return TCL_ERROR;
        }
        
        // 4. Create the struct and push it into the vector
        // Assumes constructor: ObjectId(int) or brace initialization
        temp.push_back(sdcparse::ObjectId{val});
    }
    
    // 5. Assign the temporary vector to the actual function argument
    $1 = temp;
}

%typemap(in) std::vector<sdcparse::ObjectId> & (std::vector<sdcparse::ObjectId> temp),
             const std::vector<sdcparse::ObjectId> & (std::vector<sdcparse::ObjectId> temp) {
    int listLen;
    Tcl_Obj **listPtr;

    // 1. Get the elements from the TCL list
    if (Tcl_ListObjGetElements(interp, $input, &listLen, &listPtr) != TCL_OK) {
        return TCL_ERROR;
    }

    // 2. Fill the temporary vector
    temp.reserve(listLen);
    for (int i = 0; i < listLen; i++) {
        int val;
        if (Tcl_GetIntFromObj(interp, listPtr[i], &val) != TCL_OK) {
            Tcl_SetObjResult(interp, Tcl_NewStringObj("Vector must contain integers only", -1));
            return TCL_ERROR;
        }
        
        // Construct the struct (assuming .id is the member)
        sdcparse::ObjectId obj(val);
        temp.push_back(obj);
    }
    std::vector<sdcparse::ObjectId>* temp_ptr = &temp;
    
    // 3. Pass the address of 'temp' to the C++ function
    $1 = temp_ptr;
}

%typemap(out) std::vector<sdcparse::ObjectId> {
    Tcl_Obj *listPtr = Tcl_NewListObj(0, NULL);
    
    // Use the SWIG pointer/reference macro $1
    // We cast it to the actual vector type to bypass the wrapper
    const std::vector<sdcparse::ObjectId>& vec = (const std::vector<sdcparse::ObjectId>&)$1;
    
    for (size_t i = 0; i < vec.size(); i++) {
        // Accessing the ID member (replace .id with your actual member name)
        size_t id_val = static_cast<size_t>(vec[i]); 
        Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewIntObj(id_val));
    }
    Tcl_SetObjResult(interp, listPtr);
}

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
    // %template(ObjectIdVector) vector<sdcparse::ObjectId>;
}

// Include the header file of the internal commands which will be made into
// interface functions.
%include "sdc_commands.h"
