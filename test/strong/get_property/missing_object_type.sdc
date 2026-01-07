# RUN: %not %sdcparse-test %s 2>&1 | filecheck %s

puts [_libsdcparse_create_port "clk1" -direction INPUT]

# CHECK: Custom Error at line [[# @LINE + 1]]: get_property: -object_type is required if object is a name
puts [get_property clk1 name]
