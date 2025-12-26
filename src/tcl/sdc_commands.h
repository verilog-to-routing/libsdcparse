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
