/* sdc_commands.i */
%module sdc_commands
%include "std_string.i"
%include "std_vector.i"
%{
#include "sdc_commands.h"
extern int Sdc_commands_SafeInit(Tcl_Interp* interp);
%}

// TODO: Investigate "wrapper". It may be able to cleanly inject tcl procedures.

namespace std {
    %template(StringVector) vector<std::string>;
    %template(DoubleVector) vector<double>;
    %template(IntVector) vector<int>;
}

void lineno_internal(int line_num);

// TODO: Update the prototype of this.
void create_clock_internal(double period,
                           const std::string& name,
                           const std::vector<double>& waveform,
                           bool add,
                           const std::vector<std::string>& targets);

void set_clock_groups_internal(const std::vector<std::string>& clock_list,
                               const std::vector<int>& clock_group_start_pos);

void set_input_delay_internal(bool max_delay_flag,
                              bool min_delay_flag,
                              const std::string& clock_name,
                              double delay,
                              const std::vector<std::string>& targets);

void set_output_delay_internal(bool max_delay_flag,
                               bool min_delay_flag,
                               const std::string& clock_name,
                               double delay,
                               const std::vector<std::string>& targets);

// TODO: Investigate if this is too poor for performance
std::vector<std::string> all_ports_internal();

std::vector<std::string> all_clocks_internal();

std::string get_name_internal(std::string object_id);

bool is_object_id_internal(std::string object_id);

// TODO: Add more commands.
