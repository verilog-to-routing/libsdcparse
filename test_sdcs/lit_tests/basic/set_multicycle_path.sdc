# RUN: %sdcparse-test %s 2>&1 | filecheck %s

_libsdcparse_create_port "clk1" -type INPUT
_libsdcparse_create_port "clk2" -type INPUT

create_clock -period 1 clk1
create_clock -period 1 clk2

# CHECK: set_multicycle_path 4 -from {clk1} -to {clk2}
set_multicycle_path -from [get_clocks clk1] -to [get_clocks clk2] 4

# CHECK: set_multicycle_path 3 -setup -from {clk1} -to {clk2}
set_multicycle_path -setup -from [get_clocks clk1] -to [get_clocks clk2] 3

# CHECK: set_multicycle_path 2 -hold -from {clk1} -to {clk2}
set_multicycle_path -hold -from [get_clocks clk1] -to [get_clocks clk2] 2
