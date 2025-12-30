# RUN: %sdcparse-test %s 2>&1 | filecheck %s

_libsdcparse_create_port "clk1" -type INPUT
_libsdcparse_create_port "clk2" -type INPUT

create_clock -period 1 clk1
create_clock -period 1 clk2

# CHECK: set_clock_uncertainty {{0.0250*}}
set_clock_uncertainty 0.025

# CHECK: set_clock_uncertainty -from {clk1} {{0.050*}}
set_clock_uncertainty -from [get_clocks clk1] 0.05

# CHECK: set_clock_uncertainty -from {clk1} -to {clk2} {{0.050*}}
set_clock_uncertainty -from [get_clocks clk1] -to [get_clocks clk2] 0.05
