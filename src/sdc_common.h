#ifndef SDC_COMMON_H
#define SDC_COMMON_H

#include "sdc.h"

/*
 * Parser info
 */
extern int yylineno;
extern char* yytext;

namespace sdcparse {

extern std::shared_ptr<SdcCommands> g_sdc_commands;

/*
 * Function Declarations
 */

//SDC Command List manipulation
std::shared_ptr<SdcCommands> alloc_sdc_commands();

//create_clock
std::shared_ptr<CreateClock> alloc_sdc_create_clock();
std::shared_ptr<CreateClock> sdc_create_clock_set_period(std::shared_ptr<CreateClock> sdc_create_clock, double period);
std::shared_ptr<CreateClock> sdc_create_clock_set_name(std::shared_ptr<CreateClock> sdc_create_clock, const std::string& name);
std::shared_ptr<CreateClock> sdc_create_clock_set_waveform(std::shared_ptr<CreateClock> sdc_create_clock, double rise_time, double fall_time);
std::shared_ptr<CreateClock> sdc_create_clock_add_targets(std::shared_ptr<CreateClock> sdc_create_clock, std::shared_ptr<StringGroup> target_group);
std::shared_ptr<SdcCommands> add_sdc_create_clock(std::shared_ptr<SdcCommands> sdc_commands, std::shared_ptr<CreateClock> sdc_create_clock);

//set_input_delay & set_output_delay
std::shared_ptr<SetIoDelay> alloc_sdc_set_io_delay(IoDelayType cmd_type);
std::shared_ptr<SetIoDelay> sdc_set_io_delay_set_clock(std::shared_ptr<SetIoDelay> sdc_set_io_delay, const std::string& clock_name);
std::shared_ptr<SetIoDelay> sdc_set_io_delay_set_max_value(std::shared_ptr<SetIoDelay> sdc_set_io_delay, double max_value);
std::shared_ptr<SetIoDelay> sdc_set_io_delay_set_ports(std::shared_ptr<SetIoDelay> sdc_set_io_delay, std::shared_ptr<StringGroup> ports);
std::shared_ptr<SdcCommands> add_sdc_set_io_delay(std::shared_ptr<SdcCommands> sdc_commands, std::shared_ptr<SetIoDelay> sdc_set_io_delay);

//set_clock_groups
std::shared_ptr<SetClockGroups> alloc_sdc_set_clock_groups();
std::shared_ptr<SetClockGroups> sdc_set_clock_groups_set_type(std::shared_ptr<SetClockGroups> sdc_set_clock_groups, ClockGroupsType type);
std::shared_ptr<SetClockGroups> sdc_set_clock_groups_add_group(std::shared_ptr<SetClockGroups> sdc_set_clock_groups, std::shared_ptr<StringGroup> clock_group);
std::shared_ptr<SdcCommands> add_sdc_set_clock_groups(std::shared_ptr<SdcCommands> sdc_commands, std::shared_ptr<SetClockGroups> sdc_set_clock_groups);

//set_false_path
std::shared_ptr<SetFalsePath> alloc_sdc_set_false_path();
std::shared_ptr<SetFalsePath> sdc_set_false_path_add_to_from_group(std::shared_ptr<SetFalsePath> sdc_set_false_path, std::shared_ptr<StringGroup> group, FromToType to_from_dir);
std::shared_ptr<SdcCommands> add_sdc_set_false_path(std::shared_ptr<SdcCommands> sdc_commands, std::shared_ptr<SetFalsePath> sdc_set_false_path);

//set_max_delay
std::shared_ptr<SetMaxDelay> alloc_sdc_set_max_delay();
std::shared_ptr<SetMaxDelay> sdc_set_max_delay_set_max_delay_value(std::shared_ptr<SetMaxDelay> sdc_set_max_delay, double max_delay);
std::shared_ptr<SetMaxDelay> sdc_set_max_delay_add_to_from_group(std::shared_ptr<SetMaxDelay> sdc_set_max_delay, std::shared_ptr<StringGroup> group, FromToType to_from_dir);
std::shared_ptr<SdcCommands> add_sdc_set_max_delay(std::shared_ptr<SdcCommands> sdc_commands, std::shared_ptr<SetMaxDelay> sdc_set_max_delay);

//set_multicycle_path
std::shared_ptr<SetMulticyclePath> alloc_sdc_set_multicycle_path();
std::shared_ptr<SetMulticyclePath> sdc_set_multicycle_path_set_type(std::shared_ptr<SetMulticyclePath> sdc_set_multicycle_path, McpType type);
std::shared_ptr<SetMulticyclePath> sdc_set_multicycle_path_set_mcp_value(std::shared_ptr<SetMulticyclePath> sdc_set_multicycle_path, int mcp_value);
std::shared_ptr<SetMulticyclePath> sdc_set_multicycle_path_add_to_from_group(std::shared_ptr<SetMulticyclePath> sdc_set_multicycle_path, std::shared_ptr<StringGroup> group, FromToType to_from_dir);
std::shared_ptr<SdcCommands> add_sdc_set_multicycle_path(std::shared_ptr<SdcCommands> sdc_commands, std::shared_ptr<SetMulticyclePath> sdc_set_multicycle_path);


//string_group
std::shared_ptr<StringGroup> alloc_sdc_string_group(StringGroupType type);
std::shared_ptr<StringGroup> make_sdc_string_group(StringGroupType type, const std::string& string);
std::shared_ptr<StringGroup> duplicate_sdc_string_group(std::shared_ptr<StringGroup> string_group);
std::shared_ptr<StringGroup> sdc_string_group_add_string(std::shared_ptr<StringGroup> sdc_string_group, const std::string& string);
std::shared_ptr<StringGroup> sdc_string_group_add_strings(std::shared_ptr<StringGroup> sdc_string_group, std::shared_ptr<StringGroup> string_group_to_add);

//utility
char* strdup(const char* src);
char* strndup(const char* src, size_t len);

} //namespace
#endif
