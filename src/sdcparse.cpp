#include "sdcparse.hpp"

#include "tcl_interpreter.h"

namespace sdcparse {

void sdc_parse_filename(const std::string& filename, Callback& callback) {
    TclInterpreter interpreter(callback);
    interpreter.eval_file(filename);
}

}
