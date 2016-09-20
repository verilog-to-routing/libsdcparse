#include <stdio.h>

#include "sdc.h"
#include "sdc_common.h"
#include "sdc_parser.gen.h"


extern FILE	*sdcparse_in; //Global input file defined by flex

namespace sdcparse {

/*
 * Given a filename parses the file as an SDC file
 * and returns a pointer to a struct containing all
 * the sdc commands.  See sdc.h for data structure
 * detials.
 */
std::shared_ptr<SdcCommands> sdc_parse_filename(std::string filename) {
    return sdc_parse_filename(filename.c_str());
}

std::shared_ptr<SdcCommands> sdc_parse_filename(const char* filename) {
    std::shared_ptr<SdcCommands> sdc_commands;

    FILE* infile = fopen(filename, "r");
    if(infile != NULL) {
        sdc_commands = sdc_parse_file(infile);
        fclose(infile);
    } else {
        fclose(infile);
        sdc_error(0, "", "Could not open file %s.\n", filename);
    }

    return sdc_commands;
}

std::shared_ptr<SdcCommands> sdc_parse_file(FILE* sdc_file) {
    sdcparse_in = sdc_file;

    auto sdc_commands = std::make_shared<SdcCommands>();

    Parser parser(sdc_commands);
    int error = parser.parse();
    if(error) {
        sdc_error(0, "", "SDC Error: file failed to parse!\n");
    }

    return sdc_commands;
}

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

}
