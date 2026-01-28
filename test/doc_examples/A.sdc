# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: [[clk1_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "clk1" -direction INPUT -clock_driver]
puts [libsdcparse_create_port "in1" -direction INPUT]
puts [libsdcparse_create_port "in2" -direction INPUT]
puts [libsdcparse_create_port "out1" -direction OUTPUT]
puts [libsdcparse_create_port "out2" -direction OUTPUT]

# CHECK: create_clock -period {{0.0*}} -waveform {{{0.0*}} {{0.0*}}} {[[clk1_port_ptr]]}
create_clock -period 0 *
