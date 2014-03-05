/* recognize tokens for the calculator and print them out */
%{
/*
 * Include Files
 */
#include "sdc_common.h"
#include "sdc_parse.tab.h"
%}

/*
 * Options
 */
%option yylineno
%option noyywrap

/*
 * Symbol Definitions
 */
%%
"#".*                           { /* skip comments */ }
[\n\r]                          { /* skip EOL, (need \r to work with non-unix line endings) */ }
[ \t]+                          { /* skip white space */ }

"create_clock"                  { return CMD_CREATE_CLOCK; }
"set_input_delay"               { return CMD_SET_INPUT_DELAY; }
"set_output_delay"              { return CMD_SET_OUTPUT_DELAY; }
"set_clock_groups"              { return CMD_SET_CLOCK_GROUPS; }
"set_false_path"                { return CMD_SET_FALSE_PATH; }
"set_max_delay"                 { return CMD_SET_MAX_DELAY; }
"set_multicycle_path"           { return CMD_SET_MULTICYCLE_PATH; }

"get_ports"                     { return CMD_GET_PORTS; }
"get_clocks"                    { return CMD_GET_CLOCKS; }

"-period"                       { return ARG_PERIOD; }
"-name"                         { return ARG_NAME; }
"-waveform"                     { return ARG_WAVEFORM; }
"-clock"                        { return ARG_CLOCK; }
"-max"                          { return ARG_MAX; }
"-exclusive"                    { return ARG_EXCLUSIVE; }
"-group"                        { return ARG_GROUP; }
"-from"                         { return ARG_FROM; }
"-to"                           { return ARG_TO; }
"-setup"                        { return ARG_SETUP; }

[-+]?[0-9]+                     { yylval.intVal = atoi(yytext); return BARE_INT_NUMBER; }
[-+]?([0-9]*\.?[0-9]+|[0-9]+\.) { yylval.floatVal = atof(yytext); return BARE_FLOAT_NUMBER; }
[-a-zA-Z0-9_~|:*]*              { yylval.strVal = strdup(yytext); return BARE_STRING; }
.                               { return (int)(*yytext); /*Literal character to pass unrecognized stuff to parser (i.e. brackets) */ }
%%
