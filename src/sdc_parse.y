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
    t_sdc_set_input_delay* set_input_delay;
    t_sdc_set_output_delay* set_output_delay;

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
%type <set_input_delay> cmd_set_input_delay
%type <set_output_delay> cmd_set_output_delay

%type <string_group> stringGroup
%type <port_group> cmd_get_ports
%type <clock_group> cmd_get_clocks

%start sdc_commands

%%
sdc_commands: /*empty*/                      { $$ = alloc_sdc_commands(); }
    | sdc_commands cmd_create_clock          { $$ = add_sdc_create_clock($1, $2); }
    | sdc_commands cmd_set_input_delay       { $$ = add_sdc_set_input_delay($1, $2); }
    | sdc_commands cmd_set_output_delay      { $$ = add_sdc_set_output_delay($1, $2); }
    | sdc_commands cmd_set_clock_groups      {}
    | sdc_commands cmd_set_false_path        {}
    | sdc_commands cmd_set_max_delay         {}
    | sdc_commands cmd_set_multicycle_path   {}
    ;

cmd_create_clock: CMD_CREATE_CLOCK                          { printf("P: create_clock\n"); $$ = alloc_sdc_create_clock(); }
    | cmd_create_clock ARG_PERIOD number                    { printf("P:\t-period %f\n", $3); $$ = sdc_create_clock_set_period($1, $3); }
    | cmd_create_clock ARG_NAME string                      { printf("P:\t-name %s\n", $3); $$ = sdc_create_clock_set_name($1, $3); }
    | cmd_create_clock ARG_WAVEFORM '{' number number '}'   { printf("P:\t-waveform %f %f\n", $4, $5); $$ = sdc_create_clock_set_waveform($1, $4, $5); }
    | cmd_create_clock string                               { printf("P:\t target %s\n", $2); $$ = sdc_create_clock_set_target($1, $2); }
    ;

cmd_set_input_delay: CMD_SET_INPUT_DELAY        { printf("P: set_input_delay\n"); $$ = alloc_sdc_set_input_delay(); }
    | cmd_set_input_delay ARG_CLOCK string      { printf("P:\t-clock %s\n", $3); $$ = sdc_set_input_delay_set_clock($1, $3); }
    | cmd_set_input_delay ARG_MAX number        { printf("P:\t-max %f\n", $3); $$ = sdc_set_input_delay_set_max_delay($1, $3); }
    | cmd_set_input_delay '[' cmd_get_ports ']' { printf("P:\tget_ports ?\n"); $$ = sdc_set_input_delay_set_ports($1, $3); }
    ;

cmd_set_output_delay: CMD_SET_OUTPUT_DELAY       { printf("P: set_output_delay\n");  $$ = alloc_sdc_set_output_delay(); }
    | cmd_set_output_delay ARG_CLOCK string      { printf("P:\t-clock %s\n", $3); $$ = sdc_set_output_delay_set_clock($1, $3); }
    | cmd_set_output_delay ARG_MAX number        { printf("P:\t-max %f\n", $3); $$ = sdc_set_output_delay_set_max_delay($1, $3); }
    | cmd_set_output_delay '[' cmd_get_ports ']' { printf("P:\tget_ports ?\n"); $$ = sdc_set_output_delay_set_ports($1, $3); }
    ;

cmd_set_clock_groups: CMD_SET_CLOCK_GROUPS                  { printf("P: set_clock_groups\n"); }
    | cmd_set_clock_groups ARG_EXCLUSIVE                    { printf("P:\t-exclusive\n"); }
    | cmd_set_clock_groups ARG_GROUP '{' stringGroup '}'     { printf("P:\t-group ?\n"); }
    | cmd_set_clock_groups ARG_GROUP string                 { printf("P:\t-group %s\n", $3); }
    ;

cmd_set_false_path: CMD_SET_FALSE_PATH                      { printf("P: set_false_path\n"); }
    | cmd_set_false_path ARG_FROM '[' cmd_get_clocks ']'    { printf("P:\t-from ?\n"); }
    | cmd_set_false_path ARG_TO '[' cmd_get_clocks ']'      { printf("P:\t-to ?\n"); }
    ;

cmd_set_max_delay: CMD_SET_MAX_DELAY                        { printf("P: set_max_delay\n"); }
    | cmd_set_max_delay number                              { printf("P: delay %f\n", $2); }
    | cmd_set_max_delay ARG_FROM '[' cmd_get_clocks ']'     { printf("P:\t-from ?\n"); }
    | cmd_set_max_delay ARG_TO '[' cmd_get_clocks ']'       { printf("P:\t-to ?\n"); }
    ;

cmd_set_multicycle_path: CMD_SET_MULTICYCLE_PATH                { printf("P: set_multicycle_path\n"); }
    | cmd_set_multicycle_path number                            { printf("P:\tmcp_val %f\n", $2); }
    | cmd_set_multicycle_path ARG_SETUP                         { printf("P:\t-setup\n"); }
    | cmd_set_multicycle_path ARG_FROM '[' cmd_get_clocks ']'   { printf("P:\t-from ?\n"); }
    | cmd_set_multicycle_path ARG_TO '[' cmd_get_clocks ']'     { printf("P:\t-to ?\n"); }
    ;

cmd_get_ports: CMD_GET_PORTS            { printf("P: get_ports\n"); $$ = alloc_sdc_get_ports(); }
    | cmd_get_ports '{' stringGroup '}' { printf("P:\ttarget ?\n"); $$ = sdc_get_ports_add_ports($1, $3); }
    | cmd_get_ports string              { printf("P:\ttarget ?\n"); $$ = sdc_get_ports_add_port($1, $2); }
    ;

cmd_get_clocks: CMD_GET_CLOCKS              { printf("P: get_clocks\n"); $$ = alloc_sdc_get_clocks(); }
    | cmd_get_clocks '{' stringGroup '}'    { printf("P:\ttargets ?\n"); $$ = sdc_get_clocks_add_clocks($1, $3); }
    | cmd_get_clocks string                 { printf("P:\ttargets ?\n"); $$ = sdc_get_clocks_add_clock($1, $2); }
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
        t_sdc_set_input_delay* sdc_set_input_delay = g_sdc_commands->set_input_delay_cmds[i];
        printf("set_input_delay -clock %s -max %f [get_ports {", 
                    sdc_set_input_delay->clock_name,
                    sdc_set_input_delay->max_delay);
        for(int j = 0; j < sdc_set_input_delay->target_ports->num_ports; j++) {
            printf("%s ", sdc_set_input_delay->target_ports->ports[j]);
        }
        printf("} ]\n");
    }

    for(int i = 0; i < g_sdc_commands->num_set_output_delay_cmds; i++) {
        t_sdc_set_output_delay* sdc_set_output_delay = g_sdc_commands->set_output_delay_cmds[i];
        printf("set_output_delay -clock %s -max %f [get_ports {", 
                    sdc_set_output_delay->clock_name,
                    sdc_set_output_delay->max_delay);
        for(int j = 0; j < sdc_set_output_delay->target_ports->num_ports; j++) {
            printf("%s ", sdc_set_output_delay->target_ports->ports[j]);
        }
        printf("} ]\n");
    }
    return 0;
}

int yyerror(const char *msg) {
    fprintf(stderr, "%s at line %d: %s\n", msg, yylineno, yytext);
    return 1;
}
