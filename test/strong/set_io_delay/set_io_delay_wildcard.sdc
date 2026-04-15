# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK-NEXT: [[in1_port_ptr:__vtr_obj_[0-9]+]]
puts [libsdcparse_create_port "in1" -direction INPUT]

# CHECK: create_clock -period {{10.0*}} -waveform {{{0.0*}} {{5.0*}}} -name clk
create_clock -period 10.0 -name clk

# CHECK: clk: [[clk_clock_ptr:__vtr_obj_[0-9]+]]
puts "clk: [get_clocks clk]"

# CHECK: set_input_delay -clock {[[clk_clock_ptr]]} -max {{0.50*}} {[[in1_port_ptr]]}
set_input_delay -clock * -max 0.5 [get_ports in1]

# CHECK: set_output_delay -clock {[[clk_clock_ptr]]} -max {{0.50*}} {[[in1_port_ptr]]}
set_output_delay -clock * -max 0.5 [get_ports in1]