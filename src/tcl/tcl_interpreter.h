#pragma once

#include <cassert>
#include <iostream>
#include <tcl/tcl.h>
#include <string>

#include "sdc_commands.h"
#include "sdc_wrapper.h"
#include "sdcparse.hpp"

extern "C" {
    extern int Sdc_commands_Init(Tcl_Interp* interp);
}

namespace sdcparse {

class TclInterpreter {
  private:
    Tcl_Interp *interp;

    bool init_success_;
  public:
    TclInterpreter(Callback& callback, const char* argv0 = nullptr)
        : interp(nullptr), init_success_(false) {

        // Register the callback.
        g_callback = &callback;

        // Initialize the Tcl Library.
        static bool initLib = false;
        if (!initLib) {
            Tcl_FindExecutable(argv0);
            initLib = true;
        }

        // Create and initialize the Tcl Interpreter.
        interp = Tcl_CreateInterp();
        if (!interp) {
            callback.parse_error(0, "", "Failed to initialize Tcl library");
            return;
        }
        if (Tcl_Init(interp) != TCL_OK) {
            callback.parse_error(0, "", "Failed to init Tcl interpreter");
            return;
        }

        // Register the SWIG commands.
        if (Sdc_commands_Init(interp) != TCL_OK) {
            callback.parse_error(0, "", "SWIG module Sdc_commands_Init failed.");
            return;
        }

        // Add the sdc wrapper file.
        // This is used to make the C++ interfaces simpler.
        const char* sdc_wrapper = get_sdc_wrapper_script();
        if (Tcl_Eval(interp, sdc_wrapper) != TCL_OK) {
            print_error_log_();
            callback.parse_error(0, "", "Failed to evaluate SDC wrapper.");
            return;
        }

        // Mark this class as being fully initialized.
        init_success_ = true;
    }

    ~TclInterpreter() {
        // Delete the interpreter.
        if (interp)
            Tcl_DeleteInterp(interp);

        // Shut down TCL subsystem entirely. This should prevent any leaks.
        Tcl_Finalize();
    }

    void eval_file(const std::string& filename) {
        // Ensure that the class was initialized correctly.
        assert(g_callback != nullptr);
        if (!init_success_) {
            g_callback->parse_error(0, "", "Failed to parse due to interpreter initialization error.");
            return;
        }

        // Signal the start of the parse.
        g_callback->start_parse();

        // Set the filename.
        g_callback->filename(filename);

        // Use the interpreter to evaluate the file.
        int code = Tcl_EvalFile(interp, filename.c_str());

        // Check for any errors.
        if (code >= TCL_ERROR) {
            // NOTE: The stack trace here is very detailed, and may be too
            //       detailed for a user.
            // TODO: Make this optionally activated.
            print_error_log_();

            // Get the error line number.
            // Get the return options dictionary
            Tcl_Obj *options = Tcl_GetReturnOptions(interp, code);
            Tcl_Obj *lineKey = Tcl_NewStringObj("-errorline", -1);
            Tcl_Obj *lineVal;
            // Extract the line number from the dictionary
            Tcl_DictObjGet(NULL, options, lineKey, &lineVal);

            int line_number;
            Tcl_GetIntFromObj(interp, lineVal, &line_number);
            // Clean up
            Tcl_DecrRefCount(lineKey);

            // Signal that an error has occured.
            g_callback->parse_error(line_number, "", Tcl_GetStringResult(interp));
        }

        // Signal the end of parsing.
        g_callback->finish_parse();
    }

  private:
    void print_error_log_() {
        std::cerr << "--- SDC TCL Parse Error ---" << std::endl;
        const char* msg = Tcl_GetStringResult(interp);
        std::cerr << "Message: " << msg << std::endl;

        const char* stack_trace = Tcl_GetVar(interp, "errorInfo", TCL_GLOBAL_ONLY);
        if (stack_trace) {
            std::cerr << "Stack Trace:\n" << stack_trace << std::endl;
        }
        std::cerr << "---------------------------" << std::endl;
    }
};

} // namespace sdcparse
