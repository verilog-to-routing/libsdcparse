# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: [[clk1_port_ptr:__vtr_obj_[0-9]+]]
puts [libsdcparse_create_port "clk1" -direction INPUT]
# CHECK: [[clk2_port_ptr:__vtr_obj_[0-9]+]]
puts [libsdcparse_create_port "clk2" -direction INPUT]

create_clock -period 1 clk1
create_clock -period 1 clk2
create_clock -period 1 -name virtual1
create_clock -period 1 -name virtual2
create_clock -period 1 -name virtual3

# CHECK: clk1: [[clk1_ptr:__vtr_obj_[0-9]+]]
puts "clk1: [get_clocks clk1]"
# CHECK: clk2: [[clk2_ptr:__vtr_obj_[0-9]+]]
puts "clk2: [get_clocks clk2]"
# CHECK: virtual1: [[virtual1_ptr:__vtr_obj_[0-9]+]]
puts "virtual1: [get_clocks virtual1]"
# CHECK: virtual2: [[virtual2_ptr:__vtr_obj_[0-9]+]]
puts "virtual2: [get_clocks virtual2]"
# CHECK: virtual3: [[virtual3_ptr:__vtr_obj_[0-9]+]]
puts "virtual3: [get_clocks virtual3]"

# CHECK: set_clock_groups -asynchronous -group {[[clk1_ptr]]} -group {[[clk2_ptr]]}
set_clock_groups -asynchronous -group clk1 -group clk2

# TODO: Order is not guaranteed. Need to make more robust.
# CHECK: set_clock_groups -asynchronous -group {[[virtual1_ptr]] [[virtual2_ptr]] [[virtual3_ptr]]} -group {[[clk2_ptr]]}
set_clock_groups -asynchronous -group [get_clocks virtual*] -group clk2

# CHECK: set_clock_groups -asynchronous -group {[[clk1_ptr]]} -group {[[virtual1_ptr]]} -group {[[virtual2_ptr]]} -group {[[virtual3_ptr]]}
set_clock_groups -asynchronous -group clk1 -group virtual1 -group virtual2 -group virtual3

# CHECK: set_clock_groups -logically_exclusive -group {[[clk1_ptr]]} -group {[[clk2_ptr]]}
set_clock_groups -logically_exclusive -group clk1 -group clk2

# CHECK: set_clock_groups -physically_exclusive -group {[[clk1_ptr]]} -group {[[clk2_ptr]]}
set_clock_groups -physically_exclusive -group clk1 -group clk2
