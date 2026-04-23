#include "sdcparse.hpp"

#if LIBSDCPARSE_WITH_TCL
#include "tcl_interpreter.h"
#endif

namespace sdcparse {

void sdc_parse_filename(const std::string& filename, Callback& callback) {
#if LIBSDCPARSE_WITH_TCL
    TclInterpreter interpreter(callback);
    interpreter.eval_file(filename);
#else
    (void)filename;
    (void)callback;
    throw std::runtime_error("libsdcparse built without TCL support");
#endif
}

} // namespace sdcparse
