# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: [[A0_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "A\[0\]" -direction INPUT]
# CHECK: [[A1_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "A\[1\]" -direction INPUT]
# CHECK: [[A2_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "A\[2\]" -direction INPUT]
# CHECK: [[A3_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "A\[3\]" -direction INPUT]
# CHECK: [[B0_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "B\[0\]" -direction INPUT]
# CHECK: [[B1_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "B\[1\]" -direction INPUT]
# CHECK: [[B2_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "B\[2\]" -direction INPUT]
# CHECK: [[B3_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "B\[3\]" -direction INPUT]

# CHECK-DAG: [[A0_ptr]]
# CHECK-DAG: [[A1_ptr]]
# CHECK-DAG: [[A2_ptr]]
# CHECK-DAG: [[A3_ptr]]
# CHECK-NEXT: DONE
puts [get_ports A*]
puts DONE

# CHECK: [[A0_ptr]]
# CHECK-NEXT: DONE
puts "get_ports A\[0\]:"
puts [get_ports A[0]]
puts DONE

# CHECK: [[A0_ptr]]
# CHECK-NEXT: DONE
puts "get_ports A\\\[0\\\]:"
puts [get_ports A\[0\]]
puts DONE

# CHECK-DAG: [[A0_ptr]]
# CHECK-DAG: [[A1_ptr]]
# CHECK-DAG: [[A2_ptr]]
# CHECK-DAG: [[A3_ptr]]
# CHECK-NEXT: DONE
puts "get_ports A\[*\]:"
puts [get_ports A[*]]
puts DONE

# CHECK-DAG: [[A0_ptr]]
# CHECK-DAG: [[A1_ptr]]
# CHECK-DAG: [[A2_ptr]]
# CHECK-DAG: [[A3_ptr]]
# CHECK-NEXT: DONE
puts "get_ports {A\\\[*\\\]}:"
puts [get_ports {A\[*\]}]
puts DONE

# CHECK-DAG: [[B0_ptr]]
# CHECK-DAG: [[B1_ptr]]
# CHECK-DAG: [[B2_ptr]]
# CHECK-DAG: [[B3_ptr]]
# CHECK-NEXT: DONE
puts "get_ports B\[*\]:"
puts [get_ports B[*]]
puts DONE

# TODO: Should we handle more interesting cases like A[3:0]?
#       This will return A[3], A[2], A[1], A[0].

# puts "get_ports A\[3:0\]:"
# puts [get_ports A[3:0]]
