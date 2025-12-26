#pragma once

#include <string>
#include "sdcparse.hpp"

// TODO: Create a global variable to hold the callback.
//       It will be a pointer to the base callback class.

namespace sdcparse {

extern Callback* g_callback;

} // namespace sdcparse

void create_clock_internal(double period,
                           const std::string& name,
                           const std::vector<double>& waveform,
                           bool add,
                           const std::vector<std::string>& targets);

void set_input_delay_internal(bool max_delay_flag,
                              bool min_delay_flag,
                              const std::string& clock_name,
                              double delay,
                              const std::vector<std::string>& targets);

std::vector<std::string> all_ports_internal();
