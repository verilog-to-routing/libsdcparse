# RUN: %sdcparse-test %s 2>&1 | filecheck %s

puts [_libsdcparse_create_port "clk1" -direction INPUT]
puts [_libsdcparse_create_pin "pin1" -direction INPUT]
puts [_libsdcparse_create_cell "cell1"]

# CHECK: clk1
puts [get_name [get_ports clk1]]

# CHECK: pin1
puts [get_name [get_pins pin1]]

# CHECK: cell1
puts [get_name [get_cells cell1]]

create_clock -period 1 [get_ports clk1] -name CLOCK

# CHECK: CLOCK
puts [get_name [get_clocks CLOCK]]
