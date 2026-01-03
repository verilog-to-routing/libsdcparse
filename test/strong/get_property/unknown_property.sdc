# RUN: %not %sdcparse-test %s 2>&1 | filecheck %s

puts [_libsdcparse_create_port "clk1" -direction INPUT]

# CHECK: Custom Error at line [[# @LINE + 1]]: get_property: Unknown property: 'unknown_property'
puts [get_property clk1 unknown_property]
