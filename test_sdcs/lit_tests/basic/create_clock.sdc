# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: [[clk1_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "clk1" -type INPUT]
# CHECK: [[clk2_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "clk2" -type INPUT]
# CHECK: [[clk3_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "clk3" -type INPUT]
# CHECK: [[clk4_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "clk4" -type INPUT]
# CHECK: [[clk5_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "clk5" -type INPUT]
# CHECK: [[clk6_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "clk6" -type INPUT]

# CHECK: create_clock -period {{1.0*}} -waveform {{{0.0*}} {{0.50*}}} {[[clk1_port_ptr]]}
create_clock -period 1.0 clk1

# CHECK: create_clock -period {{6.0*}} -waveform {{{0.0*}} {{3.0*}}} {[[clk2_port_ptr]]} -add
create_clock -period 6.0 clk2 -add

# CHECK: create_clock -period {{7.0*}} -waveform {{{0.0*}} {{3.50*}}} {[[clk3_port_ptr]]} -add
create_clock -period 7.0 -add clk3

# CHECK: create_clock -period {{2.0*}} -waveform {{{0.0*}} {{1.0*}}} -name Test {[[clk4_port_ptr]] [[clk5_port_ptr]]}
create_clock -period 2.0 {clk4 clk5} -name Test

# CHECK: create_clock -period {{5.0*}} -waveform {{{0.0*}} {{2.50*}}} -name Jeff
create_clock -period 5 -name Jeff

# CHECK: create_clock -period {{3.0*}} -waveform {{{1.0*}} {{2.0*}}} {[[clk6_port_ptr]]}
create_clock -period 3 -waveform {1 2} clk6
