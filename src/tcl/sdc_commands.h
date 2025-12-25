#pragma once

#include <string>

// TODO: Create a global variable to hold the callback.
//       It will be a pointer to the base callback class.

void create_clock_internal(double period, const std::string& name);
