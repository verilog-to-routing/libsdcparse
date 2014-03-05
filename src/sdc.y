/* simplest version of calculator */
%{
#include <stdio.h>
extern int yylineno;
extern char* yytext;
%}

%union {
    char* strVal;
    double floatVal;
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
%type <strVal> string cmd_get_ports cmd_get_clocks cmd_set_output_delay
%type <floatVal> number


%%
cmdlist: /*empty*/
    | cmdlist cmd;

cmd: cmd_create_clock           {}
    | cmd_set_input_delay       {}
    | cmd_set_output_delay      {}
    | cmd_set_clock_groups      {}
    | cmd_set_false_path        {}
    | cmd_set_max_delay         {}
    | cmd_set_multicycle_path   {}
    ;

cmd_create_clock: CMD_CREATE_CLOCK                          { printf("P: create_clock\n"); }
    | cmd_create_clock ARG_PERIOD number                    { printf("P:\t-period %f\n", $3); }
    | cmd_create_clock ARG_NAME string                      { printf("P:\t-name %s\n", $3); }
    | cmd_create_clock ARG_WAVEFORM '{' number number '}'   { printf("P:\t-waveform %f %f\n", $4, $5); }
    | cmd_create_clock string                               { printf("P:\t target %s\n", $2); }
    ;

cmd_set_input_delay: CMD_SET_INPUT_DELAY        { printf("P: set_input_delay\n"); }
    | cmd_set_input_delay ARG_CLOCK string      { printf("P:\t-clock %s\n", $3); }
    | cmd_set_input_delay ARG_MAX number        { printf("P:\t-max %f\n", $3); }
    | cmd_set_input_delay '[' cmd_get_ports ']' { printf("P:\tget_ports ?\n"); }
    ;

cmd_set_output_delay: CMD_SET_OUTPUT_DELAY       { printf("P: set_output_delay\n"); }
    | cmd_set_output_delay ARG_CLOCK string      { printf("P:\t-clock %s\n", $3); }
    | cmd_set_output_delay ARG_MAX number        { printf("P:\t-max %f\n", $3); }
    | cmd_set_output_delay '[' cmd_get_ports ']' { printf("P:\tget_ports ?\n"); }
    ;

cmd_set_clock_groups: CMD_SET_CLOCK_GROUPS                  { printf("P: set_clock_groups\n"); }
    | cmd_set_clock_groups ARG_EXCLUSIVE                    { printf("P:\t-exclusive\n"); }
    | cmd_set_clock_groups ARG_GROUP '{' stringList '}'     { printf("P:\t-group ?\n"); }
    | cmd_set_clock_groups ARG_GROUP string                 { printf("P:\t-group %s\n", $3); }
    ;

cmd_set_false_path: CMD_SET_FALSE_PATH                      { printf("P: set_false_path\n"); }
    | cmd_set_false_path ARG_FROM '[' cmd_get_clocks ']'    { printf("P:\t-from ?\n"); }
    | cmd_set_false_path ARG_TO '[' cmd_get_clocks ']'      { printf("P:\t-to ?\n"); }
    ;

cmd_set_max_delay: CMD_SET_MAX_DELAY        { printf("P: set_max_delay\n"); }
    | cmd_set_max_delay number              { printf("P: delay %f\n", $2); }
    | cmd_set_max_delay ARG_FROM '[' cmd_get_clocks ']'    { printf("P:\t-from ?\n"); }
    | cmd_set_max_delay ARG_TO '[' cmd_get_clocks ']'      { printf("P:\t-to ?\n"); }
    ;

cmd_set_multicycle_path: CMD_SET_MULTICYCLE_PATH                { printf("P: set_multicycle_path\n"); }
    | cmd_set_multicycle_path number                            { printf("P:\tmcp_val %f\n", $2); }
    | cmd_set_multicycle_path ARG_SETUP                         { printf("P:\t-setup\n"); }
    | cmd_set_multicycle_path ARG_FROM '[' cmd_get_clocks ']'   { printf("P:\t-from ?\n"); }
    | cmd_set_multicycle_path ARG_TO '[' cmd_get_clocks ']'     { printf("P:\t-to ?\n"); }
    ;

cmd_get_ports: CMD_GET_PORTS            { printf("P: get_ports\n"); }
    | cmd_get_ports '{' stringList '}'  { printf("P:\ttarget ?\n");}
    | cmd_get_ports string              { printf("P:\ttarget ?\n");}
    ;

cmd_get_clocks: CMD_GET_CLOCKS          { printf("P: get_clocks\n"); }
    | cmd_get_clocks '{' stringList '}' { printf("P:\ttargets ?\n");}
    | cmd_get_clocks string             { printf("P:\ttargets ?\n");}
    ;

stringList: /*empty*/
    | stringList string             

string: BARE_STRING         { $$ = $1; }
    | '"' BARE_STRING '"'   { $$ = $2; }
    | '\'' BARE_STRING '\'' { $$ = $2; }
    ;

number: BARE_NUMBER { $$ = $1; }
    ;

%%

int main(int argc, char **argv) {
    yyparse();
    return 0;
}

int yyerror(char *msg) {
    fprintf(stderr, "%s at line %d: %s\n", msg, yylineno, yytext);
    return 1;
}
