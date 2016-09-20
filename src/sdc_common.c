/*#include <stdlib.h>*/
/*#include <stdio.h>*/
/*#include <stdarg.h>*/
/*#include <string.h>*/
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cassert>
#include <cmath>

#include "sdc_common.h"

namespace sdcparse {

/*
 * Functions for create_clock
 */
std::shared_ptr<CreateClock> sdc_create_clock_set_period(std::shared_ptr<CreateClock> sdc_create_clock, double period) {
    assert(sdc_create_clock);
    if(!std::isnan(sdc_create_clock->period)) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Can only define a single clock period.\n"); 
    } else {
        sdc_create_clock->period = period;
    }

    return sdc_create_clock;
}

std::shared_ptr<CreateClock> sdc_create_clock_set_name(std::shared_ptr<CreateClock> sdc_create_clock, const std::string& name) {
    assert(sdc_create_clock);
    if(!sdc_create_clock->name.empty()) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Can only define a single clock name.\n");
    } else {
        sdc_create_clock->name = name;
    }

    return sdc_create_clock;
}

std::shared_ptr<CreateClock> sdc_create_clock_set_waveform(std::shared_ptr<CreateClock> sdc_create_clock, double rise_edge, double fall_edge) {
    assert(sdc_create_clock);
    if(!std::isnan(sdc_create_clock->rise_edge) || !std::isnan(sdc_create_clock->fall_edge)) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Can only define a single waveform.\n"); 
    } else {
        sdc_create_clock->rise_edge = rise_edge;
        sdc_create_clock->fall_edge = fall_edge;
    }

    return sdc_create_clock;
}

std::shared_ptr<CreateClock> sdc_create_clock_add_targets(std::shared_ptr<CreateClock> sdc_create_clock, std::shared_ptr<StringGroup> target_group) {
    assert(sdc_create_clock);

    assert(target_group->group_type == StringGroupType::STRING);

    if(sdc_create_clock->targets != NULL) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Can only define a single set of targets for clock creation. "
                  "If you want to define multiple targets specify them as a list (e.g. \"{target1 target2}\".\n");
    }

    sdc_create_clock->targets = duplicate_sdc_string_group(target_group);

    return sdc_create_clock;
}

std::shared_ptr<SdcCommands> add_sdc_create_clock(std::shared_ptr<SdcCommands> sdc_commands, std::shared_ptr<CreateClock> sdc_create_clock) {
    /*
     * Error Checking
     */
    assert(sdc_commands);
    assert(sdc_create_clock);

    //Allocate a default (empty) target if none was defined, since this clock may be virtual
    if(sdc_create_clock->targets == NULL) {
        sdc_create_clock->targets = std::make_shared<StringGroup>(StringGroupType::STRING);
    }

    //Must have a clock period
    if(std::isnan(sdc_create_clock->period)) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Must define clock period.\n");
    }

    //Must have either a target (if a netlist clock), or a name (if a virtual clock) 
    if(sdc_create_clock->targets->strings.size() == 0 && sdc_create_clock->name.empty()) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Must define either a target (for netlist clock) or a name (for virtual clock).\n");
    }

    //Currently we do not support defining clock names that differ from the netlist target name
    if(sdc_create_clock->targets->strings.size() != 0 && !sdc_create_clock->name.empty()) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Currently custom names for netlist clocks are unsupported, remove '-name' option or create a virtual clock.\n");
    }

    /*
     * Set defaults
     */
    //Determine default rise/fall time for waveform
    if(std::isnan(sdc_create_clock->rise_edge) && std::isnan(sdc_create_clock->fall_edge)) {
        sdc_create_clock->rise_edge = 0.0;
        sdc_create_clock->fall_edge = sdc_create_clock->period / 2;
    }
    assert(!std::isnan(sdc_create_clock->rise_edge));
    assert(!std::isnan(sdc_create_clock->fall_edge));
    
    //Determine if clock is virtual or not
    if(sdc_create_clock->targets->strings.size() == 0 && !sdc_create_clock->name.empty()) {
        //Have a virtual target if there is a name AND no target strings
        sdc_create_clock->is_virtual = true;
    } else {
        assert(sdc_create_clock->targets->strings.size() > 0);
        //Have a real target so this is not a virtual clock
        sdc_create_clock->is_virtual = false;
    }

    /*
     * Set line number
     */
    sdc_create_clock->file_line_number = sdcparse_lineno;

    /*
     * Add command
     */
    sdc_commands->create_clock_cmds.push_back(sdc_create_clock);

    return sdc_commands;
}

/*
 * Functions for set_input_delay/set_output_delay
 */
std::shared_ptr<SetIoDelay> sdc_set_io_delay_set_clock(std::shared_ptr<SetIoDelay> sdc_set_io_delay, const std::string& clock_name) {
    assert(sdc_set_io_delay);

    if(!sdc_set_io_delay->clock_name.empty()) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Can only specify a single clock\n"); 
    }

    sdc_set_io_delay->clock_name = clock_name;
    return sdc_set_io_delay;
}

std::shared_ptr<SetIoDelay> sdc_set_io_delay_set_max_value(std::shared_ptr<SetIoDelay> sdc_set_io_delay, double max_value) {
    assert(sdc_set_io_delay);

    if(!std::isnan(sdc_set_io_delay->max_delay)) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Max delay value can only specified once.\n"); 
    }

    sdc_set_io_delay->max_delay = max_value;
    return sdc_set_io_delay;
}

std::shared_ptr<SetIoDelay> sdc_set_io_delay_set_ports(std::shared_ptr<SetIoDelay> sdc_set_io_delay, std::shared_ptr<StringGroup> ports) {
    assert(sdc_set_io_delay);
    assert(ports);
    assert(ports->group_type == StringGroupType::PORT);

    if(sdc_set_io_delay->target_ports != NULL) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Currently only a single get_ports command is supported.\n"); 
    }

    sdc_set_io_delay->target_ports = duplicate_sdc_string_group(ports);
    return sdc_set_io_delay;
}

std::shared_ptr<SdcCommands> add_sdc_set_io_delay(std::shared_ptr<SdcCommands> sdc_commands, std::shared_ptr<SetIoDelay> sdc_set_io_delay) {
    assert(sdc_commands);
    assert(sdc_set_io_delay);
    /*
     * Error checks
     */
    if(sdc_set_io_delay->clock_name.empty()) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Must specify clock name.\n"); 
    }

    if(std::isnan(sdc_set_io_delay->max_delay)) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Must specify max delay value.\n"); 
    }

    if(sdc_set_io_delay->target_ports == NULL) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Must specify target ports using get_ports.\n"); 
    }

    /*
     * Set line number
     */
    sdc_set_io_delay->file_line_number = sdcparse_lineno;

    /*
     * Add command
     */
    if(sdc_set_io_delay->io_type == IoDelayType::INPUT) {
        sdc_commands->set_input_delay_cmds.push_back(sdc_set_io_delay);
    } else {
        assert(sdc_set_io_delay->io_type == IoDelayType::OUTPUT);
        sdc_commands->set_output_delay_cmds.push_back(sdc_set_io_delay);
    }

    return sdc_commands;
}

/*
 * Functions for set_clock_groups
 */
std::shared_ptr<SetClockGroups> sdc_set_clock_groups_set_type(std::shared_ptr<SetClockGroups> sdc_set_clock_groups, ClockGroupsType type) {
    assert(sdc_set_clock_groups);

    if(sdc_set_clock_groups->type != ClockGroupsType::NONE) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Can only specify a single clock groups relation type (e.g. '-exclusive')\n"); 
    }

    sdc_set_clock_groups->type = type;
    return sdc_set_clock_groups;
}

std::shared_ptr<SetClockGroups> sdc_set_clock_groups_add_group(std::shared_ptr<SetClockGroups> sdc_set_clock_groups, std::shared_ptr<StringGroup> clock_group) {
    assert(sdc_set_clock_groups);
    assert(clock_group);

    assert(clock_group->group_type == StringGroupType::CLOCK || clock_group->group_type == StringGroupType::STRING);

    //Duplicate and insert the clock group
    sdc_set_clock_groups->clock_groups.push_back(duplicate_sdc_string_group(clock_group));

    return sdc_set_clock_groups;
}

std::shared_ptr<SdcCommands> add_sdc_set_clock_groups(std::shared_ptr<SdcCommands> sdc_commands, std::shared_ptr<SetClockGroups> sdc_set_clock_groups) {
    /*
     * Error checks
     */
    if(sdc_set_clock_groups->type == ClockGroupsType::NONE) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Must specify clock relation type as '-exclusive'.\n"); 
    }

    if(sdc_set_clock_groups->clock_groups.size() < 2) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Must specify at least 2 clock groups.\n"); 
    }

    /*
     * Set line number
     */
    sdc_set_clock_groups->file_line_number = sdcparse_lineno;

    /*
     * Add command
     */
    sdc_commands->set_clock_groups_cmds.push_back(sdc_set_clock_groups);

    return sdc_commands;
}

/*
 * Functions for set_false_path
 */
std::shared_ptr<SetFalsePath> sdc_set_false_path_add_to_from_group(std::shared_ptr<SetFalsePath> sdc_set_false_path, 
                                                           std::shared_ptr<StringGroup> group, 
                                                           FromToType to_from_dir) {
    assert(sdc_set_false_path != NULL);
    assert(group != NULL);
    assert(group->group_type == StringGroupType::CLOCK || group->group_type == StringGroupType::STRING);

    //Error checking
    if(to_from_dir == FromToType::FROM) {
        //Check that we haven't already defined the from path    
        if(sdc_set_false_path->from != NULL) {
            sdc_error(sdcparse_lineno, sdcparse_text, "Only a single '-from' option is supported.\n"); 
        }
    } else {
        assert(to_from_dir == FromToType::TO);
        //Check that we haven't already defined the from path    
        if(sdc_set_false_path->to != NULL) {
            sdc_error(sdcparse_lineno, sdcparse_text, "Only a single '-to' option is supported.\n"); 
        }
    }

    //Add the clock group
    if(to_from_dir == FromToType::FROM) {
        sdc_set_false_path->from = duplicate_sdc_string_group(group);
    } else {
        assert(to_from_dir == FromToType::TO);
        sdc_set_false_path->to = duplicate_sdc_string_group(group);
    }

    return sdc_set_false_path;
}

std::shared_ptr<SdcCommands> add_sdc_set_false_path(std::shared_ptr<SdcCommands> sdc_commands, std::shared_ptr<SetFalsePath> sdc_set_false_path) {
    /*
     * Error checks
     */
    if(sdc_set_false_path->from == NULL) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Must specify source clock(s)/object(s) with the '-from' option.\n"); 
    }

    if(sdc_set_false_path->to == NULL) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Must specify target clock(s)/objects(s) with the '-to' option.\n"); 
    }

    /*
     * Set line number
     */
    sdc_set_false_path->file_line_number = sdcparse_lineno;

    /*
     * Add command
     */
    sdc_commands->set_false_path_cmds.push_back(sdc_set_false_path);

    return sdc_commands;
}

/*
 * Functions for set_max_delay
 */
std::shared_ptr<SetMaxDelay> sdc_set_max_delay_set_max_delay_value(std::shared_ptr<SetMaxDelay> sdc_set_max_delay, double max_delay) {
    if(!std::isnan(sdc_set_max_delay->max_delay)) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Must specify max delay value only once.\n"); 
    }
    sdc_set_max_delay->max_delay = max_delay;
    return sdc_set_max_delay;
}

std::shared_ptr<SetMaxDelay> sdc_set_max_delay_add_to_from_group(std::shared_ptr<SetMaxDelay> sdc_set_max_delay, std::shared_ptr<StringGroup> group, FromToType to_from_dir) {
    assert(sdc_set_max_delay != NULL);
    assert(group != NULL);
    assert(group->group_type == StringGroupType::CLOCK || group->group_type == StringGroupType::STRING);

    //Error checking
    if(to_from_dir == FromToType::FROM) {
        //Check that we haven't already defined the from path    
        if(sdc_set_max_delay->from != NULL) {
            sdc_error(sdcparse_lineno, sdcparse_text, "Only a single '-from' option is supported.\n"); 
        }
    } else {
        assert(to_from_dir == FromToType::TO);
        //Check that we haven't already defined the from path    
        if(sdc_set_max_delay->to != NULL) {
            sdc_error(sdcparse_lineno, sdcparse_text, "Only a single '-to' option is supported.\n"); 
        }
    }

    //Add the clock group
    if(to_from_dir == FromToType::FROM) {
        sdc_set_max_delay->from = duplicate_sdc_string_group(group);
    } else {
        assert(to_from_dir == FromToType::TO);
        sdc_set_max_delay->to = duplicate_sdc_string_group(group);
    }

    return sdc_set_max_delay;
}

std::shared_ptr<SdcCommands> add_sdc_set_max_delay(std::shared_ptr<SdcCommands> sdc_commands, std::shared_ptr<SetMaxDelay> sdc_set_max_delay) {
    /*
     * Error checks
     */
    if(std::isnan(sdc_set_max_delay->max_delay)) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Must specify the max delay value.\n"); 
    }

    if(sdc_set_max_delay->from == NULL) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Must specify source clock(s) with the '-from' option.\n"); 
    }

    if(sdc_set_max_delay->to == NULL) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Must specify source clock(s) with the '-to' option.\n"); 
    }

    /*
     * Set line number
     */
    sdc_set_max_delay->file_line_number = sdcparse_lineno;

    /*
     * Add command
     */
    sdc_commands->set_max_delay_cmds.push_back(sdc_set_max_delay);

    return sdc_commands;

}
/*
 * Functions for set_multicycle_path
 */
std::shared_ptr<SetMulticyclePath> sdc_set_multicycle_path_set_type(std::shared_ptr<SetMulticyclePath> sdc_set_multicycle_path, McpType type) {
    if(sdc_set_multicycle_path->type != McpType::NONE) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Must specify the type (e.g. '-setup') only once.\n"); 
    }
    sdc_set_multicycle_path->type = type;
    return sdc_set_multicycle_path;
}

std::shared_ptr<SetMulticyclePath> sdc_set_multicycle_path_set_mcp_value(std::shared_ptr<SetMulticyclePath> sdc_set_multicycle_path, int mcp_value) {
    if(sdc_set_multicycle_path->mcp_value != UNINITIALIZED_INT) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Must specify multicycle path value only once.\n"); 
    }
    sdc_set_multicycle_path->mcp_value = mcp_value;
    return sdc_set_multicycle_path;
}

std::shared_ptr<SetMulticyclePath> sdc_set_multicycle_path_add_to_from_group(std::shared_ptr<SetMulticyclePath> sdc_set_multicycle_path, 
                                                                     std::shared_ptr<StringGroup> group, 
                                                                     FromToType to_from_dir) {
    assert(sdc_set_multicycle_path != NULL);
    assert(group != NULL);
    assert(group->group_type == StringGroupType::CLOCK || group->group_type == StringGroupType::STRING);

    //Error checking
    if(to_from_dir == FromToType::FROM) {
        //Check that we haven't already defined the from path    
        if(sdc_set_multicycle_path->from != NULL) {
            sdc_error(sdcparse_lineno, sdcparse_text, "Only a single '-from' option is supported.\n"); 
        }
    } else {
        assert(to_from_dir == FromToType::TO);
        //Check that we haven't already defined the from path    
        if(sdc_set_multicycle_path->to != NULL) {
            sdc_error(sdcparse_lineno, sdcparse_text, "Only a single '-to' option is supported.\n"); 
        }
    }

    //Add the clock group
    if(to_from_dir == FromToType::FROM) {
        sdc_set_multicycle_path->from = duplicate_sdc_string_group(group);
    } else {
        assert(to_from_dir == FromToType::TO);
        sdc_set_multicycle_path->to = duplicate_sdc_string_group(group);
    }

    return sdc_set_multicycle_path;
}

std::shared_ptr<SdcCommands> add_sdc_set_multicycle_path(std::shared_ptr<SdcCommands> sdc_commands, std::shared_ptr<SetMulticyclePath> sdc_set_multicycle_path) {
    /*
     * Error checks
     */
    if(sdc_set_multicycle_path->type != McpType::SETUP) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Must specify the multicycle path type as '-setup'.\n"); 
    }

    if(sdc_set_multicycle_path->mcp_value == UNINITIALIZED_INT) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Must specify the multicycle path value.\n"); 
    }

    if(sdc_set_multicycle_path->from == NULL) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Must specify source clock(s) with the '-from' option.\n"); 
    }

    if(sdc_set_multicycle_path->to == NULL) {
        sdc_error(sdcparse_lineno, sdcparse_text, "Must specify source clock(s) with the '-to' option.\n"); 
    }

    /*
     * Set line number
     */
    sdc_set_multicycle_path->file_line_number = sdcparse_lineno;

    /*
     * Add command
     */
    sdc_commands->set_multicycle_path_cmds.push_back(sdc_set_multicycle_path);

    return sdc_commands;
}

/*
 * Functions for string_group
 */
std::shared_ptr<StringGroup> make_sdc_string_group(StringGroupType type, const std::string& string) {
    //Convenience function for converting a single string into a group
    auto sdc_string_group = std::make_shared<StringGroup>(type);

    sdc_string_group_add_string(sdc_string_group, string);

    return sdc_string_group;
}

std::shared_ptr<StringGroup> duplicate_sdc_string_group(std::shared_ptr<StringGroup> string_group) {
    return std::make_shared<StringGroup>(*string_group);
}

std::shared_ptr<StringGroup> sdc_string_group_add_string(std::shared_ptr<StringGroup> sdc_string_group, const std::string& string) {
    assert(sdc_string_group);

    //Insert the new string
    sdc_string_group->strings.push_back(string);
    
    return sdc_string_group;
}

std::shared_ptr<StringGroup> sdc_string_group_add_strings(std::shared_ptr<StringGroup> sdc_string_group, std::shared_ptr<StringGroup> string_group_to_add) {
    for(const auto& string : string_group_to_add->strings) {
        sdc_string_group_add_string(sdc_string_group, string);
    }
    return sdc_string_group;
}

char* strdup(const char* src) {
    if(src == NULL) {
        return NULL;
    }

    size_t len = std::strlen(src); //Number of char's excluding null terminator

    //Space for [0..len] chars
    char* new_str = (char*) std::malloc((len+1)*sizeof(*src));
    assert(new_str != NULL);

    //Copy chars from [0..len], i.e. src[len] should be null terminator
    std::memcpy(new_str, src, len+1);
    
    return new_str;
}

char* strndup(const char* src, size_t len) {
    if(src == NULL) {
        return NULL;
    }

    //Space for [0..len] chars
    char* new_str = (char*) std::malloc((len+1)*sizeof(*src));
    assert(new_str != NULL);

    //Copy chars from [0..len-1]
    std::memcpy(new_str, src, len);
    
    //Add the null terminator
    new_str[len] = '\0';

    return new_str;
}

/*
 * Error reporting
 */
#ifndef CUSTOM_ERROR_REPORT
void sdc_error(const int line_no, const char* near_text, const char* fmt, ...) {
    fprintf(stderr, "SDC Error line %d near '%s': ", line_no, near_text);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    exit(1);
}
#endif

} //namespace
