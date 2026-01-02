# RUN: %sdcparse-test %s 2>&1 | filecheck %s

puts [_libsdcparse_create_port "clk" -type INPUT]
puts [_libsdcparse_create_port "clk2" -type INPUT]

# CHECK: #{{.*\.sdc}}:[[# @LINE + 2]]
# CHECK-NEXT: create_clock
create_clock -period 1.0 clk

# CHECK: #{{.*\.sdc}}:[[# @LINE + 2]]
# CHECK-NEXT: create_clock
create_clock -period 1.0 clk2

# CHECK: #{{.*\.sdc}}:[[# @LINE + 2]]
# CHECK-NEXT: set_clock_groups
set_clock_groups -exclusive -group clk -group clk2

# CHECK: #{{.*\.sdc}}:[[# @LINE + 2]]
# CHECK-NEXT: set_false_path
set_false_path -from clk -to clk2

# CHECK: #{{.*\.sdc}}:[[# @LINE + 2]]
# CHECK-NEXT: set_max_delay
set_max_delay -from clk -to clk2 0.0

# CHECK: #{{.*\.sdc}}:[[# @LINE + 2]]
# CHECK-NEXT: set_min_delay
set_min_delay -from clk -to clk2 0.0

# COM: TODO: Should have all commands here for a strong test.
