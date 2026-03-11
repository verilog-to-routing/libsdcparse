# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: [[net1_ptr:[0-9]+]]
puts [libsdcparse_create_net {\u}]

# CHECK: \u
puts [get_name [get_nets *]]

# CHECK: [[net1_ptr]]
puts [get_nets {\\u}]
