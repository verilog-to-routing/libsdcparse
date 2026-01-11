# RUN: %sdcparse-test %s 2>&1 | filecheck %s

libsdcparse_create_port "clk1" -direction INPUT
libsdcparse_create_port "clk2" -direction INPUT

create_clock -period 1 clk1
create_clock -period 1 clk2

# CHECK: [[clk1_ptr:__vtr_obj_clock_[0-9]+]]
puts [get_clocks clk1]
# CHECK: [[clk2_ptr:__vtr_obj_clock_[0-9]+]]
puts [get_clocks clk2]

# CHECK: set_min_delay {{2.0*}} -from {[[clk1_ptr]]} -to {[[clk2_ptr]]}
set_min_delay 2 -from [get_clocks clk1] -to [get_clocks clk2]
