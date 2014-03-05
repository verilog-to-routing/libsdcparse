#ifndef SDC_COMMON_H
#define SDC_COMMON_H

#include "sdc.h"

/*
 * Parser info
 */
extern int yylineno;
extern char* yytext;

extern t_sdc_commands* g_sdc_commands;

extern "C" {
    extern int yyparse();
    extern int yylex(void);
}

/*
 * Function Declarations
 */
//SDC Command List manipulation
t_sdc_commands* alloc_sdc_commands();
void free_sdc_commands(t_sdc_commands* sdc_commands);

//create_clock
t_sdc_create_clock* alloc_sdc_create_clock();
void free_sdc_create_clock(t_sdc_create_clock* sdc_create_clock);
t_sdc_create_clock* sdc_create_clock_set_period(t_sdc_create_clock* sdc_create_clock, double period);
t_sdc_create_clock* sdc_create_clock_set_name(t_sdc_create_clock* sdc_create_clock, char* name);
t_sdc_create_clock* sdc_create_clock_set_waveform(t_sdc_create_clock* sdc_create_clock, double rise_time, double fall_time);
t_sdc_create_clock* sdc_create_clock_set_target(t_sdc_create_clock* sdc_create_clock, char* target);
t_sdc_commands* add_sdc_create_clock(t_sdc_commands* sdc_commands, t_sdc_create_clock* sdc_create_clock);

//set_input_delay & set_output_delay
t_sdc_set_io_delay* alloc_sdc_set_io_delay(t_sdc_io_delay_type type);
void free_sdc_set_io_delay(t_sdc_set_io_delay* sdc_set_io_delay);
t_sdc_set_io_delay* sdc_set_io_delay_set_clock(t_sdc_set_io_delay* sdc_set_io_delay, char* clock_name);
t_sdc_set_io_delay* sdc_set_io_delay_set_max_value(t_sdc_set_io_delay* sdc_set_io_delay, double max_value);
t_sdc_set_io_delay* sdc_set_io_delay_set_ports(t_sdc_set_io_delay* sdc_set_io_delay, t_sdc_port_group* ports);
t_sdc_commands* add_sdc_set_io_delay(t_sdc_commands* sdc_commands, t_sdc_set_io_delay* sdc_set_io_delay);

//set_clock_groups
t_sdc_set_clock_groups* alloc_sdc_set_clock_groups();
void free_sdc_set_clock_groups(t_sdc_set_clock_groups* sdc_set_clock_groups);
t_sdc_set_clock_groups* sdc_set_clock_groups_set_type(t_sdc_set_clock_groups* sdc_set_clock_groups, t_sdc_clock_groups_type type);
t_sdc_set_clock_groups* sdc_set_clock_groups_add_group(t_sdc_set_clock_groups* sdc_set_clock_groups, t_sdc_clock_group* clock_group);
t_sdc_commands* add_sdc_set_clock_groups(t_sdc_commands* sdc_commands, t_sdc_set_clock_groups* sdc_set_clock_groups);

//set_false_path
t_sdc_set_false_path* alloc_sdc_set_false_path();
void free_sdc_set_false_path(t_sdc_set_false_path* sdc_set_false_path);
t_sdc_set_false_path* sdc_set_false_path_add_group(t_sdc_set_false_path* sdc_set_false_path, t_sdc_clock_group* clock_group, t_sdc_clock_group_dir clock_group_dir);
t_sdc_commands* add_sdc_set_false_path(t_sdc_commands* sdc_commands, t_sdc_set_false_path* sdc_set_false_path);

//set_max_delay
t_sdc_set_max_delay* alloc_sdc_set_max_delay();
void free_sdc_set_max_delay(t_sdc_set_max_delay* sdc_set_max_delay);
t_sdc_set_max_delay* sdc_set_max_delay_set_max_delay(t_sdc_set_max_delay* sdc_set_max_delay, double max_delay);
t_sdc_set_max_delay* sdc_set_max_delay_add_group(t_sdc_set_max_delay* sdc_set_max_delay, t_sdc_clock_group* clock_group, t_sdc_clock_group_dir clock_group_dir);
t_sdc_commands* add_sdc_set_max_delay(t_sdc_commands* sdc_commands, t_sdc_set_max_delay* sdc_set_max_delay);

//set_multicycle_path
t_sdc_set_multicycle_path* alloc_sdc_set_multicycle_path();
void free_sdc_set_multicycle_path(t_sdc_set_multicycle_path* sdc_set_multicycle_path);
t_sdc_set_multicycle_path* sdc_set_multicycle_path_set_type(t_sdc_set_multicycle_path* sdc_set_multicycle_path, t_sdc_mcp_type type);
t_sdc_set_multicycle_path* sdc_set_multicycle_path_set_mcp_value(t_sdc_set_multicycle_path* sdc_set_multicycle_path, double mcp_value);
t_sdc_set_multicycle_path* sdc_set_multicycle_path_add_group(t_sdc_set_multicycle_path* sdc_set_multicycle_path, t_sdc_clock_group* clock_group, t_sdc_clock_group_dir clock_group_dir);
t_sdc_commands* add_sdc_set_multicycle_path(t_sdc_commands* sdc_commands, t_sdc_set_multicycle_path* sdc_set_multicycle_path);



//get_ports
t_sdc_port_group* alloc_sdc_get_ports();
t_sdc_port_group* duplicate_port_group(t_sdc_port_group* port_group);
void free_sdc_port_group(t_sdc_port_group* sdc_port_group);
t_sdc_port_group* sdc_get_ports_add_ports(t_sdc_port_group* sdc_port_group, t_sdc_string_group* port_list);
t_sdc_port_group* sdc_get_ports_add_port(t_sdc_port_group* sdc_port_group, char* port_name);

//get_clocks
t_sdc_clock_group* alloc_sdc_get_clocks();
t_sdc_clock_group* duplicate_clock_group(t_sdc_clock_group* clock_group);
void free_sdc_clock_group(t_sdc_clock_group* sdc_clock_group);
t_sdc_clock_group* sdc_get_clocks_add_clocks(t_sdc_clock_group* sdc_clock_group, t_sdc_string_group* clock_list);
t_sdc_clock_group* sdc_get_clocks_add_clock(t_sdc_clock_group* sdc_clock_group, char* clock_name);

//string_group
t_sdc_string_group* alloc_sdc_string_group();
void free_sdc_string_group(t_sdc_string_group* sdc_string_group);
t_sdc_string_group* sdc_string_group_add_string(t_sdc_string_group* sdc_string_group, char* string);

#endif
