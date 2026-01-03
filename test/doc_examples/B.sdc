# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: [[clk_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "clk" -direction INPUT]
# CHECK: [[clk2_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "clk2" -direction INPUT]

# CHECK: create_clock -period {{2.0*}} -waveform {{{0.0*}} {{1.0*}}} {[[clk_port_ptr]]}
create_clock -period 2 clk

# CHECK: create_clock -period {{3.0*}} -waveform {{{0.0*}} {{1.50*}}} {[[clk2_port_ptr]]}
create_clock -period 3 clk2

# CHECK: set_clock_groups -exclusive -group {__vtr_obj_clock_0} -group {__vtr_obj_clock_1}
set_clock_groups -exclusive -group {clk} -group {clk2}
