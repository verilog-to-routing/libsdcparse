# RUN: %sdcparse-test %s 2>&1

# COM: TODO: Add the filecheck.

puts [_libsdcparse_create_port "clka" -direction INPUT]
puts [_libsdcparse_create_port "clkb" -direction INPUT]
puts [_libsdcparse_create_port "clkc" -direction INPUT]
puts [_libsdcparse_create_port "clkd" -direction INPUT]
create_clock -period 1.0 clka
create_clock -period 1.0 clkb
create_clock -period 1.0 clkc
create_clock -period 1.0 clkd

set_clock_uncertainty -setup -from clka -to clkb 0.3

set_clock_uncertainty -hold -from clka -to clkb 0.2

set_clock_uncertainty -hold -from {clka clkc} -to clkb 0.2

set_clock_uncertainty -hold -from {clka clkc} -to {clkb clkd} 0.2

set_clock_uncertainty -setup -from [get_clocks {clka clkc}] -to {clkb clkd} 0.2
