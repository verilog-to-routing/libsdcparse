
#include <iostream>
#include <string>

#include "sdc_commands.h"

#include "sdcparse.hpp"

sdcparse::Callback* sdcparse::g_callback = nullptr;

void create_clock_internal(double period, const std::string& name) {
    // TODO: Have this call the proper callback.
    // std::cout << "create_clock with period " << period << " named: " << name << std::endl;

    sdcparse::CreateClock create_clock_cmd;
    create_clock_cmd.name = name;
    create_clock_cmd.period = period;
    // TODO: Need to parse in rise and fall edges.
    create_clock_cmd.rise_edge = 0.0;
    create_clock_cmd.fall_edge = period / 2.0;
    // TODO: Need to parse targets to handle virtual clocks.
    //       A clock is virtual if it has no targets and it is named.
    // For now lets make all clocks virtual.
    create_clock_cmd.is_virtual = true;

    if (sdcparse::g_callback == nullptr) {
        // FIXME: Make this a proper error.
        throw new std::runtime_error("Callback not registered!.");
    }
    sdcparse::g_callback->create_clock(create_clock_cmd);
}
