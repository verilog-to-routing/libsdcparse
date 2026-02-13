#include "sdcparse.hpp"

#include "tcl_interpreter.h"

namespace sdcparse {

void sdc_parse_filename(std::string filename, Callback& callback) {
    sdc_parse_filename(filename.c_str(), callback);
}

void sdc_parse_filename(const char* filename, Callback& callback) {
    TclInterpreter interpreter(callback);
    interpreter.eval_file(filename);
}

}
