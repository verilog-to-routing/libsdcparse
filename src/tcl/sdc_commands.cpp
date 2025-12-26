
#include <iostream>
#include <stdexcept>
#include <string>

#include "sdc_commands.h"

#include "sdcparse.hpp"

sdcparse::Callback* sdcparse::g_callback = nullptr;

void create_clock_internal(double period,
                           const std::string& name,
                           const std::vector<double>& waveform,
                           bool add,
                           const std::vector<std::string>& targets) {
    // TODO: Have this call the proper callback.
    // std::cout << "create_clock with period " << period << " named: " << name << std::endl;

    if (add) {
        std::cout << "Add command unsupported." << std::endl;
        throw new std::runtime_error("Add command unsupported");
    }

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

    create_clock_cmd.targets.strings = targets;

    if (sdcparse::g_callback == nullptr) {
        // FIXME: Make this a proper error.
        throw new std::runtime_error("Callback not registered!.");
    }
    sdcparse::g_callback->create_clock(create_clock_cmd);
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
    set_input_delay_cmd.type = sdcparse::IoDelayType::INPUT;

    if (sdcparse::g_callback == nullptr) {
        // FIXME: Make this a proper error.
        throw new std::runtime_error("Callback not registered!.");
    }
    sdcparse::g_callback->set_io_delay(set_input_delay_cmd);
}
