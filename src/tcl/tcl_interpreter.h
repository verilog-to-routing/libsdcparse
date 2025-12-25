
#include <iostream>
#include <tcl/tcl.h>
#include <stdexcept>
#include <string>

#include "sdc_commands.h"
#include "sdc_wrapper.h"

extern "C" {
    extern int Sdc_commands_Init(Tcl_Interp* interp);
}

namespace sdcparse {

class TclInterpreter {
  private:
    Tcl_Interp *interp;
  public:
    TclInterpreter(const char* argv0 = nullptr)
        : interp(nullptr) {

        static bool initLib = false;
        if (!initLib) {
            Tcl_FindExecutable(argv0);
            initLib = true;
        }
        interp = Tcl_CreateInterp();
        if (!interp) {
            // TODO: Make this a proper error.
            throw new std::runtime_error("failed to initialize Tcl library");
        }
        if (Tcl_Init(interp) != TCL_OK) {
            // TODO: Make this a proper error.
            throw new std::runtime_error("Failed to init tcl interpreter.");
        }

        // Register the SWIG commands.
        if (Sdc_commands_Init(interp) != TCL_OK) {
            // TODO: Make this a proper error.
            throw new std::runtime_error("SWIG module Mycommands_Init failed.");
        }

        // Add the sdc wrapper file.
        // This is used to make the C++ interfaces simpler.
        const char* sdc_wrapper = get_sdc_wrapper_script();
        if (Tcl_Eval(interp, sdc_wrapper) != TCL_OK) {
            std::cout << Tcl_GetStringResult(interp) << std::endl;
            throw new std::runtime_error("Failed to evaluate SDC wrapper.");
        }
    }

    ~TclInterpreter() {
        if (interp)
            Tcl_DeleteInterp(interp);
    }

    // TODO: Add a method to register the callback.

    std::string eval_file(const std::string& filename) {
        int code = Tcl_EvalFile(interp, filename.c_str());

        if (code >= TCL_ERROR) {
            // TODO: Make this a proper error.
            std::cout << Tcl_GetStringResult(interp) << std::endl;
            throw Tcl_GetStringResult(interp);
        }

        return std::string(Tcl_GetStringResult(interp));
    }
};

} // namespace sdcparse
