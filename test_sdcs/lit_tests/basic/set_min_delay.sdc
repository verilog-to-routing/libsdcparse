# RUN: %sdcparse-test %s 2>&1 | filecheck %s

_libsdcparse_create_port "clk1" -type INPUT
_libsdcparse_create_port "clk2" -type INPUT

create_clock -period 1 clk1
create_clock -period 1 clk2

# CHECK: set_min_delay {{2.0*}} -from {clk1} -to {clk2}
set_min_delay 2 -from [get_clocks clk1] -to [get_clocks clk2]
