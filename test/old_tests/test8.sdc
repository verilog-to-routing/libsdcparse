# RUN: %sdcparse-test %s 2>&1

# COM: TODO: Add the filecheck.

puts [_libsdcparse_create_port "clk" -direction INPUT]
puts [_libsdcparse_create_port "clk2" -direction INPUT]
puts [_libsdcparse_create_port "clk_extra" -direction INPUT]
puts [_libsdcparse_create_port "clk_pll" -direction INPUT]
puts [_libsdcparse_create_port "clk2_pll" -direction INPUT]
# COM: TODO: Make this more interesting by adding output ports.

create_clock -period 1.0 *_pll
