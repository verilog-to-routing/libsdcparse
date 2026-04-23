/**
 * @file
 * @author  Alex Singer
 * @date    January 2026
 * @brief   SWIG interface file, used to define the TCL interface.
 */
%module sdc_commands

/**
 * Due to using custom ID objects to maintain our Strong IDs, the TCL interpreter needs to
 * be aware of how to handle these structs.
 *
 * We could just turn these IDs into integers; however this may cause issues since there will
 * be no way of differentiating these IDs from integers. This is because TCL treats all objects
 * as strings.
 *
 * To resolve this, we prefix the object strings for these IDs with "__vtr_obj_". This makes these
 * objects distinct and in-operable from within TCL.
 *
 * The following fragment is a set of helper methods to convert TCL objects and ObjectIds to/from
 * each other.
 *
 * The following typemaps declare to SWIG how it should handle different inputs/outputs of ObjectIds
 * and vectors of ObjectIds.
 */
%fragment("ObjectIdHelpers", "header") {
    // Convert ObjectId to TCL Handle
    static Tcl_Obj* ObjectId_To_Tcl(Tcl_Interp */*interp*/, const sdcparse::ObjectId& id) {
        size_t id_val = static_cast<size_t>(id);
        return Tcl_ObjPrintf("__vtr_obj_%lld", static_cast<Tcl_WideInt>(id_val));
    }

    // Convert TCL Handle to ObjectId
    static int Tcl_To_ObjectId(Tcl_Interp *interp, Tcl_Obj *obj, sdcparse::ObjectId *out) {
        Tcl_WideInt val;
        int parsed = 0;
        char *str = Tcl_GetString(obj);
        if (sscanf(str, "__vtr_obj_%lld%n", &val, &parsed) != 1 || str[parsed] != '\0' || val < 0) {
            Tcl_SetObjResult(interp, Tcl_ObjPrintf("Invalid ObjectId handle: %s", str));
            return TCL_ERROR;
        }
        *out = sdcparse::ObjectId(static_cast<size_t>(val));
        return TCL_OK;
    }
}

%typemap(in, fragment="ObjectIdHelpers") sdcparse::ObjectId {
    if (Tcl_To_ObjectId(interp, $input, &$1) != TCL_OK) return TCL_ERROR;
}

%typemap(out, fragment="ObjectIdHelpers") sdcparse::ObjectId {
    Tcl_SetObjResult(interp, ObjectId_To_Tcl(interp, $1));
}

%typemap(in, fragment="ObjectIdHelpers") std::vector<sdcparse::ObjectId> & (std::vector<sdcparse::ObjectId> temp),
                                         const std::vector<sdcparse::ObjectId> & (std::vector<sdcparse::ObjectId> temp) {
    int listLen;
    Tcl_Obj **elems;
    if (Tcl_ListObjGetElements(interp, $input, &listLen, &elems) != TCL_OK) return TCL_ERROR;

    temp.reserve(listLen);
    for (int i = 0; i < listLen; i++) {
        sdcparse::ObjectId id;
        if (Tcl_To_ObjectId(interp, elems[i], &id) != TCL_OK) return TCL_ERROR;
        temp.push_back(id);
    }
    std::vector<sdcparse::ObjectId>* temp_ptr = &temp;

    $1 = temp_ptr;
}

%typemap(out, fragment="ObjectIdHelpers") std::vector<sdcparse::ObjectId> {
    Tcl_Obj *listPtr = Tcl_NewListObj(0, NULL);
    const std::vector<sdcparse::ObjectId>& vec = (const std::vector<sdcparse::ObjectId>&)$1;
    for (const auto& id : vec) {
        Tcl_ListObjAppendElement(interp, listPtr, ObjectId_To_Tcl(interp, id));
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
SWIGEXPORT int Sdc_commands_SafeInit(Tcl_Interp* interp);
%}

// Wrap all functions in a try-catch block to allow errors to be thrown from
// within the interface functions.
// $function is no longer recognized, and is replaced by $action
// ref: https://swig.org/Release/CHANGES#:~:text=2025-06-09:%20olly%20The%20$function%20variable
%exception {
    try {
        $action;
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
