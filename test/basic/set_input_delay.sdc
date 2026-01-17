# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: [[clk1_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "clk1" -direction INPUT]
# CHECK-NEXT: [[in1_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "in1" -direction INPUT]
# CHECK-NEXT: [[in2_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "in2" -direction INPUT]
# CHECK-NEXT: [[in3_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "in3" -direction INPUT]
# CHECK-NEXT: [[in4_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "in4" -direction INPUT]
# CHECK-NEXT: [[A1_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "A1" -direction INPUT]
# CHECK-NEXT: [[A2_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "A2" -direction INPUT]
# CHECK-NEXT: [[A3_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "A3" -direction INPUT]
# CHECK-NEXT: [[A4_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "A4" -direction INPUT]

# CHECK: create_clock -period {{1.0*}} -waveform {{{0.0*}} {{0.50*}}} {__vtr_obj_port_0}
create_clock -period 1.0 clk1

# TODO: We should really be passing the pointer to the clock to the tool.
# CHECK: set_input_delay -clock clk1 {{0.0*}} {[[in1_port_ptr]]}
set_input_delay -clock clk1 0 in1

# CHECK: create_clock -period {{2.0*}} -waveform {{{0.0*}} {{1.0*}}} -name virtual_clock
create_clock -name virtual_clock -period 2.0

# CHECK: set_input_delay -clock virtual_clock {{1.0*}} {[[in2_port_ptr]]}
set_input_delay -clock virtual_clock 1.0 in2

# CHECK: set_input_delay -clock clk1 -max {{0.50*}} {[[in3_port_ptr]]}
set_input_delay -clock clk1 -max 0.5 in3

# CHECK: set_input_delay -clock clk1 -min {{1.0*}} {[[in4_port_ptr]]}
set_input_delay -clock clk1 -min 1 in4

# TODO: The order may not be guaranteed. Need to make this more stable.
# CHECK: set_input_delay -clock virtual_clock {{0.0*}} {[[A1_port_ptr]] [[A2_port_ptr]] [[A3_port_ptr]] [[A4_port_ptr]]}
set_input_delay -clock virtual_clock 0.0 [get_ports A*]

# CHECK: set_input_delay {{0.0*}} {[[in1_port_ptr]]}
set_input_delay 0 in1
