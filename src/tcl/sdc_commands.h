#pragma once

// TODO: Move the interface declarations to another header file. That way we
//       can just include that header file in the .i file.

#include <string>
#include "sdcparse.hpp"

// TODO: Create a global variable to hold the callback.
//       It will be a pointer to the base callback class.

namespace sdcparse {

extern Callback* g_callback;

} // namespace sdcparse

void lineno_internal(int line_num);

void create_clock_internal(double period,
                           const std::string& name,
                           const std::vector<double>& waveform,
                           bool add,
                           const std::vector<std::string>& targets);

void set_clock_groups_internal(const std::vector<std::string>& clock_list,
                               const std::vector<int>& clock_group_start_pos);

void set_false_path_internal(const std::vector<std::string>& from_list,
                             const std::vector<std::string>& to_list);

void set_min_delay_internal(double delay,
                            const std::vector<std::string>& from_list,
                            const std::vector<std::string>& to_list);

void set_max_delay_internal(double delay,
                            const std::vector<std::string>& from_list,
                            const std::vector<std::string>& to_list);

void set_multicycle_path_internal(bool is_setup,
                                  bool is_hold,
                                  const std::vector<std::string>& from_list,
                                  const std::vector<std::string>& to_list,
                                  int path_multiplier);

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

void set_clock_uncertainty_internal(bool is_setup,
                                    bool is_hold,
                                    const std::vector<std::string>& from_list,
                                    const std::vector<std::string>& to_list,
                                    double uncertainty);

void set_clock_latency_internal(bool is_source,
                                bool is_early,
                                bool is_late,
                                double latency,
                                const std::vector<std::string>& targets);

void set_disable_timing_internal(const std::vector<std::string>& from_list,
                                 const std::vector<std::string>& to_list);

std::vector<std::string> all_ports_internal();

std::vector<std::string> all_clocks_internal();

std::vector<std::string> all_inputs_internal();

std::vector<std::string> all_outputs_internal();

std::vector<std::string> all_pins_internal();

std::vector<std::string> all_cells_internal();

std::string get_name_internal(std::string object_id);

bool is_object_id_internal(std::string object_id);

std::string _libsdcparse_create_port_internal(std::string port_name,
                                              std::string port_type_str);

std::string _libsdcparse_create_pin_internal(std::string pin_name);

std::string _libsdcparse_create_cell_internal(std::string cell_name);
