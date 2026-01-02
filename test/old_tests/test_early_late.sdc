# RUN: %sdcparse-test %s 2>&1

# COM: TODO: Add the filecheck.

puts [_libsdcparse_create_port "clk1" -direction INPUT]
puts [_libsdcparse_create_port "clk2" -direction INPUT]
puts [_libsdcparse_create_port "clk3" -direction INPUT]

create_clock -period 1 [get_ports clk1]
create_clock -period 1 [get_ports clk2]
create_clock -period 1 [get_ports clk3]

#Latency
set_clock_latency -source -early -late 3.4 [get_clocks clk*]
set_clock_latency -source 3.4 [get_clocks clk*]

#Derate
set_timing_derate 0.9
set_timing_derate -early -late 0.9
