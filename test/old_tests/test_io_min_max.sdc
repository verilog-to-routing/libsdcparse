# RUN: %sdcparse-test %s 2>&1

# COM: TODO: Add the filecheck.

puts [_libsdcparse_create_port "in1" -type INPUT]
puts [_libsdcparse_create_port "in2" -type INPUT]
puts [_libsdcparse_create_port "in3" -type INPUT]
puts [_libsdcparse_create_port "out1" -type INPUT]
puts [_libsdcparse_create_port "out2" -type INPUT]

#I/O Delay
set_input_delay -clock input_clk -max -min 0.5 [get_ports {in1 in2 in3}]
set_output_delay -clock output_clk -max -min 1 [get_ports {out*}]
set_input_delay -clock input_clk 0.5 [get_ports {in1 in2 in3}]
set_output_delay -clock output_clk 1 [get_ports {out*}]
