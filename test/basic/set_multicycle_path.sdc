# RUN: %sdcparse-test %s 2>&1 | filecheck %s

libsdcparse_create_port "clk1" -direction INPUT
libsdcparse_create_port "clk2" -direction INPUT

create_clock -period 1 clk1
create_clock -period 1 clk2

# CHECK: [[clk1_ptr:__vtr_obj_clock_[0-9]+]]
puts [get_clocks clk1]
# CHECK: [[clk2_ptr:__vtr_obj_clock_[0-9]+]]
puts [get_clocks clk2]

# CHECK: set_multicycle_path 4 -from {[[clk1_ptr]]} -to {[[clk2_ptr]]}
set_multicycle_path -from [get_clocks clk1] -to [get_clocks clk2] 4

# CHECK: set_multicycle_path 3 -setup -from {[[clk1_ptr]]} -to {[[clk2_ptr]]}
set_multicycle_path -setup -from [get_clocks clk1] -to [get_clocks clk2] 3

# CHECK: set_multicycle_path 2 -hold -from {[[clk1_ptr]]} -to {[[clk2_ptr]]}
set_multicycle_path -hold -from [get_clocks clk1] -to [get_clocks clk2] 2

# CHECK: set_multicycle_path 5 -from {[[clk1_ptr]]}
set_multicycle_path -from [get_clocks clk1] 5

# CHECK: set_multicycle_path 6 -to {[[clk2_ptr]]}
set_multicycle_path -to [get_clocks clk2] 6
