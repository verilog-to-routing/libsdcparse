/* C++ parsers require Bison 3 */
%require "3.0"
%language "C++"

/* Write-out tokens header file */
%defines

/* Use Bison's 'variant' to store values. 
 * This allows us to use non POD types (e.g.
 * with constructors/destrictors), which is
 * not possible with the default mode which
 * uses unions.
 */
%define api.value.type variant

/* 
 * Use the 'complete' symbol type (i.e. variant)
 * in the lexer
 */
%define api.token.constructor

/*
 * Add a prefix the make_* functions used to
 * create the symbols
 */
%define api.token.prefix {TOKEN_}

/* Wrap everything in our namespace */
%define api.namespace {sdcparse}

%define parser_class_name {Parser}

%define api.prefix {sdcparse_}

/* Extra checks for correct usage */
%define parse.assert

/* Enable debugging info */
%define parse.trace

/* Better error reporting */
%define parse.error verbose

/* 
 * Fixes inaccuracy in verbose error reporting.
 * May be slow for some grammars.
 */
/*%define parse.lac full*/

/* Track locations */
/*%locations*/

/* Generate a table of token names */
%token-table

%code requires {
    #ifndef YY_NULLPTR
    #define YY_NULLPTR nullptr
    #endif

    #define YY_DECL sdcparse::Parser::symbol_type sdcparse_lex(void)
}

%{

#include <stdio.h>
#include "assert.h"

#include "sdc.h"
#include "sdc_common.h"

#include "sdc_parser.gen.h"
YY_DECL;

using namespace sdcparse;

%}

/* declare constant */
%token CMD_CREATE_CLOCK "create_clock"
%token CMD_SET_CLOCK_GROUPS "set_clock_groups"
%token CMD_SET_FALSE_PATH "set_false_path"
%token CMD_SET_MAX_DELAY "set_max_delay"
%token CMD_SET_MULTICYCLE_PATH "set_multicycle_path"
%token CMD_SET_INPUT_DELAY "set_input_delay"
%token CMD_SET_OUTPUT_DELAY "set_output_delay"

%token CMD_GET_PORTS "get_ports"
%token CMD_GET_CLOCKS "get_clocks"

%token ARG_PERIOD "-period"
%token ARG_WAVEFORM "-waveform"
%token ARG_NAME "-name" 
%token ARG_EXCLUSIVE "-exclusive"
%token ARG_GROUP "-group"
%token ARG_FROM "-from"
%token ARG_TO "-to"
%token ARG_SETUP "-setup"
%token ARG_CLOCK "-clock"
%token ARG_MAX "-max"

%token LSPAR "["
%token RSPAR "]"
%token LCPAR "{"
%token RCPAR "}"

%token EOL "end-of-line"
%token EOF 0 "end-of-file"

/* declare variable tokens */
%token <std::string> STRING
%token <std::string> ESCAPED_STRING
%token <std::string> CHAR
%token <float> FLOAT_NUMBER
%token <int> INT_NUMBER

/* declare types */
%type <std::string> string
%type <float> number
%type <float> float_number
%type <int> int_number

%type <std::shared_ptr<SdcCommands>> sdc_commands
%type <std::shared_ptr<CreateClock>> cmd_create_clock
%type <std::shared_ptr<SetIoDelay>> cmd_set_input_delay cmd_set_output_delay
%type <std::shared_ptr<SetClockGroups>> cmd_set_clock_groups
%type <std::shared_ptr<SetFalsePath>> cmd_set_false_path
%type <std::shared_ptr<SetMaxDelay>> cmd_set_max_delay
%type <std::shared_ptr<SetMulticyclePath>> cmd_set_multicycle_path

%type <std::shared_ptr<StringGroup>> stringGroup cmd_get_ports cmd_get_clocks

/* Top level rule */
%start sdc_commands

%%

sdc_commands:                                   { g_sdc_commands = alloc_sdc_commands(); $$ = g_sdc_commands; }
    | sdc_commands cmd_create_clock EOL         { $$ = add_sdc_create_clock($1, $2); }
    | sdc_commands cmd_set_input_delay EOL      { $$ = add_sdc_set_io_delay($1, $2); }
    | sdc_commands cmd_set_output_delay EOL     { $$ = add_sdc_set_io_delay($1, $2); }
    | sdc_commands cmd_set_clock_groups EOL     { $$ = add_sdc_set_clock_groups($1, $2); }
    | sdc_commands cmd_set_false_path EOL       { $$ = add_sdc_set_false_path($1, $2); }
    | sdc_commands cmd_set_max_delay EOL        { $$ = add_sdc_set_max_delay($1, $2); }
    | sdc_commands cmd_set_multicycle_path EOL  { $$ = add_sdc_set_multicycle_path($1, $2); }
    | sdc_commands EOL                          { /* Eat stray EOL symbols */ $$ = $1; }
    ;

cmd_create_clock: CMD_CREATE_CLOCK                          { $$ = alloc_sdc_create_clock(); }
    | cmd_create_clock ARG_PERIOD number                    { $$ = sdc_create_clock_set_period($1, $3); }
    | cmd_create_clock ARG_NAME string                      { $$ = sdc_create_clock_set_name($1, $3); }
    | cmd_create_clock ARG_WAVEFORM LCPAR number number RCPAR   { $$ = sdc_create_clock_set_waveform($1, $4, $5); }
    | cmd_create_clock LCPAR stringGroup RCPAR                  { $$ = sdc_create_clock_add_targets($1, $3); 
                                                            }
    | cmd_create_clock string                               { $$ = sdc_create_clock_add_targets($1, 
                                                                        make_sdc_string_group(sdcparse::StringGroupType::STRING, $2)); 
                                                            }
    ;

cmd_set_input_delay: CMD_SET_INPUT_DELAY        { $$ = alloc_sdc_set_io_delay(IoDelayType::INPUT); }
    | cmd_set_input_delay ARG_CLOCK string      { $$ = sdc_set_io_delay_set_clock($1, $3); }
    | cmd_set_input_delay ARG_MAX number        { $$ = sdc_set_io_delay_set_max_value($1, $3); }
    | cmd_set_input_delay LSPAR cmd_get_ports RSPAR { $$ = sdc_set_io_delay_set_ports($1, $3); }
    ;

cmd_set_output_delay: CMD_SET_OUTPUT_DELAY       { $$ = alloc_sdc_set_io_delay(IoDelayType::OUTPUT); }
    | cmd_set_output_delay ARG_CLOCK string      { $$ = sdc_set_io_delay_set_clock($1, $3); }
    | cmd_set_output_delay ARG_MAX number        { $$ = sdc_set_io_delay_set_max_value($1, $3); }
    | cmd_set_output_delay LSPAR cmd_get_ports RSPAR { $$ = sdc_set_io_delay_set_ports($1, $3); }
    ;

cmd_set_clock_groups: CMD_SET_CLOCK_GROUPS                  { $$ = alloc_sdc_set_clock_groups(); }
    | cmd_set_clock_groups ARG_EXCLUSIVE                    { $$ = sdc_set_clock_groups_set_type($1, ClockGroupsType::EXCLUSIVE); }
    | cmd_set_clock_groups ARG_GROUP LSPAR cmd_get_clocks RSPAR { $$ = sdc_set_clock_groups_add_group($1, $4); }
    | cmd_set_clock_groups ARG_GROUP LCPAR stringGroup RCPAR    { $$ = sdc_set_clock_groups_add_group($1, $4); }
    | cmd_set_clock_groups ARG_GROUP     string             { $$ = sdc_set_clock_groups_add_group($1, 
                                                                    make_sdc_string_group(sdcparse::StringGroupType::STRING, $3)); 
                                                            }
    ;

cmd_set_false_path: CMD_SET_FALSE_PATH                      { $$ = alloc_sdc_set_false_path(); }
    | cmd_set_false_path ARG_FROM LSPAR cmd_get_clocks RSPAR    { $$ = sdc_set_false_path_add_to_from_group($1, $4, FromToType::FROM); }
    | cmd_set_false_path ARG_TO   LSPAR cmd_get_clocks RSPAR    { $$ = sdc_set_false_path_add_to_from_group($1, $4, FromToType::TO  ); }
    | cmd_set_false_path ARG_FROM LCPAR stringGroup RCPAR       { $$ = sdc_set_false_path_add_to_from_group($1, $4, FromToType::FROM); }
    | cmd_set_false_path ARG_TO   LCPAR stringGroup RCPAR       { $$ = sdc_set_false_path_add_to_from_group($1, $4, FromToType::TO  ); }
    | cmd_set_false_path ARG_FROM     string                { $$ = sdc_set_false_path_add_to_from_group($1, 
                                                                    make_sdc_string_group(sdcparse::StringGroupType::STRING, $3), 
                                                                    FromToType::FROM); 
                                                            }
    | cmd_set_false_path ARG_TO       string                { $$ = sdc_set_false_path_add_to_from_group($1, 
                                                                    make_sdc_string_group(sdcparse::StringGroupType::STRING, $3), 
                                                                    FromToType::TO  ); 
                                                            }
    ;

cmd_set_max_delay: CMD_SET_MAX_DELAY                        { $$ = alloc_sdc_set_max_delay(); }
    | cmd_set_max_delay number                              { $$ = sdc_set_max_delay_set_max_delay_value($1, $2); }
    | cmd_set_max_delay ARG_FROM LSPAR cmd_get_clocks RSPAR     { $$ = sdc_set_max_delay_add_to_from_group($1, $4, FromToType::FROM); }
    | cmd_set_max_delay ARG_TO   LSPAR cmd_get_clocks RSPAR     { $$ = sdc_set_max_delay_add_to_from_group($1, $4, FromToType::TO  ); }
    | cmd_set_max_delay ARG_FROM LCPAR stringGroup RCPAR        { $$ = sdc_set_max_delay_add_to_from_group($1, $4, FromToType::FROM); }
    | cmd_set_max_delay ARG_TO   LCPAR stringGroup RCPAR        { $$ = sdc_set_max_delay_add_to_from_group($1, $4, FromToType::TO  ); }
    | cmd_set_max_delay ARG_FROM     string                 { $$ = sdc_set_max_delay_add_to_from_group($1, 
                                                                    make_sdc_string_group(sdcparse::StringGroupType::STRING, $3), 
                                                                    FromToType::FROM);
                                                            }
    | cmd_set_max_delay ARG_TO       string                 { $$ = sdc_set_max_delay_add_to_from_group($1, 
                                                                    make_sdc_string_group(sdcparse::StringGroupType::STRING, $3),
                                                                    FromToType::TO);
                                                            }
    ;

cmd_set_multicycle_path: CMD_SET_MULTICYCLE_PATH                { $$ = alloc_sdc_set_multicycle_path(); }
    | cmd_set_multicycle_path int_number                        { $$ = sdc_set_multicycle_path_set_mcp_value($1, $2); }
    | cmd_set_multicycle_path ARG_SETUP                         { $$ = sdc_set_multicycle_path_set_type($1, McpType::SETUP); }
    | cmd_set_multicycle_path ARG_FROM LSPAR cmd_get_clocks RSPAR   { $$ = sdc_set_multicycle_path_add_to_from_group($1, $4, FromToType::FROM); }
    | cmd_set_multicycle_path ARG_TO   LSPAR cmd_get_clocks RSPAR   { $$ = sdc_set_multicycle_path_add_to_from_group($1, $4, FromToType::TO); }
    | cmd_set_multicycle_path ARG_FROM LCPAR stringGroup RCPAR      { $$ = sdc_set_multicycle_path_add_to_from_group($1, $4, FromToType::FROM); }
    | cmd_set_multicycle_path ARG_TO   LCPAR stringGroup RCPAR      { $$ = sdc_set_multicycle_path_add_to_from_group($1, $4, FromToType::TO); }
    | cmd_set_multicycle_path ARG_FROM     string               { $$ = sdc_set_multicycle_path_add_to_from_group($1, 
                                                                        make_sdc_string_group(sdcparse::StringGroupType::STRING, $3), 
                                                                        FromToType::FROM);
                                                                }
    | cmd_set_multicycle_path ARG_TO       string               { $$ = sdc_set_multicycle_path_add_to_from_group($1, 
                                                                        make_sdc_string_group(sdcparse::StringGroupType::STRING, $3),
                                                                        FromToType::TO);
                                                                }
    ;

cmd_get_ports: CMD_GET_PORTS            { $$ = alloc_sdc_string_group(sdcparse::StringGroupType::PORT); }
    | cmd_get_ports LCPAR stringGroup RCPAR { $$ = sdc_string_group_add_strings($1, $3); }
    | cmd_get_ports string              { $$ = sdc_string_group_add_string($1, $2); }
    ;

cmd_get_clocks: CMD_GET_CLOCKS              { $$ = alloc_sdc_string_group(sdcparse::StringGroupType::CLOCK); }
    | cmd_get_clocks LCPAR stringGroup RCPAR    { $$ = sdc_string_group_add_strings($1, $3); }
    | cmd_get_clocks string                 { $$ = sdc_string_group_add_string($1, $2); }
    ;

stringGroup: /*empty*/   { $$ = alloc_sdc_string_group(sdcparse::StringGroupType::STRING); }
    | stringGroup string { $$ = sdc_string_group_add_string($1, $2); } 

string: STRING         { $$ = $1; }
    | ESCAPED_STRING        { $$ = $1; }
    ;

number: float_number { $$ = $1; }
    | int_number { $$ = $1; }
    ;

float_number: FLOAT_NUMBER { $$ = $1; }
    ;

int_number: INT_NUMBER { $$ = $1; }
    ;

%%


void sdcparse::Parser::error(const std::string& msg) {
    sdc_error(sdcparse_lineno, sdcparse_text, "Error: %s.\n", msg.c_str());
}
