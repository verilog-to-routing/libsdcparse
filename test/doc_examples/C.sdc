# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: [[in1_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "in1" -direction INPUT]
# CHECK-NEXT: [[in2_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "in2" -direction INPUT]
# CHECK-NEXT: [[out1_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "out1" -direction OUTPUT]
# CHECK-NEXT: [[out2_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "out2" -direction OUTPUT]
# CHECK-NEXT: [[clk_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "clk" -direction INPUT]
# CHECK-NEXT: [[clk2_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "clk2" -direction INPUT]

# CHECK: create_clock -period {{2.0*}} -waveform {{{0.0*}} {{1.0*}}} {[[clk_port_ptr]]}
create_clock -period 2 clk

# CHECK: create_clock -period {{3.0*}} -waveform {{{0.0*}} {{1.50*}}} {[[clk2_port_ptr]]}
create_clock -period 3 clk2

# CHECK: create_clock -period {{3.50*}} -waveform {{{0.0*}} {{1.750*}}} -name virtual_io_clock
create_clock -period 3.5 -name virtual_io_clock

# CHECK: set_clock_groups -group {__vtr_obj_clock_0} -group {__vtr_obj_clock_1}
set_clock_groups -exclusive -group {clk} -group {clk2}

# TODO: This is not technically correct. We need to produce warnings and only
#       set the input delays on the input ports.
# CHECK: set_input_delay -clock virtual_io_clock -max {{0.0*}} {[[in1_port_ptr]] [[in2_port_ptr]] [[out1_port_ptr]] [[out2_port_ptr]] [[clk_port_ptr]] [[clk2_port_ptr]]}
set_input_delay -clock virtual_io_clock -max 0 [get_ports {*}]

# CHECK: set_output_delay -clock virtual_io_clock -max {{0.0*}} {[[in1_port_ptr]] [[in2_port_ptr]] [[out1_port_ptr]] [[out2_port_ptr]] [[clk_port_ptr]] [[clk2_port_ptr]]}
set_output_delay -clock virtual_io_clock -max 0 [get_ports {*}]
