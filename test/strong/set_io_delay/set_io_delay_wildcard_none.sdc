# RUN: %not %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: [[in1_port_ptr:__vtr_obj_[0-9]+]]
puts [libsdcparse_create_port "in1" -direction INPUT]

# CHECK: set_input_delay: Unknown target(s): *
set_input_delay -clock * -max 0.5 [get_ports in1]