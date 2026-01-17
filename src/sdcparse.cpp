#include <stdio.h>

#include "sdcparse.hpp"

#include "sdc_lexer.hpp"
#include "sdc_error.hpp"

#include "tcl_interpreter.h"

namespace sdcparse {

void sdc_parse_filename(std::string filename, Callback& callback, bool use_tcl_interp) {
    sdc_parse_filename(filename.c_str(), callback, use_tcl_interp);
}

void sdc_parse_filename(const char* filename, Callback& callback, bool use_tcl_interp) {
    if (use_tcl_interp) {
        TclInterpreter interpreter(callback);
        interpreter.eval_file(filename);
        return;
    }

    FILE* infile = std::fopen(filename, "r");
    if(infile != NULL) {
        //Parse the file
        sdc_parse_file(infile, callback, filename);

        std::fclose(infile);
    } else {
        sdc_error_wrap(callback, 0, "", "Could not open file '%s'.\n", filename);
    }
}

void sdc_parse_file(FILE* sdc_file, Callback& callback, const char* filename) {

    //Initialize the lexer
    Lexer lexer(sdc_file, callback);

    //Setup the parser + lexer
    Parser parser(lexer, callback);

    //Just before parsing starts
    callback.start_parse();

    //Tell the caller the file name
    callback.filename(filename); 

    //Do the actual parse
    int error = parser.parse();
    if(error) {
        sdc_error_wrap(callback, 0, "", "File '%s' failed to parse.\n", filename);
    }

    //Finished parsing
    callback.finish_parse();
}

}
