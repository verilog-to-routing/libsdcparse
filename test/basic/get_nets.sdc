# RUN: %sdcparse-test %s &> %t
# RUN: filecheck %s --input-file=%t

# CHECK: [[net1_ptr:[0-9]+]]
puts [libsdcparse_create_net "net1"]
# CHECK: [[net2_ptr:[0-9]+]]
puts [libsdcparse_create_net "net2"]
# CHECK: [[net3_ptr:[0-9]+]]
puts [libsdcparse_create_net "net3"]

# CHECK: [[net1_ptr]]
libsdcparse_print_object_id_list_internal [get_nets {net1}]

# CHECK: [[net2_ptr]]
libsdcparse_print_object_id_list_internal [get_nets {net2}]

# CHECK: [[net3_ptr]]
libsdcparse_print_object_id_list_internal [get_nets {net3}]

# CHECK-DAG: [[net1_ptr]]
# CHECK-DAG: [[net2_ptr]]
# CHECK-DAG: [[net3_ptr]]
# CHECK-NEXT: DONE
libsdcparse_print_object_id_list_internal [get_nets {net*}]
puts "DONE"

# CHECK-DAG: [[net1_ptr]]
# CHECK-DAG: [[net2_ptr]]
# CHECK-DAG: [[net3_ptr]]
# CHECK-NEXT: DONE
libsdcparse_print_object_id_list_internal [get_nets -regexp {net.*}]
puts "DONE"

# CHECK: [[net1_ptr]]
libsdcparse_print_object_id_list_internal [get_nets -nocase {NET1}]

# CHECK-DAG: [[net1_ptr]]
# CHECK-DAG: [[net2_ptr]]
# CHECK-DAG: [[net3_ptr]]
# CHECK-NEXT: DONE
libsdcparse_print_object_id_list_internal [get_nets -regexp -nocase {NET.*}]
puts "DONE"

# CHECK-DAG: [[net1_ptr]]
# CHECK-DAG: [[net2_ptr]]
# CHECK-DAG: [[net3_ptr]]
# CHECK-NEXT: DONE
libsdcparse_print_object_id_list_internal [get_nets *]
puts "DONE"
