# RUN: %sdcparse-test %s 2>&1

# COM: TODO: Add the filecheck.

puts [_libsdcparse_create_port "clk" -type INPUT]
puts [_libsdcparse_create_port "clk2" -type INPUT]
puts [_libsdcparse_create_port "clk_extra" -type INPUT]
puts [_libsdcparse_create_port "clk_pll" -type INPUT]
puts [_libsdcparse_create_port "clk2_pll" -type INPUT]
# COM: TODO: Make this more interesting by adding output ports.

create_clock -period 1.0 *_pll
