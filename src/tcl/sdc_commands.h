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

void _libsdcparse_lineno_internal(int line_num);

void _libsdcparse_create_clock_internal(double period,
                                        const std::string& name,
                                        const std::vector<double>& waveform,
                                        bool add,
                                        const std::vector<std::string>& targets);

void _libsdcparse_set_clock_groups_internal(const std::vector<std::string>& clock_list,
                                            const std::vector<int>& clock_group_start_pos);

void _libsdcparse_set_false_path_internal(const std::vector<std::string>& from_list,
                                          const std::vector<std::string>& to_list);

void _libsdcparse_set_min_delay_internal(double delay,
                                         const std::vector<std::string>& from_list,
                                         const std::vector<std::string>& to_list);

void _libsdcparse_set_max_delay_internal(double delay,
                                         const std::vector<std::string>& from_list,
                                         const std::vector<std::string>& to_list);

void _libsdcparse_set_multicycle_path_internal(bool is_setup,
                                               bool is_hold,
                                               const std::vector<std::string>& from_list,
                                               const std::vector<std::string>& to_list,
                                               int path_multiplier);

void _libsdcparse_set_input_delay_internal(bool max_delay_flag,
                                           bool min_delay_flag,
                                           const std::string& clock_name,
                                           double delay,
                                           const std::vector<std::string>& targets);

void _libsdcparse_set_output_delay_internal(bool max_delay_flag,
                                            bool min_delay_flag,
                                            const std::string& clock_name,
                                            double delay,
                                            const std::vector<std::string>& targets);

void _libsdcparse_set_clock_uncertainty_internal(bool is_setup,
                                                 bool is_hold,
                                                 const std::vector<std::string>& from_list,
                                                 const std::vector<std::string>& to_list,
                                                 double uncertainty);

void _libsdcparse_set_clock_latency_internal(bool is_source,
                                             bool is_early,
                                             bool is_late,
                                             double latency,
                                             const std::vector<std::string>& targets);

void _libsdcparse_set_disable_timing_internal(const std::vector<std::string>& from_list,
                                              const std::vector<std::string>& to_list);

void _libsdcparse_set_timing_derate_internal(bool is_early,
                                             bool is_late,
                                             bool derate_nets,
                                             bool derate_cells,
                                             double derate,
                                             const std::vector<std::string>& targets);

std::vector<std::string> _libsdcparse_all_ports_internal();

std::vector<std::string> _libsdcparse_all_clocks_internal();

std::vector<std::string> _libsdcparse_all_inputs_internal();

std::vector<std::string> _libsdcparse_all_outputs_internal();

std::vector<std::string> _libsdcparse_all_pins_internal();

std::vector<std::string> _libsdcparse_all_cells_internal();

std::string _libsdcparse_get_name_internal(std::string object_id);

bool _libsdcparse_is_object_id_internal(std::string object_id);

std::string _libsdcparse_create_port_internal(std::string port_name,
                                              std::string port_type_str);

std::string _libsdcparse_create_pin_internal(std::string pin_name);

std::string _libsdcparse_create_cell_internal(std::string cell_name);

void _libsdcparse_raise_warning(const std::string& msg);
