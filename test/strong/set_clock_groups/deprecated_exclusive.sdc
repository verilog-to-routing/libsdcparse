# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: [[clk1_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "clk1" -direction INPUT]
# CHECK: [[clk2_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "clk2" -direction INPUT]

create_clock -period 1 clk1
create_clock -period 1 clk2

# CHECK: clk1: [[clk1_clock_ptr:__vtr_obj_clock_[0-9]+]]
puts "clk1: [get_clocks clk1]"
# CHECK: clk2: [[clk2_clock_ptr:__vtr_obj_clock_[0-9]+]]
puts "clk2: [get_clocks clk2]"

# CHECK: Warning at line [[# @LINE + 2]]: set_clock_groups: '-exclusive' argument is deprecated. Use '-asynchronous' instead.
# CHECK: set_clock_groups -asynchronous -group {[[clk1_clock_ptr]]} -group {[[clk2_clock_ptr]]}
set_clock_groups -exclusive -group clk1 -group clk2
