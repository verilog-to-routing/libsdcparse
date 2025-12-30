# RUN: %sdcparse-test %s 2>&1 | filecheck %s

_libsdcparse_create_port "clk1" -type INPUT
_libsdcparse_create_port "clk2" -type INPUT

create_clock -period 1 clk1
create_clock -period 1 clk2

create_clock -period 1 -name virtual1
create_clock -period 1 -name virtual2
create_clock -period 1 -name virtual3

# CHECK: set_clock_groups -group {clk1} -group {clk2}
set_clock_groups -exclusive -group clk1 -group clk2

# TODO: Order is not guarenteed. Need to make more robust.
# CHECK: set_clock_groups -group {virtual1 virtual2 virtual3} -group {clk2}
set_clock_groups -exclusive -group [get_clocks virtual*] -group clk2

# CHECK: set_clock_groups -group {clk1} -group {virtual1} -group {virtual2} -group {virtual3}
set_clock_groups -exclusive -group clk1 -group virtual1 -group virtual2 -group virtual3
