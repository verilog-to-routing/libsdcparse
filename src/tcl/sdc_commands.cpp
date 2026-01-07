
#include <cassert>
#include <string>

#include "sdc_commands.h"

#include "sdcparse.hpp"

sdcparse::Callback* sdcparse::g_callback = nullptr;

void _libsdcparse_lineno_internal(int line_num) {
    assert(sdcparse::g_callback != nullptr);
    sdcparse::g_callback->lineno(line_num);
}

void _libsdcparse_create_clock_internal(double period,
                                        const std::string& name,
                                        const std::vector<double>& waveform,
                                        bool add,
                                        const std::vector<std::string>& targets) {
    sdcparse::CreateClock create_clock_cmd;
    create_clock_cmd.name = name;
    create_clock_cmd.period = period;
    assert(waveform.size() == 2);
    create_clock_cmd.rise_edge = waveform[0];
    create_clock_cmd.fall_edge = waveform[1];
    if (targets.size() == 0 && !name.empty())
        create_clock_cmd.is_virtual = true;
    else
        create_clock_cmd.is_virtual = false;

    // TODO: Assert that all targets are IDs
    create_clock_cmd.targets.strings = targets;
    create_clock_cmd.targets.type = sdcparse::StringGroupType::OBJECT;

    create_clock_cmd.add = add;

    assert(sdcparse::g_callback != nullptr);
    sdcparse::g_callback->create_clock(create_clock_cmd);
}

void _libsdcparse_create_generated_clock_internal(const std::string& name,
                                                  const std::string& source,
                                                  int divide_by,
                                                  int multiply_by,
                                                  bool add,
                                                  const std::vector<std::string>& targets) {
    sdcparse::CreateGeneratedClock create_gen_clock_cmd;
    create_gen_clock_cmd.name = name;
    create_gen_clock_cmd.source = source;
    create_gen_clock_cmd.source_string_type = sdcparse::StringGroupType::OBJECT;
    create_gen_clock_cmd.divide_by = divide_by;
    create_gen_clock_cmd.multiply_by = multiply_by;
    create_gen_clock_cmd.add = add;
    create_gen_clock_cmd.targets.strings = targets;
    create_gen_clock_cmd.targets.type = sdcparse::StringGroupType::OBJECT;

    assert(sdcparse::g_callback != nullptr);
    sdcparse::g_callback->create_generated_clock(create_gen_clock_cmd);
}

void _libsdcparse_set_clock_groups_internal(const std::vector<std::string>& clock_list,
                                            const std::vector<int>& clock_group_start_pos,
                                            bool is_logically_exlusive,
                                            bool is_physically_exclusive,
                                            bool is_asynchronous) {
    sdcparse::SetClockGroups set_clock_groups_cmd;
    for (size_t i = 0; i < clock_group_start_pos.size() - 1; i++) {
        sdcparse::StringGroup new_group;
        new_group.type = sdcparse::StringGroupType::OBJECT;
        for (size_t j = clock_group_start_pos[i]; j < (size_t)clock_group_start_pos[i + 1]; j++) {
            new_group.strings.push_back(clock_list[j]);
        }
        set_clock_groups_cmd.clock_groups.push_back(std::move(new_group));
    }

    // Can only be one type.
    assert((int)is_logically_exlusive + (int)is_physically_exclusive + (int)is_asynchronous == 1);
    if (is_logically_exlusive)
        set_clock_groups_cmd.type = sdcparse::ClockGroupsType::LOGICALLY_EXCLUSIVE;
    else if (is_physically_exclusive)
        set_clock_groups_cmd.type = sdcparse::ClockGroupsType::PHYSICALLY_EXCLUSIVE;
    else if (is_asynchronous)
        set_clock_groups_cmd.type = sdcparse::ClockGroupsType::ASYNCHRONOUS;

    assert(sdcparse::g_callback != nullptr);
    sdcparse::g_callback->set_clock_groups(set_clock_groups_cmd);
}

void _libsdcparse_set_false_path_internal(const std::vector<std::string>& from_list,
                                          const std::vector<std::string>& to_list) {
    sdcparse::SetFalsePath set_false_path_cmd;
    set_false_path_cmd.from.strings = from_list;
    set_false_path_cmd.from.type = sdcparse::StringGroupType::OBJECT;
    set_false_path_cmd.to.strings = to_list;
    set_false_path_cmd.to.type = sdcparse::StringGroupType::OBJECT;

    assert(sdcparse::g_callback != nullptr);
    sdcparse::g_callback->set_false_path(set_false_path_cmd);
}

void _libsdcparse_set_min_delay_internal(double delay,
                                         const std::vector<std::string>& from_list,
                                         const std::vector<std::string>& to_list) {
    sdcparse::SetMinMaxDelay set_min_delay_cmd;
    set_min_delay_cmd.type = sdcparse::MinMaxType::MIN;
    set_min_delay_cmd.value = delay;
    set_min_delay_cmd.from.strings = from_list;
    set_min_delay_cmd.from.type = sdcparse::StringGroupType::OBJECT;
    set_min_delay_cmd.to.strings = to_list;
    set_min_delay_cmd.to.type = sdcparse::StringGroupType::OBJECT;

    assert(sdcparse::g_callback != nullptr);
    sdcparse::g_callback->set_min_max_delay(set_min_delay_cmd);
}

void _libsdcparse_set_max_delay_internal(double delay,
                                         const std::vector<std::string>& from_list,
                                         const std::vector<std::string>& to_list) {
    sdcparse::SetMinMaxDelay set_max_delay_cmd;
    set_max_delay_cmd.type = sdcparse::MinMaxType::MAX;
    set_max_delay_cmd.value = delay;
    set_max_delay_cmd.from.strings = from_list;
    set_max_delay_cmd.from.type = sdcparse::StringGroupType::OBJECT;
    set_max_delay_cmd.to.strings = to_list;
    set_max_delay_cmd.to.type = sdcparse::StringGroupType::OBJECT;

    assert(sdcparse::g_callback != nullptr);
    sdcparse::g_callback->set_min_max_delay(set_max_delay_cmd);
}

void _libsdcparse_set_multicycle_path_internal(bool is_setup,
                                               bool is_hold,
                                               const std::vector<std::string>& from_list,
                                               const std::vector<std::string>& to_list,
                                               int path_multiplier) {
    sdcparse::SetMulticyclePath set_multicycle_path_cmd;
    set_multicycle_path_cmd.is_setup = is_setup;
    set_multicycle_path_cmd.is_hold = is_hold;
    set_multicycle_path_cmd.from.strings = from_list;
    set_multicycle_path_cmd.from.type = sdcparse::StringGroupType::OBJECT;
    set_multicycle_path_cmd.to.strings = to_list;
    set_multicycle_path_cmd.to.type = sdcparse::StringGroupType::OBJECT;
    set_multicycle_path_cmd.mcp_value = path_multiplier;

    assert(sdcparse::g_callback != nullptr);
    sdcparse::g_callback->set_multicycle_path(set_multicycle_path_cmd);
}

void _libsdcparse_set_input_delay_internal(bool max_delay_flag,
                                           bool min_delay_flag,
                                           const std::string& clock_name,
                                           double delay,
                                           const std::vector<std::string>& targets) {

    sdcparse::SetIoDelay set_input_delay_cmd;
    set_input_delay_cmd.is_max = max_delay_flag;
    set_input_delay_cmd.is_min = min_delay_flag;
    set_input_delay_cmd.clock_name = clock_name;
    set_input_delay_cmd.delay = delay;
    set_input_delay_cmd.target_ports.strings = targets;
    set_input_delay_cmd.target_ports.type = sdcparse::StringGroupType::OBJECT;
    set_input_delay_cmd.type = sdcparse::IoDelayType::INPUT;

    assert(sdcparse::g_callback != nullptr);
    sdcparse::g_callback->set_io_delay(set_input_delay_cmd);
}

void _libsdcparse_set_output_delay_internal(bool max_delay_flag,
                                            bool min_delay_flag,
                                            const std::string& clock_name,
                                            double delay,
                                            const std::vector<std::string>& targets) {

    sdcparse::SetIoDelay set_output_delay_cmd;
    set_output_delay_cmd.is_max = max_delay_flag;
    set_output_delay_cmd.is_min = min_delay_flag;
    set_output_delay_cmd.clock_name = clock_name;
    set_output_delay_cmd.delay = delay;
    set_output_delay_cmd.target_ports.strings = targets;
    set_output_delay_cmd.target_ports.type = sdcparse::StringGroupType::OBJECT;
    set_output_delay_cmd.type = sdcparse::IoDelayType::OUTPUT;

    assert(sdcparse::g_callback != nullptr);
    sdcparse::g_callback->set_io_delay(set_output_delay_cmd);
}

void _libsdcparse_set_clock_uncertainty_internal(bool is_setup,
                                                 bool is_hold,
                                                 const std::vector<std::string>& from_list,
                                                 const std::vector<std::string>& to_list,
                                                 double uncertainty) {
    sdcparse::SetClockUncertainty set_clock_uncertainty_cmd;
    set_clock_uncertainty_cmd.is_setup = is_setup;
    set_clock_uncertainty_cmd.is_hold = is_hold;
    set_clock_uncertainty_cmd.from.strings = from_list;
    set_clock_uncertainty_cmd.from.type = sdcparse::StringGroupType::OBJECT;
    set_clock_uncertainty_cmd.to.strings = to_list;
    set_clock_uncertainty_cmd.to.type = sdcparse::StringGroupType::OBJECT;
    set_clock_uncertainty_cmd.value = uncertainty;

    assert(sdcparse::g_callback != nullptr);
    sdcparse::g_callback->set_clock_uncertainty(set_clock_uncertainty_cmd);
}

void _libsdcparse_set_clock_latency_internal(bool is_source,
                                             bool is_early,
                                             bool is_late,
                                             double latency,
                                             const std::vector<std::string>& targets) {
    sdcparse::SetClockLatency set_clock_latency_cmd;
    if (is_source) {
        set_clock_latency_cmd.type = sdcparse::ClockLatencyType::SOURCE;
    } else {
        set_clock_latency_cmd.type = sdcparse::ClockLatencyType::NONE;
    }
    set_clock_latency_cmd.is_early = is_early;
    set_clock_latency_cmd.is_late = is_late;
    set_clock_latency_cmd.value = latency;
    set_clock_latency_cmd.target_clocks.strings = targets;
    set_clock_latency_cmd.target_clocks.type = sdcparse::StringGroupType::OBJECT;

    assert(sdcparse::g_callback != nullptr);
    sdcparse::g_callback->set_clock_latency(set_clock_latency_cmd);
}

void _libsdcparse_set_disable_timing_internal(const std::vector<std::string>& from_list,
                                              const std::vector<std::string>& to_list) {
    sdcparse::SetDisableTiming set_disable_timing_cmd;
    set_disable_timing_cmd.from.strings = from_list;
    set_disable_timing_cmd.from.type = sdcparse::StringGroupType::OBJECT;
    set_disable_timing_cmd.to.strings = to_list;
    set_disable_timing_cmd.to.type = sdcparse::StringGroupType::OBJECT;

    assert(sdcparse::g_callback != nullptr);
    sdcparse::g_callback->set_disable_timing(set_disable_timing_cmd);
}

void _libsdcparse_set_timing_derate_internal(bool is_early,
                                             bool is_late,
                                             bool derate_nets,
                                             bool derate_cells,
                                             double derate,
                                             const std::vector<std::string>& targets) {
    sdcparse::SetTimingDerate set_timing_derate_cmd;
    set_timing_derate_cmd.is_early = is_early;
    set_timing_derate_cmd.is_late = is_late;
    set_timing_derate_cmd.derate_nets = derate_nets;
    set_timing_derate_cmd.derate_cells = derate_cells;
    set_timing_derate_cmd.value = derate;
    set_timing_derate_cmd.cell_targets.strings = targets;
    set_timing_derate_cmd.cell_targets.type = sdcparse::StringGroupType::OBJECT;

    assert(sdcparse::g_callback != nullptr);
    sdcparse::g_callback->set_timing_derate(set_timing_derate_cmd);
}

std::vector<std::string> _libsdcparse_all_ports_internal() {
    assert(sdcparse::g_callback != nullptr);
    return sdcparse::g_callback->obj_database.get_port_objects();
}

std::vector<std::string> _libsdcparse_all_clocks_internal() {
    assert(sdcparse::g_callback != nullptr);
    return sdcparse::g_callback->obj_database.get_clock_objects();
}

std::vector<std::string> _libsdcparse_all_inputs_internal() {
    assert(sdcparse::g_callback != nullptr);
    return sdcparse::g_callback->obj_database.get_input_port_objects();
}

std::vector<std::string> _libsdcparse_all_outputs_internal() {
    assert(sdcparse::g_callback != nullptr);
    return sdcparse::g_callback->obj_database.get_output_port_objects();
}

std::vector<std::string> _libsdcparse_all_pins_internal() {
    assert(sdcparse::g_callback != nullptr);
    return sdcparse::g_callback->obj_database.get_pin_objects();
}

std::vector<std::string> _libsdcparse_all_cells_internal() {
    assert(sdcparse::g_callback != nullptr);
    return sdcparse::g_callback->obj_database.get_cell_objects();
}

std::string _libsdcparse_get_name_internal(const std::string& object_id) {
    assert(sdcparse::g_callback != nullptr);
    return sdcparse::g_callback->obj_database.get_object_name(sdcparse::ObjectId(object_id));
}

bool _libsdcparse_is_object_id_internal(const std::string& object_id) {
    assert(sdcparse::g_callback != nullptr);
    return sdcparse::g_callback->obj_database.is_object_id(object_id);
}

std::string _libsdcparse_get_object_type_internal(const std::string& object_id) {
    assert(sdcparse::g_callback != nullptr);
    sdcparse::ObjectType object_type = sdcparse::g_callback->obj_database.get_object_type(object_id);
    return sdcparse::to_string(object_type);
}

std::string _libsdcparse_create_port_internal(const std::string& port_name,
                                              const std::string& port_dir_str) {
    sdcparse::PortDirection port_dir = sdcparse::from_string_to_port_dir(port_dir_str);
    assert(port_dir != sdcparse::PortDirection::UNKNOWN);

    assert(sdcparse::g_callback != nullptr);
    return sdcparse::g_callback->obj_database.create_port_object(port_name, port_dir).to_string();
}

std::string _libsdcparse_create_pin_internal(const std::string& pin_name) {
    assert(sdcparse::g_callback != nullptr);
    return sdcparse::g_callback->obj_database.create_pin_object(pin_name).to_string();
}

std::string _libsdcparse_create_cell_internal(const std::string& cell_name) {
    assert(sdcparse::g_callback != nullptr);
    return sdcparse::g_callback->obj_database.create_cell_object(cell_name).to_string();
}

void _libsdcparse_raise_warning(const std::string& msg) {
    assert(sdcparse::g_callback != nullptr);
    sdcparse::g_callback->parse_warning(msg);
}
