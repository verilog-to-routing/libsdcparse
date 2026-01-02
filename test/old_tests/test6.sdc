# RUN: %sdcparse-test %s 2>&1

# COM: TODO: Add the filecheck.

puts [_libsdcparse_create_port "clk" -type INPUT]
puts [_libsdcparse_create_port "clk2" -type INPUT]
puts [_libsdcparse_create_port "in1" -type INPUT]
puts [_libsdcparse_create_port "in2" -type INPUT]
puts [_libsdcparse_create_port "in3" -type INPUT]
puts [_libsdcparse_create_port "out1" -type OUTPUT]
puts [_libsdcparse_create_port "out2" -type OUTPUT]
puts [_libsdcparse_create_port "out3" -type OUTPUT]

create_clock -period 3 -waveform {1.25 2.75} clk; # rising edge at 1.25, falling at 2.75
create_clock -period 2 clk2
create_clock -period 2.5 -name virtual_io_clock
# COM: TODO: Need to verify what should be matched here. Should clk and clk2 be annotated?
set_input_delay -clock virtual_io_clock -max 1 [get_ports {*}]
set_output_delay -clock virtual_io_clock -max 0.5 [get_ports {*}]
