# RUN: %sdcparse-test %s 2>&1

# COM: TODO: Add the filecheck.

puts [libsdcparse_create_port "clk" -direction INPUT]
puts [libsdcparse_create_port "output_clk" -direction INPUT]
puts [libsdcparse_create_cell "asdf~/ff"]
puts [libsdcparse_create_cell "wer/234/ff3"]
puts [libsdcparse_create_cell "xcw/32|ff2"]
puts [libsdcparse_create_cell "asdf/ff"]
puts [libsdcparse_create_cell "qwert/asd/ff"]
puts [libsdcparse_create_cell "asdf/ff2"]

create_clock -period 1 [get_ports clk]
create_clock -period 1 [get_ports output_clk]

set_min_delay 2     -from [get_clocks {clk}]        -to [get_clocks {output_clk}]; #Clocks
set_min_delay 3.6   -from {asdf~/ff}                -to {wer/234/ff3 xcw/32|ff2}; #Objects
set_min_delay .3    -from {asdf/ff qwert/asd/ff}    -to [get_clocks {output_clk}]; #Mixed Clocks/Objects
set_min_delay 0.    -from [get_clocks {output_clk}] -to {asdf/ff2}; #Mixed Clocks/Objects
