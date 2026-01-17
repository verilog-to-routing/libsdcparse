# RUN: %sdcparse-test %s 2>&1 | filecheck %s

libsdcparse_create_port "clk1" -direction INPUT
libsdcparse_create_port "clk2" -direction INPUT

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

# CHECK: [[out_pin1_ptr:__vtr_obj_pin_[0-9]+]]
puts [libsdcparse_create_pin "out_pin1" -direction OUTPUT]
# CHECK: [[in_pin1_ptr:__vtr_obj_pin_[0-9]+]]
puts [libsdcparse_create_pin "in_pin1" -direction INPUT]

# CHECK: set_false_path -from {[[out_pin1_ptr]]} -to {[[in_pin1_ptr]]}
set_false_path -from out_pin1 -to in_pin1

# CHECK: set_false_path -from {[[out_pin1_ptr]]} -to {[[in_pin1_ptr]]}
set_false_path -from [get_pins out_pin1] -to [get_pins in_pin1]
