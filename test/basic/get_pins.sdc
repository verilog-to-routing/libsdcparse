# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: [[in1_ptr:__vtr_obj_pin_[0-9]+]]
puts [_libsdcparse_create_pin "in1" -direction INPUT]
# CHECK: [[in2_ptr:__vtr_obj_pin_[0-9]+]]
puts [_libsdcparse_create_pin "in2" -direction INPUT]
# CHECK: [[in3_ptr:__vtr_obj_pin_[0-9]+]]
puts [_libsdcparse_create_pin "in3" -direction INPUT]

# CHECK: [[in1_ptr]]
puts [get_pins {in1}]

# CHECK: [[in2_ptr]]
puts [get_pins {in2}]

# CHECK: [[in3_ptr]]
puts [get_pins {in3}]

# CHECK-DAG: [[in1_ptr]]
# CHECK-DAG: [[in2_ptr]]
# CHECK-DAG: [[in3_ptr]]
# CHECK-NEXT: DONE
puts [get_pins {in*}]
puts "DONE"

# CHECK-DAG: [[in1_ptr]]
# CHECK-DAG: [[in2_ptr]]
# CHECK-DAG: [[in3_ptr]]
puts [get_pins -regexp {in.*}]
puts "DONE"

# CHECK: [[in1_ptr]]
puts [get_pins -nocase {IN1}]

# CHECK-DAG: [[in1_ptr]]
# CHECK-DAG: [[in2_ptr]]
# CHECK-DAG: [[in3_ptr]]
puts [get_pins -regexp -nocase {IN.*}]
puts "DONE"

# CHECK-DAG: [[in1_ptr]]
# CHECK-DAG: [[in2_ptr]]
# CHECK-DAG: [[in3_ptr]]
# CHECK-NEXT: DONE
puts [get_pins *]
puts "DONE"
