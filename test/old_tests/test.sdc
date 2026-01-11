# RUN: %sdcparse-test %s &> %t
# RUN: filecheck %s --input-file=%t

# CHECK: [[clk0_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "clk0" -direction INPUT]
# CHECK: [[clk1_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "clk1" -direction INPUT]
# CHECK: [[clk2_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "clk2" -direction INPUT]
# CHECK: [[clk3_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "clk3" -direction INPUT]
# CHECK: [[clk4_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "clk4" -direction INPUT]
# CHECK: [[top_clk_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "top^clk" -direction INPUT]
# CHECK: [[asdf_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "asdf" -direction INPUT]
# CHECK: [[qwerty_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "qwerty" -direction INPUT]
# CHECK: [[in1_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "in1" -direction INPUT]
# CHECK: [[in2_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "in2" -direction INPUT]
# CHECK: [[in3_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "in3" -direction INPUT]
# CHECK: [[out1_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "out1" -direction INPUT]
# CHECK: [[out2_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "out2" -direction INPUT]
# CHECK: [[eof_test_port1_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "eof_test_port1" -direction INPUT]
# CHECK: [[eof_test_port2_port_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "eof_test_port2" -direction INPUT]

# CHECK: [[asdf_tff_cell_ptr:__vtr_obj_cell_[0-9]+]]
puts [libsdcparse_create_cell "asdf~/ff"]
# CHECK: [[wer_cell_ptr:__vtr_obj_cell_[0-9]+]]
puts [libsdcparse_create_cell "wer/234/ff3"]
# CHECK: [[xcw_cell_ptr:__vtr_obj_cell_[0-9]+]]
puts [libsdcparse_create_cell "xcw/32|ff2"]
# CHECK: [[asdf_ff_cell_ptr:__vtr_obj_cell_[0-9]+]]
puts [libsdcparse_create_cell "asdf/ff"]
# CHECK: [[qwert_cell_ptr:__vtr_obj_cell_[0-9]+]]
puts [libsdcparse_create_cell "qwert/asd/ff"]
# CHECK: [[asdf_ff2_cell_ptr:__vtr_obj_cell_[0-9]+]]
puts [libsdcparse_create_cell "asdf/ff2"]

# CHECK: [[my_inst_in0_pin_ptr:__vtr_obj_pin_[0-9]+]]
puts [libsdcparse_create_pin "my_inst/in\[0\]" -direction INPUT]
# CHECK: [[my_inst_out0_pin_ptr:__vtr_obj_pin_[0-9]+]]
puts [libsdcparse_create_pin "my_inst2/out\[0\]" -direction OUTPUT]

#Netlist Clocks
# CHECK: create_clock -period {{3.0*}} -waveform {{{1.250*}} {{2.750*}}} {[[clk0_port_ptr]]}
create_clock -period 3 -waveform {1.25 2.75} clk0; #Integer period, float waveform
# CHECK: create_clock -period {{3.0*}} -waveform {{{1.0*}} {{2.0*}}} {[[clk1_port_ptr]]}
create_clock -period 3 -waveform {1 2} clk1; #Integer period, integer waveform
# CHECK: create_clock -period {{2.30*}} -waveform {{{0.0*}} {{1.150*}}} {[[clk2_port_ptr]]}
create_clock -period 2.3 clk2; #Float period
# TODO: The order is not guaranteed.
# CHECK: create_clock -period {{2.0*}} -waveform {{{0.0*}} {{1.0*}}} {[[clk3_port_ptr]] [[clk4_port_ptr]]}
create_clock -period 2 {clk3 clk4}; #Multiple targets
# CHECK: create_clock -period {{0.0*}} -waveform {{{0.0*}} {{0.0*}}} {[[top_clk_port_ptr]]}
create_clock -period 0 {top^clk}
# CHECK: create_clock -period {{0.0*}} -waveform {{{0.0*}} {{0.0*}}} {[[asdf_port_ptr]]}
create_clock -period 0 {asdf}
# CHECK: create_clock -period {{0.0*}} -waveform {{{0.0*}} {{0.0*}}} {[[qwerty_port_ptr]]}
create_clock -period 0 {qwerty}


#Virtual Clocks
# CHECK: create_clock -period {{1.0*}} -waveform {{{0.0*}} {{0.50*}}} -name input_clk
create_clock -period 1 -name input_clk
# CHECK: create_clock -period {{1.0*}} -waveform {{{0.0*}} {{0.50*}}} -name input_clock2
create_clock -period 1 -name input_clock2
# CHECK: create_clock -period {{0.0*}} -waveform {{{0.0*}} {{0.0*}}} -name output_clk
create_clock -period 0 -name output_clk; #Zero period

# CHECK: clk0: [[clk0_clock_ptr:__vtr_obj_clock_[0-9]+]]
puts "clk0: [get_clocks clk0]"
# CHECK: clk2: [[clk2_clock_ptr:__vtr_obj_clock_[0-9]+]]
puts "clk2: [get_clocks clk2]"
# CHECK: clk3: [[clk3_clock_ptr:__vtr_obj_clock_[0-9]+]]
puts "clk3: [get_clocks clk3]"
# CHECK: input_clk: [[input_clk_clock_ptr:__vtr_obj_clock_[0-9]+]]
puts "input_clk: [get_clocks input_clk]"
# CHECK: input_clock2: [[input_clock2_clock_ptr:__vtr_obj_clock_[0-9]+]]
puts "input_clock2: [get_clocks input_clock2]"
# CHECK: asdf: [[asdf_clock_ptr:__vtr_obj_clock_[0-9]+]]
puts "asdf: [get_clocks asdf]"
# CHECK: qwerty: [[qwerty_clock_ptr:__vtr_obj_clock_[0-9]+]]
puts "qwerty: [get_clocks qwerty]"
# CHECK: output_clk: [[output_clk_clock_ptr:__vtr_obj_clock_[0-9]+]]
puts "output_clk: [get_clocks output_clk]"

#Clock Groups
# CHECK: set_clock_groups -asynchronous -group {[[input_clk_clock_ptr]]} -group {[[clk2_clock_ptr]]} -group {[[clk3_clock_ptr]]}
set_clock_groups -asynchronous -group input_clk -group {clk2} -group [get_clocks {clk3}]; #Single element
# TODO: Order is not guaranteed.
# CHECK: set_clock_groups -asynchronous -group {[[input_clk_clock_ptr]] [[input_clock2_clock_ptr]]} -group {[[clk2_clock_ptr]]} -group {[[asdf_clock_ptr]] [[qwerty_clock_ptr]]}
set_clock_groups -asynchronous -group {input_clk input_clock2} -group {clk2} -group [get_clocks {asdf qwerty}]; #Multiple string elements

#False Path
# CHECK: set_false_path -from {[[clk0_clock_ptr]]} -to {[[output_clk_clock_ptr]]}
set_false_path -from [get_clocks {clk0}]          -to [get_clocks {output_clk}]; #Clocks
# CHECK: set_false_path -from {[[asdf_tff_cell_ptr]]} -to {[[wer_cell_ptr]] [[xcw_cell_ptr]]}
set_false_path -from {asdf~/ff}                 -to {wer/234/ff3 xcw/32|ff2}; #Objects
# CHECK: set_false_path -from {[[asdf_ff_cell_ptr]] [[qwert_cell_ptr]]} -to {[[output_clk_clock_ptr]]}
set_false_path -from {asdf/ff qwert/asd/ff}     -to [get_clocks {output_clk}]; #Mixed Clocks/Objects
# CHECK: set_false_path -from {[[output_clk_clock_ptr]]} -to {[[asdf_ff2_cell_ptr]]}
set_false_path -from [get_clocks {output_clk}]   -to {asdf/ff2}; #Mixed Clocks/Objects

# COM: TODO: Add more checks to the rest of this file.

#Max Delay
set_max_delay 2     -from [get_clocks {clk0}]         -to [get_clocks {output_clk}]; #Clocks
set_max_delay 3.6   -from {asdf~/ff}                -to {wer/234/ff3 xcw/32|ff2}; #Objects
set_max_delay .3    -from {asdf/ff qwert/asd/ff}    -to [get_clocks {output_clk}]; #Mixed Clocks/Objects
set_max_delay 0.    -from [get_clocks {output_clk}]  -to {asdf/ff2}; #Mixed Clocks/Objects

#MCP
set_multicycle_path 2 -setup -from [get_clocks {clk0}]        -to [get_clocks {output_clk}]; #Clocks
set_multicycle_path 3 -setup -from {asdf~/ff}               -to {wer/234/ff3 xcw/32|ff2}; #Objects
set_multicycle_path 3 -setup -from {asdf/ff qwert/asd/ff}   -to [get_clocks {output_clk}]; #Mixed Clocks/Objects
set_multicycle_path 0 -setup -from [get_clocks {output_clk}] -to {asdf/ff2}; #Mixed Clocks/Objects
set_multicycle_path 3 -hold -from {asdf/ff qwert/asd/ff}   -to [get_clocks {output_clk}]; #hold
set_multicycle_path 2 -from [get_clocks {clk0}]        -to [get_clocks {output_clk}]; #Clocks
set_multicycle_path 2 -to [get_pins {my_inst/in[0]}]; #Pin targets
set_multicycle_path 2 -from [get_pins {my_inst/in[0]}]; #Pin targets
set_multicycle_path 2 -to [get_pins {myinst2/out[3]}] -from [get_pins {my_inst/in[0]}]; #Pin targets

#I/O Delay
set_input_delay -clock input_clk -max 0.5 [get_ports {in1 in2 in3}]
set_output_delay -clock output_clk -max 1 [get_ports {out*}]

#Line continuation
create_clock -period \
2 {clk3 \
clk4} \
#asdf

#Spaced Comments

#Non-empty line at end of file
set_output_delay -clock eof_test -max 1.7293 [ get_ports {eof_test_port*} ]
