# RUN: %sdcparse-test %s 2>&1 | filecheck %s

_libsdcparse_create_port "clk1" -type INPUT
_libsdcparse_create_port "clk2" -type INPUT

create_clock -period 1 clk1
create_clock -period 1 clk2
create_clock -period 2 -name virtual
create_clock -period 2 -name virtual2

# CHECK: [[clk1_ptr:__vtr_obj_clock_[0-9]+]]
puts [get_clocks clk1]
# CHECK: [[clk2_ptr:__vtr_obj_clock_[0-9]+]]
puts [get_clocks clk2]
# CHECK: [[virtual_ptr:__vtr_obj_clock_[0-9]+]]
puts [get_clocks virtual]
# CHECK: [[virtual2_ptr:__vtr_obj_clock_[0-9]+]]
puts [get_clocks virtual2]

# COM: TODO: Verify that this should return both the clock and the port.
# CHECK: set_false_path -from {[[clk1_ptr]] __vtr_obj_port_0} -to {[[virtual_ptr]]}
set_false_path -from clk1 -to virtual

# CHECK: set_false_path -from {[[clk2_ptr]]} -to {[[virtual2_ptr]]}
set_false_path -from [get_clocks clk2] -to [get_clocks virtual2]
