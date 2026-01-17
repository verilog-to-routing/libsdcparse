# RUN: %not %sdcparse-test %s 2>&1 | filecheck %s

puts [libsdcparse_create_port "clk1" -direction INPUT]

# CHECK: Custom Error at line [[# @LINE + 1]]: get_property: The argument 'property' is required.
puts [get_property [get_ports clk1]]
