#include <stdio.h>

#include "sdc.h"
#include "sdc_common.h"


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
SdcCommands* sdc_parse_filename(char* filename) {
    yyin = fopen(filename, "r");
    if(yyin != NULL) {
        int error = yyparse();
        if(error) {
            sdc_error(0, "", "File %s failed to parse.\n", filename);
        }
        fclose(yyin);
    } else {
        sdc_error(0, "", "Could not open file %s.\n", filename);
    }


    return g_sdc_commands;
}

SdcCommands* sdc_parse_file(FILE* sdc_file) {
    yyin = sdc_file;

    int error = yyparse();
    if(error) {
        sdc_error(0, "", "SDC Error: file failed to parse!\n");
    }

    return g_sdc_commands;
}

void sdc_parse_cleanup() {
    free_sdc_commands(g_sdc_commands);
}

}
