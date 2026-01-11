# RUN: %not %sdcparse-test %s 2>&1 | filecheck %s

puts [libsdcparse_create_port "clk1" -direction INPUT]
puts [libsdcparse_create_port "clk2" -direction INPUT]

create_clock -period 1 clk1
create_clock -period 1 clk2

# CHECK: Custom Error at line [[# @LINE + 1]]: set_clock_groups: Exactly one of the arguments {'-logically_exclusive', '-physically_exclusive', and '-asynchronous'} is required.
set_clock_groups -logically_exclusive -asynchronous -group clk1 -group clk2
