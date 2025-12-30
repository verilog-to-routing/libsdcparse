# RUN: %sdcparse-test %s 2>&1 | filecheck %s

_libsdcparse_create_port "clk1" -type INPUT
_libsdcparse_create_port "clk2" -type INPUT

create_clock -period 1 clk1
create_clock -period 1 clk2
create_clock -period 2 -name virtual
create_clock -period 2 -name virtual2

# CHECK: set_false_path -from {clk1} -to {virtual}
set_false_path -from clk1 -to virtual

# CHECK: set_false_path -from {clk2} -to {virtual2}
set_false_path -from [get_clocks clk2] -to [get_clocks virtual2]
