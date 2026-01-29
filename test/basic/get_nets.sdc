# RUN: %sdcparse-test %s &> %t
# RUN: filecheck %s --input-file=%t

# CHECK: [[net1_ptr:__vtr_obj_net_[0-9]+]]
puts [libsdcparse_create_net "net1"]
# CHECK: [[net2_ptr:__vtr_obj_net_[0-9]+]]
puts [libsdcparse_create_net "net2"]
# CHECK: [[net3_ptr:__vtr_obj_net_[0-9]+]]
puts [libsdcparse_create_net "net3"]

# CHECK: [[net1_ptr]]
puts [get_nets {net1}]

# CHECK: [[net2_ptr]]
puts [get_nets {net2}]

# CHECK: [[net3_ptr]]
puts [get_nets {net3}]

# CHECK-DAG: [[net1_ptr]]
# CHECK-DAG: [[net2_ptr]]
# CHECK-DAG: [[net3_ptr]]
# CHECK-NEXT: DONE
puts [get_nets {net*}]
puts "DONE"

# CHECK-DAG: [[net1_ptr]]
# CHECK-DAG: [[net2_ptr]]
# CHECK-DAG: [[net3_ptr]]
# CHECK-NEXT: DONE
puts [get_nets -regexp {net.*}]
puts "DONE"

# CHECK: [[net1_ptr]]
puts [get_nets -nocase {NET1}]

# CHECK-DAG: [[net1_ptr]]
# CHECK-DAG: [[net2_ptr]]
# CHECK-DAG: [[net3_ptr]]
# CHECK-NEXT: DONE
puts [get_nets -regexp -nocase {NET.*}]
puts "DONE"

# CHECK-DAG: [[net1_ptr]]
# CHECK-DAG: [[net2_ptr]]
# CHECK-DAG: [[net3_ptr]]
# CHECK-NEXT: DONE
puts [get_nets *]
puts "DONE"
