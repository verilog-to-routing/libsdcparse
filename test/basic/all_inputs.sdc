# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: START
# CHECK-EMPTY:
# CHECK-NEXT: END
puts "START"
puts [all_inputs]
puts "END"

# CHECK: [[in1_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "in1" -direction INPUT]

# CHECK: [[in1_ptr]]
puts [all_inputs]

# CHECK: [[in2_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "in2" -direction INPUT]

# CHECK-DAG: [[in1_ptr]]
# CHECK-DAG: [[in2_ptr]]
# CHECK-NEXT: DONE
puts [all_inputs]
puts DONE

puts [libsdcparse_create_port "out1" -direction OUTPUT]

# CHECK-DAG: [[in1_ptr]]
# CHECK-DAG: [[in2_ptr]]
# CHECK-NEXT: DONE
puts [all_inputs]
puts DONE

# CHECK: [[inout1_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port "inout1" -direction INOUT]

# CHECK-DAG: [[inout1_ptr]]
# CHECK-DAG: [[in1_ptr]]
# CHECK-DAG: [[in2_ptr]]
# CHECK-NEXT: DONE
puts [all_inputs]
puts DONE
