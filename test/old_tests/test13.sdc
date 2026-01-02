# RUN: %sdcparse-test %s 2>&1

# COM: TODO: Add the filecheck.

puts [_libsdcparse_create_port "clk1" -direction INPUT]
puts [_libsdcparse_create_port "clk2" -direction INPUT]
puts [_libsdcparse_create_port "clk3" -direction INPUT]

create_clock -period 1 [get_ports clk1]
create_clock -period 1 [get_ports clk2]
create_clock -period 1 [get_ports clk3]

set_clock_latency -source -late 3.4 [get_clocks clk1]
set_clock_latency -source -late 3.4 [get_clocks clk*]
set_clock_latency -source -early 3.4 [get_clocks {clk2}]
set_clock_latency -source 3.4 [get_clocks {clk3}]
