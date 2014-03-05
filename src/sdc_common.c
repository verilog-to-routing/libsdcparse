#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "assert.h"

#include "sdc_common.h"

#define UNINITIALIZED_FLOAT -1.0
/*
 * Function declarations
 */
//SDC Command List manipulation
t_sdc_commands* alloc_sdc_commands();
void free_sdc_commands(t_sdc_commands* sdc_commands);

//create_clock manipulation
t_sdc_create_clock* alloc_sdc_create_clock();
void sdc_create_clock_set_period(t_sdc_create_clock* sdc_create_clock, double period);
void sdc_create_clock_set_name(t_sdc_create_clock* sdc_create_clock, char* name);
void sdc_create_clock_set_waveform(t_sdc_create_clock* sdc_create_clock, double rise_time, double fall_time);
void sdc_create_clock_set_target(t_sdc_create_clock* sdc_create_clock, char* target);
void add_sdc_create_clock(t_sdc_commands* sdc_commands, t_sdc_create_clock* sdc_create_clock);
void free_sdc_create_clock(t_sdc_create_clock* sdc_create_clock);

//Error reporting
void sdc_error(char* fmt, ...);

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

/*
 *    for(int i = 0; i < sdc_commands->num_set_input_delay_cmds; i++) {
 *        free_sdc_set_input_delay(sdc_commands->set_input_delay_cmds[i]);
 *    }
 *
 *    for(int i = 0; i < sdc_commands->num_set_output_delay_cmds; i++) {
 *        free_sdc_set_output_delay(sdc_commands->set_output_delay_cmds[i]);
 *    }
 *
 *    for(int i = 0; i < sdc_commands->num_set_clock_groups_cmds; i++) {
 *        free_sdc_set_clock_groups(sdc_commands->set_clock_groups_cmds[i]);
 *    }
 *
 *    for(int i = 0; i < sdc_commands->num_set_false_path_cmds; i++) {
 *        free_sdc_set_false_path(sdc_commands->set_false_path_cmds[i]);
 *    }
 *
 *    for(int i = 0; i < sdc_commands->num_set_max_delay_cmds; i++) {
 *        free_sdc_set_max_delay(sdc_commands->set_max_delay_cmds[i]);
 *    }
 *
 *    for(int i = 0; i < sdc_commands->num_set_multicycle_path_cmds; i++) {
 *        free_sdc_set_multicycle_path(sdc_commands->set_multicycle_path_cmds[i]);
 *    }
 */

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

void sdc_create_clock_set_period(t_sdc_create_clock* sdc_create_clock, double period) {
    assert(sdc_create_clock != NULL);
    if(sdc_create_clock->period != UNINITIALIZED_FLOAT) {
        sdc_error("SDC Error: can only define a single clock period at line %d near '%s'\n", yylineno, yytext); 
    } else {
        sdc_create_clock->period = period;
    }
}

void sdc_create_clock_set_name(t_sdc_create_clock* sdc_create_clock, char* name) {
    assert(sdc_create_clock != NULL);
    if(sdc_create_clock->name != NULL) {
        sdc_error("SDC Error: can only define a single clock name at line %d near '%s'\n", yylineno, yytext);
    } else {
        sdc_create_clock->name = strdup(name);
    }
}

void sdc_create_clock_set_waveform(t_sdc_create_clock* sdc_create_clock, double rise_time, double fall_time) {
    assert(sdc_create_clock != NULL);
    if(sdc_create_clock->rise_time != UNINITIALIZED_FLOAT || sdc_create_clock->fall_time != UNINITIALIZED_FLOAT) {
        sdc_error("SDC Error: can only define a single waveform at line %d near '%s'\n", yylineno, yytext); 
    } else {
        sdc_create_clock->rise_time = rise_time;
        sdc_create_clock->fall_time = fall_time;
    }
}

void sdc_create_clock_set_target(t_sdc_create_clock* sdc_create_clock, char* target) {
    assert(sdc_create_clock != NULL);
    if(sdc_create_clock->target != NULL) {
        sdc_error("SDC Error: can only define a single clock target at line %d near '%s'\n", yylineno, yytext);
    } else {
        sdc_create_clock->target = strdup(target);
    }
}

void add_sdc_create_clock(t_sdc_commands* sdc_commands, t_sdc_create_clock* sdc_create_clock) {
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
}

void free_sdc_create_clock(t_sdc_create_clock* sdc_create_clock) {
    free(sdc_create_clock->name);
    free(sdc_create_clock->target);
    free(sdc_create_clock);
}

void sdc_error(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    exit(1);
}
