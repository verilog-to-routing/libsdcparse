/* sdc_commands.i */
%module sdc_commands
%include "std_string.i"
%include "std_vector.i"
%include "exception.i"
%{
#include "sdc_commands.h"
extern int Sdc_commands_SafeInit(Tcl_Interp* interp);
%}

%exception {
    try {
        $function;
    } catch (const std::out_of_range& oor) {
        SWIG_exception(SWIG_IndexError, oor.what());
    } catch (const std::exception& e) {
        SWIG_exception(SWIG_RuntimeError, e.what());
    }
}

namespace std {
    %template(StringVector) vector<std::string>;
    %template(DoubleVector) vector<double>;
    %template(IntVector) vector<int>;
}

void _libsdcparse_lineno_internal(int line_num);

void _libsdcparse_create_clock_internal(double period,
                                        const std::string& name,
                                        const std::vector<double>& waveform,
                                        bool add,
                                        const std::vector<std::string>& targets);

void _libsdcparse_create_generated_clock_internal(const std::string& name,
                                                  const std::string& source,
                                                  int divide_by,
                                                  int multiply_by,
                                                  bool add,
                                                  const std::vector<std::string>& targets);

void _libsdcparse_set_clock_groups_internal(const std::vector<std::string>& clock_list,
                                            const std::vector<int>& clock_group_start_pos,
                                            bool is_logically_exlusive,
                                            bool is_physically_exclusive,
                                            bool is_asynchronous);

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

std::string _libsdcparse_get_name_internal(const std::string& object_id);

bool _libsdcparse_is_object_id_internal(const std::string& object_id);

std::string _libsdcparse_get_object_type_internal(const std::string& object_id);

std::string _libsdcparse_create_port_internal(const std::string& port_name,
                                              const std::string& port_type_str);

std::string _libsdcparse_create_pin_internal(const std::string& pin_name);

std::string _libsdcparse_create_cell_internal(const std::string& cell_name);

void _libsdcparse_raise_warning(const std::string& msg);
