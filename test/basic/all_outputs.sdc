# RUN: %sdcparse-test %s &> %t
# RUN: filecheck %s --input-file=%t

# CHECK: START
# CHECK-EMPTY:
# CHECK-NEXT: END
puts "START"
puts [all_inputs]
puts "END"

# CHECK: [[out1_ptr:[0-9]+]]
puts [libsdcparse_create_port "out1" -direction OUTPUT]

# CHECK: [[out1_ptr]]
libsdcparse_print_object_id_list_internal [all_outputs]

# CHECK: [[out2_ptr:[0-9]+]]
puts [libsdcparse_create_port "out2" -direction OUTPUT]

# CHECK-DAG: [[out1_ptr]]
# CHECK-DAG: [[out2_ptr]]
# CHECK-NEXT: DONE
libsdcparse_print_object_id_list_internal [all_outputs]
puts DONE

puts [libsdcparse_create_port "in1" -direction INPUT]

# CHECK-DAG: [[out1_ptr]]
# CHECK-DAG: [[out2_ptr]]
# CHECK-NEXT: DONE
libsdcparse_print_object_id_list_internal [all_outputs]
puts DONE

# CHECK: [[inout1_ptr:[0-9]+]]
puts [libsdcparse_create_port "inout1" -direction INOUT]

# CHECK-DAG: [[inout1_ptr]]
# CHECK-DAG: [[out1_ptr]]
# CHECK-DAG: [[out2_ptr]]
# CHECK-NEXT: DONE
libsdcparse_print_object_id_list_internal [all_outputs]
puts DONE
