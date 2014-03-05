/* simplest version of calculator */
%{
#include <stdio.h>
#include "sdc_common.h"
int yylex(void);
int yyerror(const char *msg);
%}

%union {
    char* strVal;
    double floatVal;

    t_sdc_commands* sdc_commands;

    t_sdc_create_clock* create_clock;
    t_sdc_set_io_delay* set_io_delay;
    t_sdc_set_clock_groups* set_clock_groups;
    t_sdc_set_false_path* set_false_path;
    t_sdc_set_max_delay* set_max_delay;
    t_sdc_set_multicycle_path* set_multicycle_path;

    t_sdc_port_group* port_group;
    t_sdc_clock_group* clock_group;
    t_sdc_string_group* string_group;

}

/* declare constant */
%token CMD_CREATE_CLOCK
%token CMD_SET_CLOCK_GROUPS
%token CMD_SET_FALSE_PATH
%token CMD_SET_MAX_DELAY
%token CMD_SET_MULTICYCLE_PATH
%token CMD_SET_INPUT_DELAY
%token CMD_SET_OUTPUT_DELAY

%token CMD_GET_PORTS
%token CMD_GET_CLOCKS

%token ARG_PERIOD
%token ARG_WAVEFORM
%token ARG_NAME
%token ARG_EXCLUSIVE
%token ARG_GROUP
%token ARG_FROM
%token ARG_TO
%token ARG_SETUP
%token ARG_CLOCK
%token ARG_MAX

/* declare variable tokens */
%token <strVal> BARE_STRING
%token <strVal> BARE_CHAR
%token <floatVal> BARE_NUMBER

/* declare types */
%type <strVal> string
%type <floatVal> number

%type <sdc_commands> sdc_commands
%type <create_clock> cmd_create_clock
%type <set_io_delay> cmd_set_input_delay cmd_set_output_delay
%type <set_clock_groups> cmd_set_clock_groups
%type <set_false_path> cmd_set_false_path
%type <set_max_delay> cmd_set_max_delay
%type <set_multicycle_path> cmd_set_multicycle_path

%type <string_group> stringGroup
%type <port_group> cmd_get_ports
%type <clock_group> cmd_get_clocks

/* Top level rule */
%start sdc_commands

%%
sdc_commands: /*empty*/                      { $$ = alloc_sdc_commands(); }
    | sdc_commands cmd_create_clock          { $$ = add_sdc_create_clock($1, $2); }
    | sdc_commands cmd_set_input_delay       { $$ = add_sdc_set_io_delay($1, $2); }
    | sdc_commands cmd_set_output_delay      { $$ = add_sdc_set_io_delay($1, $2); }
    | sdc_commands cmd_set_clock_groups      { $$ = add_sdc_set_clock_groups($1, $2); }
    | sdc_commands cmd_set_false_path        { $$ = add_sdc_set_false_path($1, $2); }
    | sdc_commands cmd_set_max_delay         { $$ = add_sdc_set_max_delay($1, $2); }
    | sdc_commands cmd_set_multicycle_path   { $$ = add_sdc_set_multicycle_path($1, $2); }
    ;

cmd_create_clock: CMD_CREATE_CLOCK                          { $$ = alloc_sdc_create_clock(); }
    | cmd_create_clock ARG_PERIOD number                    { $$ = sdc_create_clock_set_period($1, $3); }
    | cmd_create_clock ARG_NAME string                      { $$ = sdc_create_clock_set_name($1, $3); }
    | cmd_create_clock ARG_WAVEFORM '{' number number '}'   { $$ = sdc_create_clock_set_waveform($1, $4, $5); }
    | cmd_create_clock string                               { $$ = sdc_create_clock_set_target($1, $2); }
    ;

cmd_set_input_delay: CMD_SET_INPUT_DELAY        { $$ = alloc_sdc_set_io_delay(INPUT_DELAY); }
    | cmd_set_input_delay ARG_CLOCK string      { $$ = sdc_set_io_delay_set_clock($1, $3); }
    | cmd_set_input_delay ARG_MAX number        { $$ = sdc_set_io_delay_set_max_value($1, $3); }
    | cmd_set_input_delay '[' cmd_get_ports ']' { $$ = sdc_set_io_delay_set_ports($1, $3); }
    ;

cmd_set_output_delay: CMD_SET_OUTPUT_DELAY       { $$ = alloc_sdc_set_io_delay(OUTPUT_DELAY); }
    | cmd_set_output_delay ARG_CLOCK string      { $$ = sdc_set_io_delay_set_clock($1, $3); }
    | cmd_set_output_delay ARG_MAX number        { $$ = sdc_set_io_delay_set_max_value($1, $3); }
    | cmd_set_output_delay '[' cmd_get_ports ']' { $$ = sdc_set_io_delay_set_ports($1, $3); }
    ;

cmd_set_clock_groups: CMD_SET_CLOCK_GROUPS                  { $$ = alloc_sdc_set_clock_groups(); }
    | cmd_set_clock_groups ARG_EXCLUSIVE                    { $$ = sdc_set_clock_groups_set_type($1, CG_EXCLUSIVE); }
    | cmd_set_clock_groups ARG_GROUP '{' stringGroup '}'    { 
                                                              //Convert the stringGroup in to a clock group
                                                              t_sdc_clock_group* tmp_clock_group = alloc_sdc_get_clocks();
                                                              tmp_clock_group = sdc_get_clocks_add_clocks(tmp_clock_group, $4);

                                                              //Add the clock group to the set_clock_groups command
                                                              $$ = sdc_set_clock_groups_add_group($1, tmp_clock_group);

                                                              //Clean-up temporaries
                                                              free_sdc_clock_group(tmp_clock_group);
                                                            }
    | cmd_set_clock_groups ARG_GROUP string                 { 
                                                              //Convert the stringGroup in to a clock group
                                                              t_sdc_clock_group* tmp_clock_group = alloc_sdc_get_clocks();
                                                              tmp_clock_group = sdc_get_clocks_add_clock(tmp_clock_group, $3);

                                                              //Add the clock group to the set_clock_groups command
                                                              $$ = sdc_set_clock_groups_add_group($1, tmp_clock_group);

                                                              //Clean-up temporaries
                                                              free_sdc_clock_group(tmp_clock_group);
                                                            }
    | cmd_set_clock_groups ARG_GROUP '[' cmd_get_clocks ']' { $$ = sdc_set_clock_groups_add_group($1, $4); }
    ;

cmd_set_false_path: CMD_SET_FALSE_PATH                      { $$ = alloc_sdc_set_false_path(); }
    | cmd_set_false_path ARG_FROM '[' cmd_get_clocks ']'    { $$ = sdc_set_false_path_add_group($1, $4, FROM); }
    | cmd_set_false_path ARG_TO '[' cmd_get_clocks ']'      { $$ = sdc_set_false_path_add_group($1, $4, TO); }
    ;

cmd_set_max_delay: CMD_SET_MAX_DELAY                        { $$ = alloc_sdc_set_max_delay(); }
    | cmd_set_max_delay number                              { $$ = sdc_set_max_delay_set_max_delay($1, $2); }
    | cmd_set_max_delay ARG_FROM '[' cmd_get_clocks ']'     { $$ = sdc_set_max_delay_add_group($1, $4, FROM); }
    | cmd_set_max_delay ARG_TO '[' cmd_get_clocks ']'       { $$ = sdc_set_max_delay_add_group($1, $4, TO); }
    ;

cmd_set_multicycle_path: CMD_SET_MULTICYCLE_PATH                { $$ = alloc_sdc_set_multicycle_path(); }
    | cmd_set_multicycle_path number                            { $$ = sdc_set_multicycle_path_set_mcp_value($1, $2); }
    | cmd_set_multicycle_path ARG_SETUP                         { $$ = sdc_set_multicycle_path_set_type($1, MCP_SETUP); }
    | cmd_set_multicycle_path ARG_FROM '[' cmd_get_clocks ']'   { $$ = sdc_set_multicycle_path_add_group($1, $4, FROM); }
    | cmd_set_multicycle_path ARG_TO '[' cmd_get_clocks ']'     { $$ = sdc_set_multicycle_path_add_group($1, $4, TO); }
    ;

cmd_get_ports: CMD_GET_PORTS            { $$ = alloc_sdc_get_ports(); }
    | cmd_get_ports '{' stringGroup '}' { $$ = sdc_get_ports_add_ports($1, $3); }
    | cmd_get_ports string              { $$ = sdc_get_ports_add_port($1, $2); }
    ;

cmd_get_clocks: CMD_GET_CLOCKS              { $$ = alloc_sdc_get_clocks(); }
    | cmd_get_clocks '{' stringGroup '}'    { $$ = sdc_get_clocks_add_clocks($1, $3); }
    | cmd_get_clocks string                 { $$ = sdc_get_clocks_add_clock($1, $2); }
    ;

stringGroup: /*empty*/   { $$ = alloc_sdc_string_group(); }
    | stringGroup string { $$ = sdc_string_group_add_string($1, $2); } 

string: BARE_STRING         { $$ = $1; }
    | '"' BARE_STRING '"'   { $$ = $2; }
    | '\'' BARE_STRING '\'' { $$ = $2; }
    ;

number: BARE_NUMBER { $$ = $1; }
    ;

%%

int main(int argc, char **argv) {
    yyparse();

    for(int i = 0; i < g_sdc_commands->num_create_clock_cmds; i++) {
        t_sdc_create_clock* sdc_create_clock = g_sdc_commands->create_clock_cmds[i];
        if(sdc_create_clock->is_virtual) {
            printf("create_clock -period %f -waveform {%f %f} -name %s\n",
                    sdc_create_clock->period,
                    sdc_create_clock->rise_time,
                    sdc_create_clock->fall_time,
                    sdc_create_clock->name);
        } else {
            printf("create_clock -period %f -waveform {%f %f} %s\n",
                    sdc_create_clock->period,
                    sdc_create_clock->rise_time,
                    sdc_create_clock->fall_time,
                    sdc_create_clock->target);
        }
    }

    for(int i = 0; i < g_sdc_commands->num_set_input_delay_cmds; i++) {
        t_sdc_set_io_delay* sdc_set_input_delay = g_sdc_commands->set_input_delay_cmds[i];
        printf("set_input_delay -clock %s -max %f [get_ports {", 
                    sdc_set_input_delay->clock_name,
                    sdc_set_input_delay->max_delay);
        for(int j = 0; j < sdc_set_input_delay->target_ports->num_ports; j++) {
            printf("%s ", sdc_set_input_delay->target_ports->ports[j]);
        }
        printf("} ]\n");
    }

    for(int i = 0; i < g_sdc_commands->num_set_output_delay_cmds; i++) {
        t_sdc_set_io_delay* sdc_set_output_delay = g_sdc_commands->set_output_delay_cmds[i];
        printf("set_output_delay -clock %s -max %f [get_ports {", 
                    sdc_set_output_delay->clock_name,
                    sdc_set_output_delay->max_delay);
        for(int j = 0; j < sdc_set_output_delay->target_ports->num_ports; j++) {
            printf("%s ", sdc_set_output_delay->target_ports->ports[j]);
        }
        printf("} ]\n");
    }

    for(int i = 0; i < g_sdc_commands->num_set_clock_groups_cmds; i++) {
        t_sdc_set_clock_groups* sdc_set_clock_groups = g_sdc_commands->set_clock_groups_cmds[i];
        printf("set_clock_groups -exclusive");
        for(int j = 0; j < sdc_set_clock_groups->num_clock_groups; j++) {
            t_sdc_clock_group* sdc_clock_group = sdc_set_clock_groups->clock_groups[j];
            printf(" -group {");
            for(int k = 0; k < sdc_clock_group->num_clocks; k++) {
                printf("%s ", sdc_clock_group->clocks[k]);
            }
            printf("}");
        }
        printf("\n");
    }

    for(int i = 0; i < g_sdc_commands->num_set_false_path_cmds; i++) {
        t_sdc_set_false_path* sdc_set_false_path = g_sdc_commands->set_false_path_cmds[i];
        printf("set_false_path -from [get_clocks {");
        for(int j = 0; j < sdc_set_false_path->from_clocks->num_clocks; j++) {
            printf("%s ", sdc_set_false_path->from_clocks->clocks[j]);
        }
        printf("}] -to [get_clocks {");
        for(int j = 0; j < sdc_set_false_path->to_clocks->num_clocks; j++) {
            printf("%s ", sdc_set_false_path->to_clocks->clocks[j]);
        }
        printf("}]\n");
    }

    for(int i = 0; i < g_sdc_commands->num_set_max_delay_cmds; i++) {
        t_sdc_set_max_delay* sdc_set_max_delay = g_sdc_commands->set_max_delay_cmds[i];
        printf("set_max_delay %f -from [get_clocks {", sdc_set_max_delay->max_delay);
        for(int j = 0; j < sdc_set_max_delay->from_clocks->num_clocks; j++) {
            printf("%s ", sdc_set_max_delay->from_clocks->clocks[j]);
        }
        printf("}] -to [get_clocks {");
        for(int j = 0; j < sdc_set_max_delay->to_clocks->num_clocks; j++) {
            printf("%s ", sdc_set_max_delay->to_clocks->clocks[j]);
        }
        printf("}]\n");
    }

    for(int i = 0; i < g_sdc_commands->num_set_multicycle_path_cmds; i++) {
        t_sdc_set_multicycle_path* sdc_set_multicycle_path = g_sdc_commands->set_multicycle_path_cmds[i];
        printf("set_multicycle_path %f -setup -from [get_clocks {", sdc_set_multicycle_path->mcp_value);
        for(int j = 0; j < sdc_set_multicycle_path->from_clocks->num_clocks; j++) {
            printf("%s ", sdc_set_multicycle_path->from_clocks->clocks[j]);
        }
        printf("}] -to [get_clocks {");
        for(int j = 0; j < sdc_set_multicycle_path->to_clocks->num_clocks; j++) {
            printf("%s ", sdc_set_multicycle_path->to_clocks->clocks[j]);
        }
        printf("}]\n");
    }

    return 0;
}

int yyerror(const char *msg) {
    fprintf(stderr, "%s at line %d: %s\n", msg, yylineno, yytext);
    return 1;
}
