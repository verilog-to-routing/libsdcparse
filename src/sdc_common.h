#ifndef SDC_COMMON_H
#define SDC_COMMON_H

#include "sdc.h"

/*
 * Parser info
 */
extern int yylineno;
extern char* yytext;

namespace sdcparse {

extern SdcCommands* g_sdc_commands;

/*
 * Function Declarations
 */

//SDC Command List manipulation
SdcCommands* alloc_sdc_commands();
void free_sdc_commands(SdcCommands* sdc_commands);

//create_clock
CreateClock* alloc_sdc_create_clock();
void free_sdc_create_clock(CreateClock* sdc_create_clock);
CreateClock* sdc_create_clock_set_period(CreateClock* sdc_create_clock, double period);
CreateClock* sdc_create_clock_set_name(CreateClock* sdc_create_clock, char* name);
CreateClock* sdc_create_clock_set_waveform(CreateClock* sdc_create_clock, double rise_time, double fall_time);
CreateClock* sdc_create_clock_add_targets(CreateClock* sdc_create_clock, StringGroup* target_group);
SdcCommands* add_sdc_create_clock(SdcCommands* sdc_commands, CreateClock* sdc_create_clock);

//set_input_delay & set_output_delay
SetIoDelay* alloc_sdc_set_io_delay(IoDelayType cmd_type);
void free_sdc_set_io_delay(SetIoDelay* sdc_set_io_delay);
SetIoDelay* sdc_set_io_delay_set_clock(SetIoDelay* sdc_set_io_delay, char* clock_name);
SetIoDelay* sdc_set_io_delay_set_max_value(SetIoDelay* sdc_set_io_delay, double max_value);
SetIoDelay* sdc_set_io_delay_set_ports(SetIoDelay* sdc_set_io_delay, StringGroup* ports);
SdcCommands* add_sdc_set_io_delay(SdcCommands* sdc_commands, SetIoDelay* sdc_set_io_delay);

//set_clock_groups
SetClockGroups* alloc_sdc_set_clock_groups();
void free_sdc_set_clock_groups(SetClockGroups* sdc_set_clock_groups);
SetClockGroups* sdc_set_clock_groups_set_type(SetClockGroups* sdc_set_clock_groups, ClockGroupsType type);
SetClockGroups* sdc_set_clock_groups_add_group(SetClockGroups* sdc_set_clock_groups, StringGroup* clock_group);
SdcCommands* add_sdc_set_clock_groups(SdcCommands* sdc_commands, SetClockGroups* sdc_set_clock_groups);

//set_false_path
SetFalsePath* alloc_sdc_set_false_path();
void free_sdc_set_false_path(SetFalsePath* sdc_set_false_path);
SetFalsePath* sdc_set_false_path_add_to_from_group(SetFalsePath* sdc_set_false_path, StringGroup* group, FromToType to_from_dir);
SdcCommands* add_sdc_set_false_path(SdcCommands* sdc_commands, SetFalsePath* sdc_set_false_path);

//set_max_delay
SetMaxDelay* alloc_sdc_set_max_delay();
void free_sdc_set_max_delay(SetMaxDelay* sdc_set_max_delay);
SetMaxDelay* sdc_set_max_delay_set_max_delay_value(SetMaxDelay* sdc_set_max_delay, double max_delay);
SetMaxDelay* sdc_set_max_delay_add_to_from_group(SetMaxDelay* sdc_set_max_delay, StringGroup* group, FromToType to_from_dir);
SdcCommands* add_sdc_set_max_delay(SdcCommands* sdc_commands, SetMaxDelay* sdc_set_max_delay);

//set_multicycle_path
SetMulticyclePath* alloc_sdc_set_multicycle_path();
void free_sdc_set_multicycle_path(SetMulticyclePath* sdc_set_multicycle_path);
SetMulticyclePath* sdc_set_multicycle_path_set_type(SetMulticyclePath* sdc_set_multicycle_path, McpType type);
SetMulticyclePath* sdc_set_multicycle_path_set_mcp_value(SetMulticyclePath* sdc_set_multicycle_path, int mcp_value);
SetMulticyclePath* sdc_set_multicycle_path_add_to_from_group(SetMulticyclePath* sdc_set_multicycle_path, StringGroup* group, FromToType to_from_dir);
SdcCommands* add_sdc_set_multicycle_path(SdcCommands* sdc_commands, SetMulticyclePath* sdc_set_multicycle_path);


//string_group
StringGroup* alloc_sdc_string_group(StringGroupType type);
StringGroup* make_sdc_string_group(StringGroupType type, char* string);
StringGroup* duplicate_sdc_string_group(StringGroup* string_group);
void free_sdc_string_group(StringGroup* sdc_string_group);
StringGroup* sdc_string_group_add_string(StringGroup* sdc_string_group, const std::string& string);
StringGroup* sdc_string_group_add_strings(StringGroup* sdc_string_group, StringGroup* string_group_to_add);

//utility
char* strdup(const char* src);
char* strndup(const char* src, size_t len);

} //namespace
#endif
