#include <stdio.h>

#include "sdc.h"
#include "sdc_common.h"
#include "sdc_parser.gen.h"


extern int yyparse();
extern FILE	*yyin;

namespace sdcparse {

bool SdcCommands::has_commands() {
    if(!create_clock_cmds.empty()) return true;
    if(!set_input_delay_cmds.empty()) return true;
    if(!set_output_delay_cmds.empty()) return true;
    if(!set_clock_groups_cmds.empty()) return true;
    if(!set_false_path_cmds.empty()) return true;
    if(!set_max_delay_cmds.empty()) return true;
    if(!set_multicycle_path_cmds.empty()) return true;
    return false; 
}

/*
 * Given a filename parses the file as an SDC file
 * and returns a pointer to a struct containing all
 * the sdc commands.  See sdc.h for data structure
 * detials.
 */
std::shared_ptr<SdcCommands> sdc_parse_filename(char* filename) {
    FILE* infile = fopen(filename, "r");
    if(infile != NULL) {
        sdc_parse_file(infile);
        fclose(infile);
    } else {
        fclose(infile);
        sdc_error(0, "", "Could not open file %s.\n", filename);
    }


    return g_sdc_commands;
}

std::shared_ptr<SdcCommands> sdc_parse_file(FILE* sdc_file) {
    yyin = sdc_file;

    Parser parser;
    int error = parser.parse();
    if(error) {
        sdc_error(0, "", "SDC Error: file failed to parse!\n");
    }

    return g_sdc_commands;
}

}
