# RUN: %sdcparse-test %s 2>&1 | filecheck %s

puts [libsdcparse_create_port "clk1" -direction INPUT]
puts [libsdcparse_create_pin "pin1" -direction INPUT]
puts [libsdcparse_create_cell "cell1"]

# CHECK: clk1
puts [get_property [get_ports clk1] name]

# CHECK: pin1
puts [get_property [get_pins pin1] name]

# CHECK: cell1
puts [get_property [get_cells cell1] name]

create_clock -period 1 [get_ports clk1] -name CLOCK

# CHECK: CLOCK
puts [get_property [get_clocks CLOCK] name]

# CHECK: clk1
puts [get_property clk1 name -object_type port]
# CHECK: pin1
puts [get_property pin1 name -object_type pin]
# CHECK: cell1
puts [get_property cell1 name -object_type cell]
# CHECK: CLOCK
puts [get_property CLOCK name -object_type clock]
