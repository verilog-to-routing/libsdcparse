
#include <iostream>
#include <string>

#include "sdc_commands.h"

void create_clock_internal(double period, const std::string& name) {
    // TODO: Have this call the proper callback.
    std::cout << "create_clock with period " << period << " named: " << name << std::endl;
}
