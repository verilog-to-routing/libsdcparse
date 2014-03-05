#include <stdio.h>

#include "sdc.h"
#include "sdc_common.h"

extern int yyparse();
extern FILE	*yyin;

/*
 * Given a filename parses the file as an SDC file
 * and returns a pointer to a struct containing all
 * the sdc commands.  See sdc.h for data structure
 * detials.
 */
t_sdc_commands* sdc_parse_file(char* filename) {
    yyin = fopen(filename, "r");
    if(yyin != NULL) {
        int error = yyparse();
        if(error) {
            sdc_error("SDC Error: file %s failed to parse!\n", filename);
        }
    } else {
        sdc_error("SDC Error: Could not open file %s!\n", filename);
    }

    return g_sdc_commands;
}
