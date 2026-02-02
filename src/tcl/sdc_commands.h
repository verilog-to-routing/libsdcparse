#pragma once
/**
 * @file
 * @author  Alex Singer
 * @date    January 2026
 * @brief   Declaration of all interface callback functions which will be
 *          callable from TCL.
 *
 * This file is included within the SWIG interface file, so anything declared
 * in here will be setup as a callable function within TCL.
 */

#include <string>
#include <vector>

/**
 * @brief Set the internal line number in the callback function.
 */
void libsdcparse_lineno_internal(int line_num);

/**
 * @brief Raise a warning message from the TCL side.
 */
void libsdcparse_raise_warning(const std::string& msg);

// The following are internal wrapper calls to the underlying callback data
// structures. The line up basically one-to-one with the SDC commands that are
// supported.

void libsdcparse_create_clock_internal(double period,
                                        const std::string& name,
                                        const std::vector<double>& waveform,
                                        bool add,
                                        const std::vector<std::string>& targets);

void libsdcparse_create_generated_clock_internal(const std::string& name,
                                                  const std::string& source,
                                                  int divide_by,
                                                  int multiply_by,
                                                  bool add,
                                                  const std::vector<std::string>& targets);

void libsdcparse_set_clock_groups_internal(const std::vector<std::string>& clock_list,
                                            const std::vector<int>& clock_group_start_pos,
                                            bool is_logically_exclusive,
                                            bool is_physically_exclusive,
                                            bool is_asynchronous);

void libsdcparse_set_false_path_internal(const std::vector<std::string>& from_list,
                                          const std::vector<std::string>& to_list);

void libsdcparse_set_min_delay_internal(double delay,
                                         const std::vector<std::string>& from_list,
                                         const std::vector<std::string>& to_list);

void libsdcparse_set_max_delay_internal(double delay,
                                         const std::vector<std::string>& from_list,
                                         const std::vector<std::string>& to_list);

void libsdcparse_set_multicycle_path_internal(bool is_setup,
                                               bool is_hold,
                                               const std::vector<std::string>& from_list,
                                               const std::vector<std::string>& to_list,
                                               int path_multiplier);

void libsdcparse_set_input_delay_internal(bool max_delay_flag,
                                           bool min_delay_flag,
                                           const std::string& clock_name,
                                           double delay,
                                           const std::vector<std::string>& targets);

void libsdcparse_set_output_delay_internal(bool max_delay_flag,
                                            bool min_delay_flag,
                                            const std::string& clock_name,
                                            double delay,
                                            const std::vector<std::string>& targets);

void libsdcparse_set_clock_uncertainty_internal(bool is_setup,
                                                 bool is_hold,
                                                 const std::vector<std::string>& from_list,
                                                 const std::vector<std::string>& to_list,
                                                 double uncertainty);

void libsdcparse_set_clock_latency_internal(bool is_source,
                                             bool is_early,
                                             bool is_late,
                                             double latency,
                                             const std::vector<std::string>& targets);

void libsdcparse_set_disable_timing_internal(const std::vector<std::string>& from_list,
                                              const std::vector<std::string>& to_list);

void libsdcparse_set_timing_derate_internal(bool is_early,
                                             bool is_late,
                                             bool derate_nets,
                                             bool derate_cells,
                                             double derate,
                                             const std::vector<std::string>& targets);

std::vector<std::string> libsdcparse_all_ports_internal();

std::vector<std::string> libsdcparse_all_clocks_internal();

std::vector<std::string> libsdcparse_all_inputs_internal();

std::vector<std::string> libsdcparse_all_outputs_internal();

std::vector<std::string> libsdcparse_all_pins_internal();

std::vector<std::string> libsdcparse_all_cells_internal();

std::vector<std::string> libsdcparse_all_clock_drivers_internal();

std::vector<std::string> libsdcparse_all_nets_internal();

std::string libsdcparse_get_name_internal(const std::string& object_id);

bool libsdcparse_is_object_id_internal(const std::string& object_id);

std::string libsdcparse_get_object_type_internal(const std::string& object_id);

std::string libsdcparse_create_port_internal(const std::string& port_name,
                                             const std::string& port_dir_str,
                                             bool is_clock_driver);

std::string libsdcparse_create_pin_internal(const std::string& pin_name,
                                            bool is_clock_driver);

std::string libsdcparse_create_cell_internal(const std::string& cell_name);

std::string libsdcparse_create_net_internal(const std::string& net_name);

