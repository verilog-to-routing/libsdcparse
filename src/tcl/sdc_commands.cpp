
#include <iostream>
#include <stdexcept>
#include <string>

#include "sdc_commands.h"

#include "sdcparse.hpp"

sdcparse::Callback* sdcparse::g_callback = nullptr;

void lineno_internal(int line_num) {
    sdcparse::g_callback->lineno(line_num);
}

void create_clock_internal(double period,
                           const std::string& name,
                           const std::vector<double>& waveform,
                           bool add,
                           const std::vector<std::string>& targets) {
    // TODO: Have this call the proper callback.
    // std::cout << "create_clock with period " << period << " named: " << name << std::endl;

    sdcparse::CreateClock create_clock_cmd;
    create_clock_cmd.name = name;
    create_clock_cmd.period = period;
    if (waveform.size() == 0) {
        create_clock_cmd.rise_edge = 0.0;
        create_clock_cmd.fall_edge = period / 2.0;
    } else {
        if (waveform.size() != 2) {
            throw new std::runtime_error("Waveform must be length 2.");
        }
        create_clock_cmd.rise_edge = waveform[0];
        create_clock_cmd.fall_edge = waveform[1];
    }
    if (targets.size() == 0 && !name.empty())
        create_clock_cmd.is_virtual = true;
    else
        create_clock_cmd.is_virtual = false;

    // TODO: Assert that all targets are IDs
    create_clock_cmd.targets.strings = targets;
    create_clock_cmd.targets.type = sdcparse::StringGroupType::OBJECT;

    create_clock_cmd.add = add;

    // TODO: This can probably be made into an assert.
    if (sdcparse::g_callback == nullptr) {
        // FIXME: Make this a proper error.
        throw new std::runtime_error("Callback not registered!.");
    }
    sdcparse::g_callback->create_clock(create_clock_cmd);
}

void set_clock_groups_internal(const std::vector<std::string>& clock_list,
                               const std::vector<int>& clock_group_start_pos) {
    sdcparse::SetClockGroups set_clock_groups_cmd;
    for (size_t i = 0; i < clock_group_start_pos.size() - 1; i++) {
        sdcparse::StringGroup new_group;
        new_group.type = sdcparse::StringGroupType::OBJECT;
        for (size_t j = clock_group_start_pos[i]; j < (size_t)clock_group_start_pos[i + 1]; j++) {
            new_group.strings.push_back(clock_list[j]);
        }
        set_clock_groups_cmd.clock_groups.push_back(std::move(new_group));
    }

    if (sdcparse::g_callback == nullptr) {
        // FIXME: Make this a proper error.
        throw new std::runtime_error("Callback not registered!.");
    }
    sdcparse::g_callback->set_clock_groups(set_clock_groups_cmd);
}

void set_false_path_internal(const std::vector<std::string>& from_list,
                             const std::vector<std::string>& to_list) {
    sdcparse::SetFalsePath set_false_path_cmd;
    set_false_path_cmd.from.strings = from_list;
    set_false_path_cmd.from.type = sdcparse::StringGroupType::OBJECT;
    set_false_path_cmd.to.strings = to_list;
    set_false_path_cmd.to.type = sdcparse::StringGroupType::OBJECT;

    if (sdcparse::g_callback == nullptr) {
        // FIXME: Make this a proper error.
        throw new std::runtime_error("Callback not registered!.");
    }
    sdcparse::g_callback->set_false_path(set_false_path_cmd);
}

void set_min_delay_internal(double delay,
                            const std::vector<std::string>& from_list,
                            const std::vector<std::string>& to_list) {
    sdcparse::SetMinMaxDelay set_min_delay_cmd;
    set_min_delay_cmd.type = sdcparse::MinMaxType::MIN;
    set_min_delay_cmd.value = delay;
    set_min_delay_cmd.from.strings = from_list;
    set_min_delay_cmd.from.type = sdcparse::StringGroupType::OBJECT;
    set_min_delay_cmd.to.strings = to_list;
    set_min_delay_cmd.to.type = sdcparse::StringGroupType::OBJECT;

    if (sdcparse::g_callback == nullptr) {
        // FIXME: Make this a proper error.
        throw new std::runtime_error("Callback not registered!.");
    }
    sdcparse::g_callback->set_min_max_delay(set_min_delay_cmd);
}

void set_max_delay_internal(double delay,
                            const std::vector<std::string>& from_list,
                            const std::vector<std::string>& to_list) {
    sdcparse::SetMinMaxDelay set_max_delay_cmd;
    set_max_delay_cmd.type = sdcparse::MinMaxType::MAX;
    set_max_delay_cmd.value = delay;
    set_max_delay_cmd.from.strings = from_list;
    set_max_delay_cmd.from.type = sdcparse::StringGroupType::OBJECT;
    set_max_delay_cmd.to.strings = to_list;
    set_max_delay_cmd.to.type = sdcparse::StringGroupType::OBJECT;

    if (sdcparse::g_callback == nullptr) {
        // FIXME: Make this a proper error.
        throw new std::runtime_error("Callback not registered!.");
    }
    sdcparse::g_callback->set_min_max_delay(set_max_delay_cmd);
}

void set_multicycle_path_internal(bool is_setup,
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

    if (sdcparse::g_callback == nullptr) {
        // FIXME: Make this a proper error.
        throw new std::runtime_error("Callback not registered!.");
    }
    sdcparse::g_callback->set_multicycle_path(set_multicycle_path_cmd);
}

void set_input_delay_internal(bool max_delay_flag,
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

    if (sdcparse::g_callback == nullptr) {
        // FIXME: Make this a proper error.
        throw new std::runtime_error("Callback not registered!.");
    }
    sdcparse::g_callback->set_io_delay(set_input_delay_cmd);
}

void set_output_delay_internal(bool max_delay_flag,
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

    if (sdcparse::g_callback == nullptr) {
        // FIXME: Make this a proper error.
        throw new std::runtime_error("Callback not registered!.");
    }
    sdcparse::g_callback->set_io_delay(set_output_delay_cmd);
}


std::vector<std::string> all_ports_internal() {
    if (sdcparse::g_callback == nullptr) {
        // FIXME: Make this a proper error.
        throw new std::runtime_error("Callback not registered!.");
    }

    return sdcparse::g_callback->obj_database.get_port_objects();
}

std::vector<std::string> all_clocks_internal() {
    if (sdcparse::g_callback == nullptr) {
        // FIXME: Make this a proper error.
        throw new std::runtime_error("Callback not registered!.");
    }

    return sdcparse::g_callback->obj_database.get_clock_objects();
}

std::string get_name_internal(std::string object_id) {
    if (sdcparse::g_callback == nullptr) {
        // FIXME: Make this a proper error.
        throw new std::runtime_error("Callback not registered!.");
    }

    return sdcparse::g_callback->obj_database.get_object_name(object_id);
}

bool is_object_id_internal(std::string object_id) {
    if (sdcparse::g_callback == nullptr) {
        // FIXME: Make this a proper error.
        throw new std::runtime_error("Callback not registered!.");
    }

    return sdcparse::g_callback->obj_database.is_object_id(object_id);
}
