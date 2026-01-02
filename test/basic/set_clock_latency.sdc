# RUN: %sdcparse-test %s 2>&1 | filecheck %s

_libsdcparse_create_port "clk1" -type INPUT

create_clock -period 1 clk1

# CHECK: [[clk1_ptr:__vtr_obj_clock_[0-9]+]]
puts [get_clocks clk1]

# CHECK: set_clock_latency -source {{1.0*}} {[[clk1_ptr]]}
set_clock_latency -source 1.0 [get_clocks clk1]
