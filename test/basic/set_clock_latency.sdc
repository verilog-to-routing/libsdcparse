# RUN: %sdcparse-test %s 2>&1 | filecheck %s

libsdcparse_create_port "clk1" -direction INPUT

create_clock -period 1 clk1

# CHECK: clk1: [[clk1_ptr:__vtr_obj_[0-9]+]]
puts "clk1: [get_clocks clk1]"

# CHECK: set_clock_latency -source {{1.0*}} {[[clk1_ptr]]}
set_clock_latency -source 1.0 [get_clocks clk1]
