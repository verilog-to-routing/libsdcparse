#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "assert.h"

#include "sdc_common.h"

#define UNINITIALIZED_FLOAT -1.0

/*
 * Data structure to store the SDC Commands
 */
t_sdc_commands* g_sdc_commands;

/*
 * Functions for SDC Command List
 */
t_sdc_commands* alloc_sdc_commands() {

    //Alloc and initialize to empty
    t_sdc_commands* sdc_commands = (t_sdc_commands*) calloc(1, sizeof(t_sdc_commands));

    assert(sdc_commands != NULL);

    return sdc_commands;
}

void free_sdc_commands(t_sdc_commands* sdc_commands) {
    for(int i = 0; i < sdc_commands->num_create_clock_cmds; i++) {
        free_sdc_create_clock(sdc_commands->create_clock_cmds[i]);
    }

    for(int i = 0; i < sdc_commands->num_set_input_delay_cmds; i++) {
        free_sdc_set_io_delay(sdc_commands->set_input_delay_cmds[i]);
    }

    for(int i = 0; i < sdc_commands->num_set_output_delay_cmds; i++) {
        free_sdc_set_io_delay(sdc_commands->set_output_delay_cmds[i]);
    }

    for(int i = 0; i < sdc_commands->num_set_clock_groups_cmds; i++) {
        free_sdc_set_clock_groups(sdc_commands->set_clock_groups_cmds[i]);
    }

    for(int i = 0; i < sdc_commands->num_set_false_path_cmds; i++) {
        free_sdc_set_false_path(sdc_commands->set_false_path_cmds[i]);
    }

    for(int i = 0; i < sdc_commands->num_set_max_delay_cmds; i++) {
        free_sdc_set_max_delay(sdc_commands->set_max_delay_cmds[i]);
    }

    for(int i = 0; i < sdc_commands->num_set_multicycle_path_cmds; i++) {
        free_sdc_set_multicycle_path(sdc_commands->set_multicycle_path_cmds[i]);
    }

    free(sdc_commands);
}

/*
 * Functions for create_clock
 */
t_sdc_create_clock* alloc_sdc_create_clock() {
    t_sdc_create_clock* sdc_create_clock = (t_sdc_create_clock*) malloc(sizeof(t_sdc_create_clock));
    assert(sdc_create_clock != NULL);

    //Initialize
    sdc_create_clock->period = UNINITIALIZED_FLOAT;
    sdc_create_clock->name = NULL;
    sdc_create_clock->rise_time = UNINITIALIZED_FLOAT;
    sdc_create_clock->fall_time = UNINITIALIZED_FLOAT;
    sdc_create_clock->target = NULL;
    sdc_create_clock->is_virtual = false;

    return sdc_create_clock;
}

void free_sdc_create_clock(t_sdc_create_clock* sdc_create_clock) {
    free(sdc_create_clock->name);
    free(sdc_create_clock->target);
    free(sdc_create_clock);
}

t_sdc_create_clock* sdc_create_clock_set_period(t_sdc_create_clock* sdc_create_clock, double period) {
    assert(sdc_create_clock != NULL);
    if(sdc_create_clock->period != UNINITIALIZED_FLOAT) {
        sdc_error("SDC Error: can only define a single clock period at line %d near '%s'\n", yylineno, yytext); 
    } else {
        sdc_create_clock->period = period;
    }

    return sdc_create_clock;
}

t_sdc_create_clock* sdc_create_clock_set_name(t_sdc_create_clock* sdc_create_clock, char* name) {
    assert(sdc_create_clock != NULL);
    if(sdc_create_clock->name != NULL) {
        sdc_error("SDC Error: can only define a single clock name at line %d near '%s'\n", yylineno, yytext);
    } else {
        sdc_create_clock->name = strdup(name);
    }

    return sdc_create_clock;
}

t_sdc_create_clock* sdc_create_clock_set_waveform(t_sdc_create_clock* sdc_create_clock, double rise_time, double fall_time) {
    assert(sdc_create_clock != NULL);
    if(sdc_create_clock->rise_time != UNINITIALIZED_FLOAT || sdc_create_clock->fall_time != UNINITIALIZED_FLOAT) {
        sdc_error("SDC Error: can only define a single waveform at line %d near '%s'\n", yylineno, yytext); 
    } else {
        sdc_create_clock->rise_time = rise_time;
        sdc_create_clock->fall_time = fall_time;
    }

    return sdc_create_clock;
}

t_sdc_create_clock* sdc_create_clock_set_target(t_sdc_create_clock* sdc_create_clock, char* target) {
    assert(sdc_create_clock != NULL);
    if(sdc_create_clock->target != NULL) {
        sdc_error("SDC Error: can only define a single clock target at line %d near '%s'\n", yylineno, yytext);
    } else {
        sdc_create_clock->target = strdup(target);
    }

    return sdc_create_clock;
}

t_sdc_commands* add_sdc_create_clock(t_sdc_commands* sdc_commands, t_sdc_create_clock* sdc_create_clock) {
    /*
     * Error Checking
     */
    assert(sdc_commands != NULL);
    assert(sdc_create_clock != NULL);

    //Must have a clock period
    if(sdc_create_clock->period == UNINITIALIZED_FLOAT) {
        sdc_error("SDC Error: Must define clock period at line %d near '%s'\n", yylineno, yytext);
    }

    //Must have either a target (if a netlist clock), or a name (if a virtual clock) 
    if(sdc_create_clock->target == NULL && sdc_create_clock->name == NULL) {
        sdc_error("SDC Error: Must define either a target (for netlist clock) or a name (for virtual clock) at line %d near '%s'\n", yylineno, yytext);
    }

    //Currently we do not support defining clock names that differ from the netlist target name
    if(sdc_create_clock->target != NULL && sdc_create_clock->name != NULL) {
        sdc_error("SDC Error: Currently custom names for netlist clocks are unsupported, remove '-name' option at line %d near '%s', or create a virtual clock.\n", yylineno, yytext);
    }

    /*
     * Set defaults
     */
    //Determine default rise/fall time for waveform
    if(sdc_create_clock->rise_time == UNINITIALIZED_FLOAT && sdc_create_clock->fall_time == UNINITIALIZED_FLOAT) {
        sdc_create_clock->rise_time = 0.0;
        sdc_create_clock->fall_time = sdc_create_clock->period / 2;
    }
    assert(sdc_create_clock->rise_time != UNINITIALIZED_FLOAT);
    assert(sdc_create_clock->fall_time != UNINITIALIZED_FLOAT);
    
    //Determine if clock is virtual or not
    if(sdc_create_clock->target != NULL) {
        //Have a real target so this is not a virtual clock
        sdc_create_clock->is_virtual = false;
    } else {
        sdc_create_clock->is_virtual = true;
    }

    /*
     * Add command
     */
    sdc_commands->num_create_clock_cmds++;
    sdc_commands->create_clock_cmds = (t_sdc_create_clock**) realloc(sdc_commands->create_clock_cmds, sdc_commands->num_create_clock_cmds*sizeof(*sdc_commands->create_clock_cmds));
    sdc_commands->create_clock_cmds[sdc_commands->num_create_clock_cmds-1] = sdc_create_clock;

    return sdc_commands;
}

/*
 * Functions for set_input_delay/set_output_delay
 */
t_sdc_set_io_delay* alloc_sdc_set_io_delay(t_sdc_io_delay_type type) {
    //Allocate
    t_sdc_set_io_delay* sdc_set_io_delay = (t_sdc_set_io_delay*) malloc(sizeof(t_sdc_set_io_delay));
    assert(sdc_set_io_delay != NULL);

    //Initialize
    sdc_set_io_delay->type = type;
    sdc_set_io_delay->clock_name = NULL;
    sdc_set_io_delay->max_delay = UNINITIALIZED_FLOAT;
    sdc_set_io_delay->target_ports = NULL;

    return sdc_set_io_delay;
}

void free_sdc_set_io_delay(t_sdc_set_io_delay* sdc_set_io_delay) {
    free(sdc_set_io_delay->clock_name);
    free_sdc_port_group(sdc_set_io_delay->target_ports);
    free(sdc_set_io_delay);
}

t_sdc_set_io_delay* sdc_set_io_delay_set_clock(t_sdc_set_io_delay* sdc_set_io_delay, char* clock_name) {
    if(sdc_set_io_delay->clock_name != NULL) {
        sdc_error("SDC Error: can only specify a single clock at line %d near '%s'\n", yylineno, yytext); 
    }
    sdc_set_io_delay->clock_name = strdup(clock_name);
    return sdc_set_io_delay;
}

t_sdc_set_io_delay* sdc_set_io_delay_set_max_value(t_sdc_set_io_delay* sdc_set_io_delay, double max_value) {
    if(sdc_set_io_delay->max_delay != UNINITIALIZED_FLOAT) {
        sdc_error("SDC Error: max delay can only specified once at line %d near '%s'\n", yylineno, yytext); 
    }
    sdc_set_io_delay->max_delay = max_value;
    return sdc_set_io_delay;
}

t_sdc_set_io_delay* sdc_set_io_delay_set_ports(t_sdc_set_io_delay* sdc_set_io_delay, t_sdc_port_group* ports) {
    if(sdc_set_io_delay->target_ports != NULL) {
        sdc_error("SDC Error: currently on a single get_ports command is supported at line %d near '%s'\n", yylineno, yytext); 
    }
    sdc_set_io_delay->target_ports = duplicate_port_group(ports);
    return sdc_set_io_delay;
}

t_sdc_commands* add_sdc_set_io_delay(t_sdc_commands* sdc_commands, t_sdc_set_io_delay* sdc_set_io_delay) {
    /*
     * Error checks
     */
    if(sdc_set_io_delay->clock_name == NULL) {
        sdc_error("SDC Error: must specify clock name at line %d near '%s'\n", yylineno, yytext); 
    }

    if(sdc_set_io_delay->max_delay == UNINITIALIZED_FLOAT) {
        sdc_error("SDC Error: must specify max delay value at line %d near '%s'\n", yylineno, yytext); 
    }

    if(sdc_set_io_delay->target_ports == NULL) {
        sdc_error("SDC Error: must specify target ports using get_ports at line %d near '%s'\n", yylineno, yytext); 
    }

    /*
     * Add command
     */
    if(sdc_set_io_delay->type == INPUT_DELAY) {
        sdc_commands->num_set_input_delay_cmds++;
        sdc_commands->set_input_delay_cmds = (t_sdc_set_io_delay**) realloc(sdc_commands->set_input_delay_cmds, sdc_commands->num_set_input_delay_cmds*sizeof(*sdc_commands->set_input_delay_cmds));
        sdc_commands->set_input_delay_cmds[sdc_commands->num_set_input_delay_cmds-1] = sdc_set_io_delay;
    } else {
        assert(sdc_set_io_delay->type == OUTPUT_DELAY);
        sdc_commands->num_set_output_delay_cmds++;
        sdc_commands->set_output_delay_cmds = (t_sdc_set_io_delay**) realloc(sdc_commands->set_output_delay_cmds, sdc_commands->num_set_output_delay_cmds*sizeof(*sdc_commands->set_output_delay_cmds));
        sdc_commands->set_output_delay_cmds[sdc_commands->num_set_output_delay_cmds-1] = sdc_set_io_delay;
    }

    return sdc_commands;
}

/*
 * Functions for set_clock_groups
 */
t_sdc_set_clock_groups* alloc_sdc_set_clock_groups() {
    //Allocate
    t_sdc_set_clock_groups* sdc_set_clock_groups = (t_sdc_set_clock_groups*) malloc(sizeof(t_sdc_set_clock_groups));
    assert(sdc_set_clock_groups != NULL);

    //Initialize
    sdc_set_clock_groups->type = CG_NONE;
    sdc_set_clock_groups->num_clock_groups = 0;
    sdc_set_clock_groups->clock_groups = NULL;

    return sdc_set_clock_groups;
}

void free_sdc_set_clock_groups(t_sdc_set_clock_groups* sdc_set_clock_groups) {
    for(int i = 0; i < sdc_set_clock_groups->num_clock_groups; i++) {
        free_sdc_clock_group(sdc_set_clock_groups->clock_groups[i]);
    }
    free(sdc_set_clock_groups);
}

t_sdc_set_clock_groups* sdc_set_clock_groups_set_type(t_sdc_set_clock_groups* sdc_set_clock_groups, t_sdc_clock_groups_type type) {
    if(sdc_set_clock_groups->type != CG_NONE) {
        sdc_error("SDC Error: can only specify a single clock groups relation type (e.g. '-exclusive') at line %d near '%s'\n", yylineno, yytext); 
    }
    sdc_set_clock_groups->type = type;
    return sdc_set_clock_groups;
}

t_sdc_set_clock_groups* sdc_set_clock_groups_add_group(t_sdc_set_clock_groups* sdc_set_clock_groups, t_sdc_clock_group* clock_group) {
    //Allocate space
    sdc_set_clock_groups->num_clock_groups++;
    sdc_set_clock_groups->clock_groups = (t_sdc_clock_group**) realloc(sdc_set_clock_groups->clock_groups, sdc_set_clock_groups->num_clock_groups*sizeof(*sdc_set_clock_groups->clock_groups));
    assert(sdc_set_clock_groups->clock_groups != NULL);

    //Duplicate and insert the clock group
    sdc_set_clock_groups->clock_groups[sdc_set_clock_groups->num_clock_groups-1] = duplicate_clock_group(clock_group);

    return sdc_set_clock_groups;
}

t_sdc_commands* add_sdc_set_clock_groups(t_sdc_commands* sdc_commands, t_sdc_set_clock_groups* sdc_set_clock_groups) {
    /*
     * Error checks
     */
    if(sdc_set_clock_groups->type == CG_NONE) {
        sdc_error("SDC Error: must specify clock relation type as '-exclusive' at line %d near '%s'\n", yylineno, yytext); 
    }

    if(sdc_set_clock_groups->num_clock_groups < 2) {
        sdc_error("SDC Error: must specify at least 2 clock groups at line %d near '%s'\n", yylineno, yytext); 
    }

    /*
     * Add command
     */
    sdc_commands->num_set_clock_groups_cmds++;
    sdc_commands->set_clock_groups_cmds = (t_sdc_set_clock_groups**) realloc(sdc_commands->set_clock_groups_cmds, sdc_commands->num_set_clock_groups_cmds*sizeof(*sdc_commands->set_clock_groups_cmds));
    sdc_commands->set_clock_groups_cmds[sdc_commands->num_set_clock_groups_cmds-1] = sdc_set_clock_groups;

    return sdc_commands;
}

/*
 * Functions for set_false_path
 */
t_sdc_set_false_path* alloc_sdc_set_false_path() {
    //Allocate
    t_sdc_set_false_path* sdc_set_false_path = (t_sdc_set_false_path*) malloc(sizeof(t_sdc_set_false_path));

    //Initialize
    sdc_set_false_path->from_clocks = NULL;
    sdc_set_false_path->to_clocks = NULL;

    return sdc_set_false_path;
}

void free_sdc_set_false_path(t_sdc_set_false_path* sdc_set_false_path) {
    free_sdc_clock_group(sdc_set_false_path->from_clocks);
    free_sdc_clock_group(sdc_set_false_path->to_clocks);
    free(sdc_set_false_path);
}

t_sdc_set_false_path* sdc_set_false_path_add_group(t_sdc_set_false_path* sdc_set_false_path, t_sdc_clock_group* clock_group, t_sdc_clock_group_dir clock_group_dir) {
    assert(sdc_set_false_path != NULL);

    //Error checking
    if(clock_group_dir == FROM) {
        //Check that we haven't already defined the from path    
        if(sdc_set_false_path->from_clocks != NULL) {
            sdc_error("SDC Error: only a single '-from' option is supported at line %d near '%s'\n", yylineno, yytext); 
        }
    } else {
        assert(clock_group_dir == TO);
        //Check that we haven't already defined the from path    
        if(sdc_set_false_path->to_clocks != NULL) {
            sdc_error("SDC Error: only a single '-to' option is supported at line %d near '%s'\n", yylineno, yytext); 
        }
    }

    //Add the clock group
    if(clock_group_dir == FROM) {
        sdc_set_false_path->from_clocks = duplicate_clock_group(clock_group);
    } else {
        assert(clock_group_dir == TO);
        sdc_set_false_path->to_clocks = duplicate_clock_group(clock_group);
    }

    return sdc_set_false_path;
}

t_sdc_commands* add_sdc_set_false_path(t_sdc_commands* sdc_commands, t_sdc_set_false_path* sdc_set_false_path) {
    /*
     * Error checks
     */
    if(sdc_set_false_path->from_clocks == NULL) {
        sdc_error("SDC Error: must specify source clock(s) with the '-from' option at line %d near '%s'\n", yylineno, yytext); 
    }

    if(sdc_set_false_path->to_clocks == NULL) {
        sdc_error("SDC Error: must specify source clock(s) with the '-to' option at line %d near '%s'\n", yylineno, yytext); 
    }

    /*
     * Add command
     */
    sdc_commands->num_set_false_path_cmds++;
    sdc_commands->set_false_path_cmds = (t_sdc_set_false_path**) realloc(sdc_commands->set_false_path_cmds, sdc_commands->num_set_false_path_cmds*sizeof(*sdc_commands->set_false_path_cmds));
    sdc_commands->set_false_path_cmds[sdc_commands->num_set_false_path_cmds-1] = sdc_set_false_path;

    return sdc_commands;
}

/*
 * Functions for set_max_delay
 */
t_sdc_set_max_delay* alloc_sdc_set_max_delay() {
    //Allocate
    t_sdc_set_max_delay* sdc_set_max_delay = (t_sdc_set_max_delay*) malloc(sizeof(t_sdc_set_max_delay));

    //Initialize
    sdc_set_max_delay->max_delay = UNINITIALIZED_FLOAT;
    sdc_set_max_delay->from_clocks = NULL;
    sdc_set_max_delay->to_clocks = NULL;

    return sdc_set_max_delay;
}

void free_sdc_set_max_delay(t_sdc_set_max_delay* sdc_set_max_delay) {
    free_sdc_clock_group(sdc_set_max_delay->from_clocks);
    free_sdc_clock_group(sdc_set_max_delay->to_clocks);
    free(sdc_set_max_delay);
}

t_sdc_set_max_delay* sdc_set_max_delay_set_max_delay(t_sdc_set_max_delay* sdc_set_max_delay, double max_delay) {
    if(sdc_set_max_delay->max_delay != UNINITIALIZED_FLOAT) {
        sdc_error("SDC Error: must specify max delay value only once at line %d near '%s'\n", yylineno, yytext); 
    }
    sdc_set_max_delay->max_delay = max_delay;
    return sdc_set_max_delay;
}

t_sdc_set_max_delay* sdc_set_max_delay_add_group(t_sdc_set_max_delay* sdc_set_max_delay, t_sdc_clock_group* clock_group, t_sdc_clock_group_dir clock_group_dir) {
    assert(sdc_set_max_delay != NULL);

    //Error checking
    if(clock_group_dir == FROM) {
        //Check that we haven't already defined the from path    
        if(sdc_set_max_delay->from_clocks != NULL) {
            sdc_error("SDC Error: only a single '-from' option is supported at line %d near '%s'\n", yylineno, yytext); 
        }
    } else {
        assert(clock_group_dir == TO);
        //Check that we haven't already defined the from path    
        if(sdc_set_max_delay->to_clocks != NULL) {
            sdc_error("SDC Error: only a single '-to' option is supported at line %d near '%s'\n", yylineno, yytext); 
        }
    }

    //Add the clock group
    if(clock_group_dir == FROM) {
        sdc_set_max_delay->from_clocks = duplicate_clock_group(clock_group);
    } else {
        assert(clock_group_dir == TO);
        sdc_set_max_delay->to_clocks = duplicate_clock_group(clock_group);
    }

    return sdc_set_max_delay;
}

t_sdc_commands* add_sdc_set_max_delay(t_sdc_commands* sdc_commands, t_sdc_set_max_delay* sdc_set_max_delay) {
    /*
     * Error checks
     */
    if(sdc_set_max_delay->max_delay == UNINITIALIZED_FLOAT) {
        sdc_error("SDC Error: must specify the max delay value at line %d near '%s'\n", yylineno, yytext); 
    }

    if(sdc_set_max_delay->from_clocks == NULL) {
        sdc_error("SDC Error: must specify source clock(s) with the '-from' option at line %d near '%s'\n", yylineno, yytext); 
    }

    if(sdc_set_max_delay->to_clocks == NULL) {
        sdc_error("SDC Error: must specify source clock(s) with the '-to' option at line %d near '%s'\n", yylineno, yytext); 
    }

    /*
     * Add command
     */
    sdc_commands->num_set_max_delay_cmds++;
    sdc_commands->set_max_delay_cmds = (t_sdc_set_max_delay**) realloc(sdc_commands->set_max_delay_cmds, sdc_commands->num_set_max_delay_cmds*sizeof(*sdc_commands->set_max_delay_cmds));
    sdc_commands->set_max_delay_cmds[sdc_commands->num_set_max_delay_cmds-1] = sdc_set_max_delay;

    return sdc_commands;

}
/*
 * Functions for set_multicycle_path
 */
t_sdc_set_multicycle_path* alloc_sdc_set_multicycle_path() {
    //Allocate
    t_sdc_set_multicycle_path* sdc_set_multicycle_path = (t_sdc_set_multicycle_path*) malloc(sizeof(t_sdc_set_multicycle_path));

    //Initialize
    sdc_set_multicycle_path->mcp_value = UNINITIALIZED_FLOAT;
    sdc_set_multicycle_path->from_clocks = NULL;
    sdc_set_multicycle_path->to_clocks = NULL;

    return sdc_set_multicycle_path;

}
void free_sdc_set_multicycle_path(t_sdc_set_multicycle_path* sdc_set_multicycle_path) {
    free_sdc_clock_group(sdc_set_multicycle_path->from_clocks);
    free_sdc_clock_group(sdc_set_multicycle_path->to_clocks);
    free(sdc_set_multicycle_path);
}

t_sdc_set_multicycle_path* sdc_set_multicycle_path_set_type(t_sdc_set_multicycle_path* sdc_set_multicycle_path, t_sdc_mcp_type type) {
    if(sdc_set_multicycle_path->type != MCP_NONE) {
        sdc_error("SDC Error: must specify the type (e.g. '-setup') only once at line %d near '%s'\n", yylineno, yytext); 
    }
    sdc_set_multicycle_path->type = type;
    return sdc_set_multicycle_path;
}

t_sdc_set_multicycle_path* sdc_set_multicycle_path_set_mcp_value(t_sdc_set_multicycle_path* sdc_set_multicycle_path, double mcp_value) {
    if(sdc_set_multicycle_path->mcp_value != UNINITIALIZED_FLOAT) {
        sdc_error("SDC Error: must specify multicycle path value only once at line %d near '%s'\n", yylineno, yytext); 
    }
    sdc_set_multicycle_path->mcp_value = mcp_value;
    return sdc_set_multicycle_path;
}

t_sdc_set_multicycle_path* sdc_set_multicycle_path_add_group(t_sdc_set_multicycle_path* sdc_set_multicycle_path, t_sdc_clock_group* clock_group, t_sdc_clock_group_dir clock_group_dir) {
    assert(sdc_set_multicycle_path != NULL);

    //Error checking
    if(clock_group_dir == FROM) {
        //Check that we haven't already defined the from path    
        if(sdc_set_multicycle_path->from_clocks != NULL) {
            sdc_error("SDC Error: only a single '-from' option is supported at line %d near '%s'\n", yylineno, yytext); 
        }
    } else {
        assert(clock_group_dir == TO);
        //Check that we haven't already defined the from path    
        if(sdc_set_multicycle_path->to_clocks != NULL) {
            sdc_error("SDC Error: only a single '-to' option is supported at line %d near '%s'\n", yylineno, yytext); 
        }
    }

    //Add the clock group
    if(clock_group_dir == FROM) {
        sdc_set_multicycle_path->from_clocks = duplicate_clock_group(clock_group);
    } else {
        assert(clock_group_dir == TO);
        sdc_set_multicycle_path->to_clocks = duplicate_clock_group(clock_group);
    }

    return sdc_set_multicycle_path;
}

t_sdc_commands* add_sdc_set_multicycle_path(t_sdc_commands* sdc_commands, t_sdc_set_multicycle_path* sdc_set_multicycle_path) {
    /*
     * Error checks
     */
    if(sdc_set_multicycle_path->type != MCP_SETUP) {
        sdc_error("SDC Error: must specify the multicycle path type as '-setup' at line %d near '%s'\n", yylineno, yytext); 
    }

    if(sdc_set_multicycle_path->mcp_value == UNINITIALIZED_FLOAT) {
        sdc_error("SDC Error: must specify the multicycle path value at line %d near '%s'\n", yylineno, yytext); 
    }

    if(sdc_set_multicycle_path->from_clocks == NULL) {
        sdc_error("SDC Error: must specify source clock(s) with the '-from' option at line %d near '%s'\n", yylineno, yytext); 
    }

    if(sdc_set_multicycle_path->to_clocks == NULL) {
        sdc_error("SDC Error: must specify source clock(s) with the '-to' option at line %d near '%s'\n", yylineno, yytext); 
    }

    /*
     * Add command
     */
    sdc_commands->num_set_multicycle_path_cmds++;
    sdc_commands->set_multicycle_path_cmds = (t_sdc_set_multicycle_path**) realloc(sdc_commands->set_multicycle_path_cmds, sdc_commands->num_set_multicycle_path_cmds*sizeof(*sdc_commands->set_multicycle_path_cmds));
    sdc_commands->set_multicycle_path_cmds[sdc_commands->num_set_multicycle_path_cmds-1] = sdc_set_multicycle_path;

    return sdc_commands;
}

/*
 * Functions for get_clocks
 */
t_sdc_clock_group* alloc_sdc_get_clocks() {
    //Allocate and initialize
    t_sdc_clock_group* sdc_clock_group = (t_sdc_clock_group*) calloc(1, sizeof(t_sdc_clock_group)); 
    assert(sdc_clock_group != NULL);

    return sdc_clock_group;
}

t_sdc_clock_group* duplicate_clock_group(t_sdc_clock_group* sdc_clock_group) {
    //Allocate
    t_sdc_clock_group* new_sdc_clock_group = (t_sdc_clock_group*) malloc(sizeof(t_sdc_clock_group));
    assert(new_sdc_clock_group != NULL);

    //Deep Copy
    new_sdc_clock_group->num_clocks = sdc_clock_group->num_clocks;

    new_sdc_clock_group->clocks = (char**) calloc(new_sdc_clock_group->num_clocks, sizeof(*new_sdc_clock_group->clocks));
    assert(new_sdc_clock_group->clocks != NULL);
    for(int i = 0; i < new_sdc_clock_group->num_clocks; i++) {
        new_sdc_clock_group->clocks[i] = strdup(sdc_clock_group->clocks[i]);
    }

    return new_sdc_clock_group;
}

void free_sdc_clock_group(t_sdc_clock_group* sdc_clock_group) {
    assert(sdc_clock_group != NULL);

    for(int i = 0; i < sdc_clock_group->num_clocks; i++) {
        free(sdc_clock_group->clocks[i]);
    }
    free(sdc_clock_group);
}

t_sdc_clock_group* sdc_get_clocks_add_clocks(t_sdc_clock_group* sdc_clock_group, t_sdc_string_group* clock_list) {
    assert(sdc_clock_group != NULL);
    assert(clock_list != NULL);

    //Save the old number of clocks so we know where to add from
    int old_num_clocks = sdc_clock_group->num_clocks;

    //Reallocat space
    sdc_clock_group->num_clocks += clock_list->num_strings;
    sdc_clock_group->clocks = (char**) realloc(sdc_clock_group->clocks, sdc_clock_group->num_clocks*sizeof(*sdc_clock_group->clocks));
    assert(sdc_clock_group->clocks != NULL);

    //Copy all the strings
    for(int i = 0; i < clock_list->num_strings; i++) {
        sdc_clock_group->clocks[old_num_clocks + i] = strdup(clock_list->strings[i]);
    }

    return sdc_clock_group;
}

t_sdc_clock_group* sdc_get_clocks_add_clock(t_sdc_clock_group* sdc_clock_group, char* clock_name) {
    assert(sdc_clock_group != NULL);

    //Allocate space
    sdc_clock_group->num_clocks++;
    sdc_clock_group->clocks = (char**) realloc(sdc_clock_group->clocks, sdc_clock_group->num_clocks*sizeof(*sdc_clock_group->clocks));
    assert(sdc_clock_group->clocks != NULL);

    //Insert the new string
    sdc_clock_group->clocks[sdc_clock_group->num_clocks-1] = strdup(clock_name);
    
    return sdc_clock_group;
}

/*
 * Functions for get_ports
 */
t_sdc_port_group* alloc_sdc_get_ports() {
    //Allocate and initialize
    t_sdc_port_group* sdc_port_group = (t_sdc_port_group*) calloc(1, sizeof(t_sdc_port_group)); 
    assert(sdc_port_group != NULL);

    return sdc_port_group;
}

t_sdc_port_group* duplicate_port_group(t_sdc_port_group* sdc_port_group) {
    //Allocate
    t_sdc_port_group* new_sdc_port_group = (t_sdc_port_group*) malloc(sizeof(t_sdc_port_group));
    assert(new_sdc_port_group != NULL);

    //Deep Copy
    new_sdc_port_group->num_ports = sdc_port_group->num_ports;

    new_sdc_port_group->ports = (char**) calloc(new_sdc_port_group->num_ports, sizeof(*new_sdc_port_group->ports));
    assert(new_sdc_port_group->ports != NULL);
    for(int i = 0; i < new_sdc_port_group->num_ports; i++) {
        new_sdc_port_group->ports[i] = strdup(sdc_port_group->ports[i]);
    }

    return new_sdc_port_group;
}

void free_sdc_port_group(t_sdc_port_group* sdc_port_group) {
    assert(sdc_port_group != NULL);

    for(int i = 0; i < sdc_port_group->num_ports; i++) {
        free(sdc_port_group->ports[i]);
    }
    free(sdc_port_group);
}

t_sdc_port_group* sdc_get_ports_add_ports(t_sdc_port_group* sdc_port_group, t_sdc_string_group* port_list) {
    assert(sdc_port_group != NULL);
    assert(port_list != NULL);

    //Save the old number of ports so we know where to add from
    int old_num_ports = sdc_port_group->num_ports;

    //Reallocat space
    sdc_port_group->num_ports += port_list->num_strings;
    sdc_port_group->ports = (char**) realloc(sdc_port_group->ports, sdc_port_group->num_ports*sizeof(*sdc_port_group->ports));
    assert(sdc_port_group->ports != NULL);

    //Copy all the strings
    for(int i = 0; i < port_list->num_strings; i++) {
        sdc_port_group->ports[old_num_ports + i] = strdup(port_list->strings[i]);
    }

    return sdc_port_group;
}

t_sdc_port_group* sdc_get_ports_add_port(t_sdc_port_group* sdc_port_group, char* port_name) {
    assert(sdc_port_group != NULL);

    //Allocate space
    sdc_port_group->num_ports++;
    sdc_port_group->ports = (char**) realloc(sdc_port_group->ports, sdc_port_group->num_ports*sizeof(*sdc_port_group->ports));
    assert(sdc_port_group->ports != NULL);

    //Insert the new string
    sdc_port_group->ports[sdc_port_group->num_ports-1] = strdup(port_name);
    
    return sdc_port_group;
}

/*
 * Functions for string_group
 */
t_sdc_string_group* alloc_sdc_string_group() {
    //Allocate and initialize
    t_sdc_string_group* sdc_string_group = (t_sdc_string_group*) calloc(1, sizeof(t_sdc_string_group)); 
    assert(sdc_string_group != NULL);

    return sdc_string_group;
}

void free_sdc_string_group(t_sdc_string_group* sdc_string_group) {
    assert(sdc_string_group != NULL);

    for(int i = 0; i < sdc_string_group->num_strings; i++) {
        free(sdc_string_group->strings[i]);
    }
    free(sdc_string_group);
}

t_sdc_string_group* sdc_string_group_add_string(t_sdc_string_group* sdc_string_group, char* string) {
    assert(sdc_string_group != NULL);

    //Allocate space
    sdc_string_group->num_strings++;
    sdc_string_group->strings = (char**) realloc(sdc_string_group->strings, sdc_string_group->num_strings*sizeof(*sdc_string_group->strings));
    assert(sdc_string_group->strings != NULL);

    //Insert the new string
    sdc_string_group->strings[sdc_string_group->num_strings-1] = strdup(string);
    
    return sdc_string_group;
}

/*
 * Error handling
 */
void sdc_error(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    exit(1);
}
