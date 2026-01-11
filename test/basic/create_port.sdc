# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# NOTE: This test does not have to use these exact port names, we just want
#       to make sure that these ports are actually getting made. Should make
#       this more robust.

# CHECK: __vtr_obj_port_0
puts [libsdcparse_create_port in1 -direction INPUT]

# CHECK: __vtr_obj_port_1
puts [libsdcparse_create_port in1 -direction INPUT]
