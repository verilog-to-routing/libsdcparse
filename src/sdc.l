/* recognize tokens for the calculator and print them out */
%{
/*
 * Include Files
 */
#include "sdc.tab.h"
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
"\n"                            { /* skip EOL */ }
[ \t]+                          { /* skip white space */ }

"create_clock"                  { printf("L: create_clock\n"); return CMD_CREATE_CLOCK; }

"-period"                       { printf("L: -period\n"); return ARG_PERIOD; }
"-name"                         { printf("L: -name\n"); return ARG_NAME; }

[-+]?([0-9]*\.?[0-9]+|[0-9]+\.) { printf("L: Number=%.2f\n", atof(yytext)); yylval.floatVal = atof(yytext); return BARE_NUMBER; }

[-a-zA-Z0-9_~|:*]*              { printf("L: Bare String %s\n", yytext); yylval.strVal = strdup(yytext); return BARE_STRING; }

.                               { return (int)(*yytext); }
%%
/*[^\s]+                          { printf("L: String\n"); yylval.strVal = strdup(yytext); return STRING; }*/
/*\"([^\s]+)\"                    { printf("L: Double Quoted String %s\n", yytext); }*/
/*'([^\s]+)'                      { printf("L: Single Quoted String %s\n", yytext); }*/
/*"{"([^\s]+)"}"                  { printf("L: Curly Brace Quoted String %s\n", yytext); }*/

/*
 *.                               { printf("L: Unrecognizec char: %c\n", *yytext); yylval.strVal = strdup(yytext); return BARE_CHAR; }
 */

/*
 *"set_clock_groups"              { printf("L: set_clock_groups\n"); return CMD_SET_CLOCK_GROUPS; }
 *"set_false_path"                { printf("L: set_false_path\n"); return CMD_SET_FALSE_PATH; }
 *"set_max_delay"                 { printf("L: set_max_delay\n"); return CMD_SET_MAX_DELAY; }
 *"set_multicycle_path"           { printf("L: set_multicycle_path\n"); return CMD_SET_MULTICYCLE_PATH; }
 *"set_input_delay"               { printf("L: set_input_delay\n"); return CMD_SET_INPUT_DELAY; }
 *"set_output_delay"              { printf("L: set_output_delay\n"); return CMD_SET_OUTPUT_DELAY; }
 *"get_ports"                     { printf("L: get_ports\n"); return CMD_GET_PORTS; }
 *"-waveform"                     { printf("L: -waveform\n"); return ARG_WAVEFORM; }
 *"-exclusive"                    { printf("L: -exclusive\n"); return ARG_EXCLUSIVE; }
 *"-group"                        { printf("L: -group\n"); return ARG_GROUP; }
 *"-from"                         { printf("L: -from\n"); return ARG_FROM; }
 *"-to"                           { printf("L: -to\n"); return ARG_TO; }
 *"-setup"                        { printf("L: -setup\n"); return ARG_SETUP; }
 *"-clock"                        { printf("L: -clock\n"); return ARG_CLOCK; }
 *"-max"                          { printf("L: -max\n"); return ARG_MAX; }
 */
