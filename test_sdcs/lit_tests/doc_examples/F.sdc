# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: [[in1_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "in1" -type INPUT]
# CHECK-NEXT: [[in2_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "in2" -type INPUT]
# CHECK-NEXT: [[in3_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "in3" -type INPUT]
# CHECK-NEXT: [[out1_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "out1" -type OUTPUT]
# CHECK-NEXT: [[out2_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "out2" -type OUTPUT]
# CHECK-NEXT: [[clk_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "clk" -type INPUT]
# CHECK-NEXT: [[clk2_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "clk2" -type INPUT]

# TODO: Need better support for square brackets.
# CHECK-NEXT: [[FFAQ0_pin_ptr:__vtr_obj_pin_[0-9]+]]
puts [_libsdcparse_create_pin "FFA.Q0" -type OUTPUT]
# CHECK-NEXT: [[FFDin0_pin_ptr:__vtr_obj_pin_[0-9]+]]
puts [_libsdcparse_create_pin "to_FFD.in0" -type INPUT]

# CHECK: create_clock -period {{3.0*}} -waveform {{{1.250*}} {{2.750*}}} {[[clk_port_ptr]]}
create_clock -period 3 -waveform {1.25 2.75} clk

# CHECK: create_clock -period {{2.0*}} -waveform {{{0.0*}} {{1.0*}}} {[[clk2_port_ptr]]}
create_clock -period 2 clk2

# CHECK: create_clock -period {{1.0*}} -waveform {{{0.0*}} {{0.50*}}} -name input_clk
create_clock -period 1 -name input_clk

# CHECK: create_clock -period {{0.0*}} -waveform {{{0.0*}} {{0.0*}}} -name output_clk
create_clock -period 0 -name output_clk

# TODO: Need to update the testcase in the docs. It should be invalid to remove the
#       space after the commands
# CHECK: set_clock_latency -source {{1.0*}} {__vtr_obj_clock_0}
set_clock_latency -source 1.0 [get_clocks {clk}]

# CHECK: set_clock_groups -group {__vtr_obj_clock_2} -group {__vtr_obj_clock_1}
set_clock_groups -exclusive -group input_clk -group clk2

# CHECK: set_false_path -from {__vtr_obj_clock_0} -to {__vtr_obj_clock_3}
set_false_path -from [get_clocks {clk}] -to [get_clocks {output_clk}]

# TODO: The order here is not guarenteed.
# CHECK: set_input_delay -clock input_clk -max {{0.50*}} {[[in1_port_ptr]] [[in2_port_ptr]] [[in3_port_ptr]]}
set_input_delay -clock input_clk -max 0.5 [get_ports {in1 in2 in3}]

# TODO: The order here is not guarenteed.
# CHECK: set_output_delay -clock output_clk -max {{1.0*}} {[[out1_port_ptr]] [[out2_port_ptr]]}
set_output_delay -clock output_clk -max 1 [get_ports {out*}]

# CHECK: set_max_delay {{17.0*}} -from {__vtr_obj_clock_2} -to {__vtr_obj_clock_3}
set_max_delay 17 -from [get_clocks {input_clk}] -to [get_clocks {output_clk}]

# CHECK: set_min_delay {{2.0*}} -from {__vtr_obj_clock_2} -to {__vtr_obj_clock_3}
set_min_delay 2 -from [get_clocks {input_clk}] -to [get_clocks {output_clk}]

# CHECK: set_multicycle_path 3 -setup -from {__vtr_obj_clock_0} -to {__vtr_obj_clock_1}
set_multicycle_path -setup -from [get_clocks {clk}] -to [get_clocks {clk2}] 3

# CHECK: set_clock_uncertainty -from {__vtr_obj_clock_0} -to {__vtr_obj_clock_1} {{0.750*}}
set_clock_uncertainty -from [get_clocks {clk}] -to [get_clocks {clk2}] 0.75

# TODO: Need to handle square brackets better.
# CHECK: set_disable_timing -from {[[FFAQ0_pin_ptr]]} -to {[[FFDin0_pin_ptr]]}
set_disable_timing -from [get_pins {FFA.Q0}] -to [get_pins {to_FFD.in0}]
