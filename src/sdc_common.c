/*#include <stdlib.h>*/
/*#include <stdio.h>*/
/*#include <stdarg.h>*/
/*#include <string.h>*/
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cassert>

#include "sdc_common.h"

namespace sdcparse {

constexpr float UNINITIALIZED_FLOAT = -1.0;
constexpr int UNINITIALIZED_INT = -1;

/*
 * Data structure to store the SDC Commands
 */
Commands* g_sdc_commands;

/*
 * Functions for SDC Command List
 */
Commands* alloc_sdc_commands() {

    //Alloc and initialize to empty
    Commands* sdc_commands = (Commands*) calloc(1, sizeof(Commands));

    assert(sdc_commands != NULL);
    assert(sdc_commands->has_commands == false);

    return sdc_commands;
}

void free_sdc_commands(Commands* sdc_commands) {
    if(sdc_commands != NULL) {
        for(int i = 0; i < sdc_commands->num_create_clock_cmds; i++) {
            free_sdc_create_clock(sdc_commands->create_clock_cmds[i]);
        }
        free(sdc_commands->create_clock_cmds);

        for(int i = 0; i < sdc_commands->num_set_input_delay_cmds; i++) {
            free_sdc_set_io_delay(sdc_commands->set_input_delay_cmds[i]);
        }
        free(sdc_commands->set_input_delay_cmds);

        for(int i = 0; i < sdc_commands->num_set_output_delay_cmds; i++) {
            free_sdc_set_io_delay(sdc_commands->set_output_delay_cmds[i]);
        }
        free(sdc_commands->set_output_delay_cmds);

        for(int i = 0; i < sdc_commands->num_set_clock_groups_cmds; i++) {
            free_sdc_set_clock_groups(sdc_commands->set_clock_groups_cmds[i]);
        }
        free(sdc_commands->set_clock_groups_cmds);

        for(int i = 0; i < sdc_commands->num_set_false_path_cmds; i++) {
            free_sdc_set_false_path(sdc_commands->set_false_path_cmds[i]);
        }
        free(sdc_commands->set_false_path_cmds);

        for(int i = 0; i < sdc_commands->num_set_max_delay_cmds; i++) {
            free_sdc_set_max_delay(sdc_commands->set_max_delay_cmds[i]);
        }
        free(sdc_commands->set_max_delay_cmds);

        for(int i = 0; i < sdc_commands->num_set_multicycle_path_cmds; i++) {
            free_sdc_set_multicycle_path(sdc_commands->set_multicycle_path_cmds[i]);
        }
        free(sdc_commands->set_multicycle_path_cmds);

        free(sdc_commands);
    }
}

/*
 * Functions for create_clock
 */
CreateClock* alloc_sdc_create_clock() {
    CreateClock* sdc_create_clock = (CreateClock*) malloc(sizeof(CreateClock));
    assert(sdc_create_clock != NULL);

    //Initialize
    sdc_create_clock->period = UNINITIALIZED_FLOAT;
    sdc_create_clock->name = NULL;
    sdc_create_clock->rise_edge = UNINITIALIZED_FLOAT;
    sdc_create_clock->fall_edge = UNINITIALIZED_FLOAT;
    sdc_create_clock->targets = NULL;
    sdc_create_clock->is_virtual = false;

    sdc_create_clock->file_line_number = UNINITIALIZED_INT;

    return sdc_create_clock;
}

void free_sdc_create_clock(CreateClock* sdc_create_clock) {
    if(sdc_create_clock != NULL) {
        free(sdc_create_clock->name);
        free_sdc_string_group(sdc_create_clock->targets);
        free(sdc_create_clock);
    }
}

CreateClock* sdc_create_clock_set_period(CreateClock* sdc_create_clock, double period) {
    assert(sdc_create_clock != NULL);
    if(sdc_create_clock->period != UNINITIALIZED_FLOAT) {
        sdc_error(yylineno, yytext, "Can only define a single clock period.\n"); 
    } else {
        sdc_create_clock->period = period;
    }

    return sdc_create_clock;
}

CreateClock* sdc_create_clock_set_name(CreateClock* sdc_create_clock, char* name) {
    assert(sdc_create_clock != NULL);
    if(sdc_create_clock->name != NULL) {
        sdc_error(yylineno, yytext, "Can only define a single clock name.\n");
    } else {
        sdc_create_clock->name = sdcparse::strdup(name);
    }

    return sdc_create_clock;
}

CreateClock* sdc_create_clock_set_waveform(CreateClock* sdc_create_clock, double rise_edge, double fall_edge) {
    assert(sdc_create_clock != NULL);
    if(sdc_create_clock->rise_edge != UNINITIALIZED_FLOAT || sdc_create_clock->fall_edge != UNINITIALIZED_FLOAT) {
        sdc_error(yylineno, yytext, "Can only define a single waveform.\n"); 
    } else {
        sdc_create_clock->rise_edge = rise_edge;
        sdc_create_clock->fall_edge = fall_edge;
    }

    return sdc_create_clock;
}

CreateClock* sdc_create_clock_add_targets(CreateClock* sdc_create_clock, StringGroup* target_group) {
    assert(sdc_create_clock != NULL);

    assert(target_group->group_type == StringGroupType::STRING);

    if(sdc_create_clock->targets != NULL) {
        sdc_error(yylineno, yytext, "Can only define a single set of targets for clock creation. "
                  "If you want to define multiple targets specify them as a list (e.g. \"{target1 target2}\".\n");
    }

    sdc_create_clock->targets = duplicate_sdc_string_group(target_group);

    return sdc_create_clock;
}

Commands* add_sdc_create_clock(Commands* sdc_commands, CreateClock* sdc_create_clock) {
    /*
     * Error Checking
     */
    assert(sdc_commands != NULL);
    assert(sdc_create_clock != NULL);

    //Allocate a default (empty) target if none was defined, since this clock may be virtual
    if(sdc_create_clock->targets == NULL) {
        sdc_create_clock->targets = alloc_sdc_string_group(StringGroupType::STRING);
        assert(sdc_create_clock->targets != NULL);
    }

    //Must have a clock period
    if(sdc_create_clock->period == UNINITIALIZED_FLOAT) {
        sdc_error(yylineno, yytext, "Must define clock period.\n");
    }

    //Must have either a target (if a netlist clock), or a name (if a virtual clock) 
    if(sdc_create_clock->targets->strings.size() == 0 && sdc_create_clock->name == NULL) {
        sdc_error(yylineno, yytext, "Must define either a target (for netlist clock) or a name (for virtual clock).\n");
    }

    //Currently we do not support defining clock names that differ from the netlist target name
    if(sdc_create_clock->targets->strings.size() != 0 && sdc_create_clock->name != NULL) {
        sdc_error(yylineno, yytext, "Currently custom names for netlist clocks are unsupported, remove '-name' option or create a virtual clock.\n");
    }

    /*
     * Set defaults
     */
    //Determine default rise/fall time for waveform
    if(sdc_create_clock->rise_edge == UNINITIALIZED_FLOAT && sdc_create_clock->fall_edge == UNINITIALIZED_FLOAT) {
        sdc_create_clock->rise_edge = 0.0;
        sdc_create_clock->fall_edge = sdc_create_clock->period / 2;
    }
    assert(sdc_create_clock->rise_edge != UNINITIALIZED_FLOAT);
    assert(sdc_create_clock->fall_edge != UNINITIALIZED_FLOAT);
    
    //Determine if clock is virtual or not
    if(sdc_create_clock->targets->strings.size() == 0 && sdc_create_clock->name != NULL) {
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
    sdc_create_clock->file_line_number = yylineno;

    /*
     * Add command
     */
    sdc_commands->has_commands = true;
    sdc_commands->num_create_clock_cmds++;
    sdc_commands->create_clock_cmds = (CreateClock**) realloc(sdc_commands->create_clock_cmds, sdc_commands->num_create_clock_cmds*sizeof(*sdc_commands->create_clock_cmds));
    sdc_commands->create_clock_cmds[sdc_commands->num_create_clock_cmds-1] = sdc_create_clock;

    return sdc_commands;
}

/*
 * Functions for set_input_delay/set_output_delay
 */
SetIoDelay* alloc_sdc_set_io_delay(IoDelayType cmd_type) {
    //Allocate
    SetIoDelay* sdc_set_io_delay = (SetIoDelay*) malloc(sizeof(SetIoDelay));
    assert(sdc_set_io_delay != NULL);

    //Initialize
    sdc_set_io_delay->cmd_type = cmd_type;
    sdc_set_io_delay->clock_name = NULL;
    sdc_set_io_delay->max_delay = UNINITIALIZED_FLOAT;
    sdc_set_io_delay->target_ports = NULL;

    sdc_set_io_delay->file_line_number = UNINITIALIZED_INT;

    return sdc_set_io_delay;
}

void free_sdc_set_io_delay(SetIoDelay* sdc_set_io_delay) {
    if(sdc_set_io_delay != NULL) {
        free(sdc_set_io_delay->clock_name);
        free_sdc_string_group(sdc_set_io_delay->target_ports);
        free(sdc_set_io_delay);
    }
}

SetIoDelay* sdc_set_io_delay_set_clock(SetIoDelay* sdc_set_io_delay, char* clock_name) {
    assert(sdc_set_io_delay != NULL);

    if(sdc_set_io_delay->clock_name != NULL) {
        sdc_error(yylineno, yytext, "Can only specify a single clock\n"); 
    }

    sdc_set_io_delay->clock_name = sdcparse::strdup(clock_name);
    return sdc_set_io_delay;
}

SetIoDelay* sdc_set_io_delay_set_max_value(SetIoDelay* sdc_set_io_delay, double max_value) {
    assert(sdc_set_io_delay != NULL);

    if(sdc_set_io_delay->max_delay != UNINITIALIZED_FLOAT) {
        sdc_error(yylineno, yytext, "Max delay value can only specified once.\n"); 
    }

    sdc_set_io_delay->max_delay = max_value;
    return sdc_set_io_delay;
}

SetIoDelay* sdc_set_io_delay_set_ports(SetIoDelay* sdc_set_io_delay, StringGroup* ports) {
    assert(sdc_set_io_delay != NULL);
    assert(ports != NULL);
    assert(ports->group_type == StringGroupType::PORT);

    if(sdc_set_io_delay->target_ports != NULL) {
        sdc_error(yylineno, yytext, "Currently only a single get_ports command is supported.\n"); 
    }

    sdc_set_io_delay->target_ports = duplicate_sdc_string_group(ports);
    return sdc_set_io_delay;
}

Commands* add_sdc_set_io_delay(Commands* sdc_commands, SetIoDelay* sdc_set_io_delay) {
    assert(sdc_commands != NULL);
    assert(sdc_set_io_delay != NULL);
    /*
     * Error checks
     */
    if(sdc_set_io_delay->clock_name == NULL) {
        sdc_error(yylineno, yytext, "Must specify clock name.\n"); 
    }

    if(sdc_set_io_delay->max_delay == UNINITIALIZED_FLOAT) {
        sdc_error(yylineno, yytext, "Must specify max delay value.\n"); 
    }

    if(sdc_set_io_delay->target_ports == NULL) {
        sdc_error(yylineno, yytext, "Must specify target ports using get_ports.\n"); 
    }

    /*
     * Set line number
     */
    sdc_set_io_delay->file_line_number = yylineno;

    /*
     * Add command
     */
    if(sdc_set_io_delay->cmd_type == IoDelayType::INPUT) {
        sdc_commands->has_commands = true;
        sdc_commands->num_set_input_delay_cmds++;
        sdc_commands->set_input_delay_cmds = (SetIoDelay**) realloc(sdc_commands->set_input_delay_cmds, sdc_commands->num_set_input_delay_cmds*sizeof(*sdc_commands->set_input_delay_cmds));
        sdc_commands->set_input_delay_cmds[sdc_commands->num_set_input_delay_cmds-1] = sdc_set_io_delay;
    } else {
        assert(sdc_set_io_delay->cmd_type == IoDelayType::OUTPUT);
        sdc_commands->has_commands = true;
        sdc_commands->num_set_output_delay_cmds++;
        sdc_commands->set_output_delay_cmds = (SetIoDelay**) realloc(sdc_commands->set_output_delay_cmds, sdc_commands->num_set_output_delay_cmds*sizeof(*sdc_commands->set_output_delay_cmds));
        sdc_commands->set_output_delay_cmds[sdc_commands->num_set_output_delay_cmds-1] = sdc_set_io_delay;
    }

    return sdc_commands;
}

/*
 * Functions for set_clock_groups
 */
SetClockGroups* alloc_sdc_set_clock_groups() {
    //Allocate
    SetClockGroups* sdc_set_clock_groups = (SetClockGroups*) malloc(sizeof(SetClockGroups));
    assert(sdc_set_clock_groups != NULL);

    //Initialize
    sdc_set_clock_groups->type = ClockGroupsType::NONE;
    sdc_set_clock_groups->num_clock_groups = 0;
    sdc_set_clock_groups->clock_groups = NULL;

    sdc_set_clock_groups->file_line_number = UNINITIALIZED_INT;

    return sdc_set_clock_groups;
}

void free_sdc_set_clock_groups(SetClockGroups* sdc_set_clock_groups) {
    if(sdc_set_clock_groups != NULL) {
        for(int i = 0; i < sdc_set_clock_groups->num_clock_groups; i++) {
            free_sdc_string_group(sdc_set_clock_groups->clock_groups[i]);
        }
        free(sdc_set_clock_groups->clock_groups);
        free(sdc_set_clock_groups);
    }
}

SetClockGroups* sdc_set_clock_groups_set_type(SetClockGroups* sdc_set_clock_groups, ClockGroupsType type) {
    assert(sdc_set_clock_groups != NULL);

    if(sdc_set_clock_groups->type != ClockGroupsType::NONE) {
        sdc_error(yylineno, yytext, "Can only specify a single clock groups relation type (e.g. '-exclusive')\n"); 
    }

    sdc_set_clock_groups->type = type;
    return sdc_set_clock_groups;
}

SetClockGroups* sdc_set_clock_groups_add_group(SetClockGroups* sdc_set_clock_groups, StringGroup* clock_group) {
    assert(sdc_set_clock_groups != NULL);
    assert(clock_group != NULL);

    assert(clock_group->group_type == StringGroupType::CLOCK || clock_group->group_type == StringGroupType::STRING);

    //Allocate space
    sdc_set_clock_groups->num_clock_groups++;
    sdc_set_clock_groups->clock_groups = (StringGroup**) realloc(sdc_set_clock_groups->clock_groups, sdc_set_clock_groups->num_clock_groups*sizeof(*sdc_set_clock_groups->clock_groups));
    assert(sdc_set_clock_groups->clock_groups != NULL);

    //Duplicate and insert the clock group
    sdc_set_clock_groups->clock_groups[sdc_set_clock_groups->num_clock_groups-1] = duplicate_sdc_string_group(clock_group);

    return sdc_set_clock_groups;
}

Commands* add_sdc_set_clock_groups(Commands* sdc_commands, SetClockGroups* sdc_set_clock_groups) {
    /*
     * Error checks
     */
    if(sdc_set_clock_groups->type == ClockGroupsType::NONE) {
        sdc_error(yylineno, yytext, "Must specify clock relation type as '-exclusive'.\n"); 
    }

    if(sdc_set_clock_groups->num_clock_groups < 2) {
        sdc_error(yylineno, yytext, "Must specify at least 2 clock groups.\n"); 
    }

    /*
     * Set line number
     */
    sdc_set_clock_groups->file_line_number = yylineno;

    /*
     * Add command
     */
    sdc_commands->has_commands = true;
    sdc_commands->num_set_clock_groups_cmds++;
    sdc_commands->set_clock_groups_cmds = (SetClockGroups**) realloc(sdc_commands->set_clock_groups_cmds, sdc_commands->num_set_clock_groups_cmds*sizeof(*sdc_commands->set_clock_groups_cmds));
    sdc_commands->set_clock_groups_cmds[sdc_commands->num_set_clock_groups_cmds-1] = sdc_set_clock_groups;

    return sdc_commands;
}

/*
 * Functions for set_false_path
 */
SetFalsePath* alloc_sdc_set_false_path() {
    //Allocate
    SetFalsePath* sdc_set_false_path = (SetFalsePath*) malloc(sizeof(SetFalsePath));

    //Initialize
    sdc_set_false_path->from = NULL;
    sdc_set_false_path->to = NULL;

    sdc_set_false_path->file_line_number = UNINITIALIZED_INT;

    return sdc_set_false_path;
}

void free_sdc_set_false_path(SetFalsePath* sdc_set_false_path) {
    if(sdc_set_false_path != NULL) {
        free_sdc_string_group(sdc_set_false_path->from);
        free_sdc_string_group(sdc_set_false_path->to);

        free(sdc_set_false_path);
    }
}

SetFalsePath* sdc_set_false_path_add_to_from_group(SetFalsePath* sdc_set_false_path, 
                                                           StringGroup* group, 
                                                           FromToType to_from_dir) {
    assert(sdc_set_false_path != NULL);
    assert(group != NULL);
    assert(group->group_type == StringGroupType::CLOCK || group->group_type == StringGroupType::STRING);

    //Error checking
    if(to_from_dir == FromToType::FROM) {
        //Check that we haven't already defined the from path    
        if(sdc_set_false_path->from != NULL) {
            sdc_error(yylineno, yytext, "Only a single '-from' option is supported.\n"); 
        }
    } else {
        assert(to_from_dir == FromToType::TO);
        //Check that we haven't already defined the from path    
        if(sdc_set_false_path->to != NULL) {
            sdc_error(yylineno, yytext, "Only a single '-to' option is supported.\n"); 
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

Commands* add_sdc_set_false_path(Commands* sdc_commands, SetFalsePath* sdc_set_false_path) {
    /*
     * Error checks
     */
    if(sdc_set_false_path->from == NULL) {
        sdc_error(yylineno, yytext, "Must specify source clock(s)/object(s) with the '-from' option.\n"); 
    }

    if(sdc_set_false_path->to == NULL) {
        sdc_error(yylineno, yytext, "Must specify target clock(s)/objects(s) with the '-to' option.\n"); 
    }

    /*
     * Set line number
     */
    sdc_set_false_path->file_line_number = yylineno;

    /*
     * Add command
     */
    sdc_commands->has_commands = true;
    sdc_commands->num_set_false_path_cmds++;
    sdc_commands->set_false_path_cmds = (SetFalsePath**) realloc(sdc_commands->set_false_path_cmds, sdc_commands->num_set_false_path_cmds*sizeof(*sdc_commands->set_false_path_cmds));
    sdc_commands->set_false_path_cmds[sdc_commands->num_set_false_path_cmds-1] = sdc_set_false_path;

    return sdc_commands;
}

/*
 * Functions for set_max_delay
 */
SetMaxDelay* alloc_sdc_set_max_delay() {
    //Allocate
    SetMaxDelay* sdc_set_max_delay = (SetMaxDelay*) malloc(sizeof(SetMaxDelay));

    //Initialize
    sdc_set_max_delay->max_delay = UNINITIALIZED_FLOAT;
    sdc_set_max_delay->from = NULL;
    sdc_set_max_delay->to = NULL;

    sdc_set_max_delay->file_line_number = UNINITIALIZED_INT;

    return sdc_set_max_delay;
}

void free_sdc_set_max_delay(SetMaxDelay* sdc_set_max_delay) {
    if(sdc_set_max_delay != NULL) {
        free_sdc_string_group(sdc_set_max_delay->from);
        free_sdc_string_group(sdc_set_max_delay->to);
        free(sdc_set_max_delay);
    }
}

SetMaxDelay* sdc_set_max_delay_set_max_delay_value(SetMaxDelay* sdc_set_max_delay, double max_delay) {
    if(sdc_set_max_delay->max_delay != UNINITIALIZED_FLOAT) {
        sdc_error(yylineno, yytext, "Must specify max delay value only once.\n"); 
    }
    sdc_set_max_delay->max_delay = max_delay;
    return sdc_set_max_delay;
}

SetMaxDelay* sdc_set_max_delay_add_to_from_group(SetMaxDelay* sdc_set_max_delay, StringGroup* group, FromToType to_from_dir) {
    assert(sdc_set_max_delay != NULL);
    assert(group != NULL);
    assert(group->group_type == StringGroupType::CLOCK || group->group_type == StringGroupType::STRING);

    //Error checking
    if(to_from_dir == FromToType::FROM) {
        //Check that we haven't already defined the from path    
        if(sdc_set_max_delay->from != NULL) {
            sdc_error(yylineno, yytext, "Only a single '-from' option is supported.\n"); 
        }
    } else {
        assert(to_from_dir == FromToType::TO);
        //Check that we haven't already defined the from path    
        if(sdc_set_max_delay->to != NULL) {
            sdc_error(yylineno, yytext, "Only a single '-to' option is supported.\n"); 
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

Commands* add_sdc_set_max_delay(Commands* sdc_commands, SetMaxDelay* sdc_set_max_delay) {
    /*
     * Error checks
     */
    if(sdc_set_max_delay->max_delay == UNINITIALIZED_FLOAT) {
        sdc_error(yylineno, yytext, "Must specify the max delay value.\n"); 
    }

    if(sdc_set_max_delay->from == NULL) {
        sdc_error(yylineno, yytext, "Must specify source clock(s) with the '-from' option.\n"); 
    }

    if(sdc_set_max_delay->to == NULL) {
        sdc_error(yylineno, yytext, "Must specify source clock(s) with the '-to' option.\n"); 
    }

    /*
     * Set line number
     */
    sdc_set_max_delay->file_line_number = yylineno;

    /*
     * Add command
     */
    sdc_commands->has_commands = true;
    sdc_commands->num_set_max_delay_cmds++;
    sdc_commands->set_max_delay_cmds = (SetMaxDelay**) realloc(sdc_commands->set_max_delay_cmds, sdc_commands->num_set_max_delay_cmds*sizeof(*sdc_commands->set_max_delay_cmds));
    sdc_commands->set_max_delay_cmds[sdc_commands->num_set_max_delay_cmds-1] = sdc_set_max_delay;

    return sdc_commands;

}
/*
 * Functions for set_multicycle_path
 */
SetMulticyclePath* alloc_sdc_set_multicycle_path() {
    //Allocate
    SetMulticyclePath* sdc_set_multicycle_path = (SetMulticyclePath*) malloc(sizeof(SetMulticyclePath));
    assert(sdc_set_multicycle_path != NULL);

    //Initialize
    sdc_set_multicycle_path->type = McpType::NONE;
    sdc_set_multicycle_path->mcp_value = UNINITIALIZED_INT;
    sdc_set_multicycle_path->from = NULL;
    sdc_set_multicycle_path->to = NULL;

    sdc_set_multicycle_path->file_line_number = UNINITIALIZED_INT;

    return sdc_set_multicycle_path;

}
void free_sdc_set_multicycle_path(SetMulticyclePath* sdc_set_multicycle_path) {
    if(sdc_set_multicycle_path != NULL) {
        free_sdc_string_group(sdc_set_multicycle_path->from);
        free_sdc_string_group(sdc_set_multicycle_path->to);
        free(sdc_set_multicycle_path);
    }
}

SetMulticyclePath* sdc_set_multicycle_path_set_type(SetMulticyclePath* sdc_set_multicycle_path, McpType type) {
    if(sdc_set_multicycle_path->type != McpType::NONE) {
        sdc_error(yylineno, yytext, "Must specify the type (e.g. '-setup') only once.\n"); 
    }
    sdc_set_multicycle_path->type = type;
    return sdc_set_multicycle_path;
}

SetMulticyclePath* sdc_set_multicycle_path_set_mcp_value(SetMulticyclePath* sdc_set_multicycle_path, int mcp_value) {
    if(sdc_set_multicycle_path->mcp_value != UNINITIALIZED_INT) {
        sdc_error(yylineno, yytext, "Must specify multicycle path value only once.\n"); 
    }
    sdc_set_multicycle_path->mcp_value = mcp_value;
    return sdc_set_multicycle_path;
}

SetMulticyclePath* sdc_set_multicycle_path_add_to_from_group(SetMulticyclePath* sdc_set_multicycle_path, 
                                                                     StringGroup* group, 
                                                                     FromToType to_from_dir) {
    assert(sdc_set_multicycle_path != NULL);
    assert(group != NULL);
    assert(group->group_type == StringGroupType::CLOCK || group->group_type == StringGroupType::STRING);

    //Error checking
    if(to_from_dir == FromToType::FROM) {
        //Check that we haven't already defined the from path    
        if(sdc_set_multicycle_path->from != NULL) {
            sdc_error(yylineno, yytext, "Only a single '-from' option is supported.\n"); 
        }
    } else {
        assert(to_from_dir == FromToType::TO);
        //Check that we haven't already defined the from path    
        if(sdc_set_multicycle_path->to != NULL) {
            sdc_error(yylineno, yytext, "Only a single '-to' option is supported.\n"); 
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

Commands* add_sdc_set_multicycle_path(Commands* sdc_commands, SetMulticyclePath* sdc_set_multicycle_path) {
    /*
     * Error checks
     */
    if(sdc_set_multicycle_path->type != McpType::SETUP) {
        sdc_error(yylineno, yytext, "Must specify the multicycle path type as '-setup'.\n"); 
    }

    if(sdc_set_multicycle_path->mcp_value == UNINITIALIZED_FLOAT) {
        sdc_error(yylineno, yytext, "Must specify the multicycle path value.\n"); 
    }

    if(sdc_set_multicycle_path->from == NULL) {
        sdc_error(yylineno, yytext, "Must specify source clock(s) with the '-from' option.\n"); 
    }

    if(sdc_set_multicycle_path->to == NULL) {
        sdc_error(yylineno, yytext, "Must specify source clock(s) with the '-to' option.\n"); 
    }

    /*
     * Set line number
     */
    sdc_set_multicycle_path->file_line_number = yylineno;

    /*
     * Add command
     */
    sdc_commands->has_commands = true;
    sdc_commands->num_set_multicycle_path_cmds++;
    sdc_commands->set_multicycle_path_cmds = (SetMulticyclePath**) realloc(sdc_commands->set_multicycle_path_cmds, sdc_commands->num_set_multicycle_path_cmds*sizeof(*sdc_commands->set_multicycle_path_cmds));
    sdc_commands->set_multicycle_path_cmds[sdc_commands->num_set_multicycle_path_cmds-1] = sdc_set_multicycle_path;

    return sdc_commands;
}

/*
 * Functions for string_group
 */
StringGroup* alloc_sdc_string_group(StringGroupType type) {
    //Allocate and initialize
    StringGroup* sdc_string_group = (StringGroup*) calloc(1, sizeof(StringGroup)); 
    assert(sdc_string_group != NULL);

    sdc_string_group->group_type = type;

    return sdc_string_group;
}

StringGroup* make_sdc_string_group(StringGroupType type, char* string) {
    //Convenience function for converting a single string into a group
    StringGroup* sdc_string_group = alloc_sdc_string_group(type);

    sdc_string_group_add_string(sdc_string_group, string);

    return sdc_string_group;
}

StringGroup* duplicate_sdc_string_group(StringGroup* string_group) {
    //Allocate
    StringGroup* new_sdc_string_group = (StringGroup*) malloc(sizeof(StringGroup));
    assert(new_sdc_string_group != NULL);

    //Deep Copy
    new_sdc_string_group->group_type = string_group->group_type;
    new_sdc_string_group->strings = string_group->strings;

    return new_sdc_string_group;
}

void free_sdc_string_group(StringGroup* sdc_string_group) {
    if(sdc_string_group != NULL) {

        free(sdc_string_group);
    }
}

StringGroup* sdc_string_group_add_string(StringGroup* sdc_string_group, const std::string& string) {
    assert(sdc_string_group != NULL);

    //Insert the new string
    sdc_string_group->strings.push_back(string);
    
    return sdc_string_group;
}

StringGroup* sdc_string_group_add_strings(StringGroup* sdc_string_group, StringGroup* string_group_to_add) {
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
