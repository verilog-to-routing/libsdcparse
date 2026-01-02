# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: [[clk1_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "clk1" -type INPUT]

# TODO: This is not technically correct. To make this test better, we need to
#       have IO ports and produce warnings that we are trying to treate a non-clock
#       as a clock.
# CHECK: create_clock -period {{0.0*}} -waveform {{{0.0*}} {{0.0*}}} {[[clk1_port_ptr]]}
create_clock -period 0 *
