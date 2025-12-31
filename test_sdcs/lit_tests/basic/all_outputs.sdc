# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: START
# CHECK-EMPTY:
# CHECK-NEXT: END
puts "START"
puts [all_inputs]
puts "END"

# CHECK: [[out1_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "out1" -type OUTPUT]

# CHECK: [[out1_ptr]]
puts [all_outputs]

# CHECK: [[out2_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "out2" -type OUTPUT]

# CHECK-DAG: [[out1_ptr]]
# CHECK-DAG: [[out2_ptr]]
# CHECK-NEXT: DONE
puts [all_outputs]
puts DONE

puts [_libsdcparse_create_port "in1" -type INPUT]

# CHECK-DAG: [[out1_ptr]]
# CHECK-DAG: [[out2_ptr]]
# CHECK-NEXT: DONE
puts [all_outputs]
puts DONE

# CHECK: [[inout1_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "inout1" -type INOUT]

# CHECK-DAG: [[inout1_ptr]]
# CHECK-DAG: [[out1_ptr]]
# CHECK-DAG: [[out2_ptr]]
# CHECK-NEXT: DONE
puts [all_outputs]
puts DONE
