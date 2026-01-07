# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: [[in1_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "in1" -direction INPUT]
# CHECK: [[in2_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "in2" -direction INPUT]
# CHECK: [[in3_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "in3" -direction INPUT]

# CHECK: [[in1_ptr]]
puts [get_ports {in1}]

# CHECK: [[in2_ptr]]
puts [get_ports {in2}]

# CHECK: [[in3_ptr]]
puts [get_ports {in3}]

# CHECK-DAG: [[in1_ptr]]
# CHECK-DAG: [[in2_ptr]]
# CHECK-DAG: [[in3_ptr]]
# CHECK-NEXT: DONE
puts [get_ports {in*}]
puts "DONE"

# CHECK-DAG: [[in1_ptr]]
# CHECK-DAG: [[in2_ptr]]
# CHECK-DAG: [[in3_ptr]]
# CHECK-NEXT: DONE
puts [get_ports -regexp {in.*}]
puts "DONE"

# CHECK: [[in1_ptr]]
puts [get_ports -nocase {IN1}]

# CHECK-DAG: [[in1_ptr]]
# CHECK-DAG: [[in2_ptr]]
# CHECK-DAG: [[in3_ptr]]
# CHECK-NEXT: DONE
puts [get_ports -regexp -nocase {IN.*}]
puts "DONE"

# CHECK-DAG: [[in1_ptr]]
# CHECK-DAG: [[in2_ptr]]
# CHECK-DAG: [[in3_ptr]]
# CHECK-NEXT: DONE
puts [get_ports *]
puts "DONE"
