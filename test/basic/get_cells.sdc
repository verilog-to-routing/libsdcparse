# RUN: %sdcparse-test %s &> %t
# RUN: filecheck %s --input-file=%t

# CHECK: [[cell1_ptr:__vtr_obj_cell_[0-9]+]]
puts [_libsdcparse_create_cell "cell1"]
# CHECK: [[cell2_ptr:__vtr_obj_cell_[0-9]+]]
puts [_libsdcparse_create_cell "cell2"]
# CHECK: [[cell3_ptr:__vtr_obj_cell_[0-9]+]]
puts [_libsdcparse_create_cell "cell3"]

# CHECK: [[cell1_ptr]]
puts [get_cells {cell1}]

# CHECK: [[cell2_ptr]]
puts [get_cells {cell2}]

# CHECK: [[cell3_ptr]]
puts [get_cells {cell3}]

# CHECK-DAG: [[cell1_ptr]]
# CHECK-DAG: [[cell2_ptr]]
# CHECK-DAG: [[cell3_ptr]]
# CHECK-NEXT: DONE
puts [get_cells {cell*}]
puts "DONE"

# CHECK-DAG: [[cell1_ptr]]
# CHECK-DAG: [[cell2_ptr]]
# CHECK-DAG: [[cell3_ptr]]
puts [get_cells -regexp {cell.*}]
puts "DONE"

# CHECK: [[cell1_ptr]]
puts [get_cells -nocase {CELL1}]

# CHECK-DAG: [[cell1_ptr]]
# CHECK-DAG: [[cell2_ptr]]
# CHECK-DAG: [[cell3_ptr]]
puts [get_cells -regexp -nocase {CELL.*}]
puts "DONE"

# CHECK-DAG: [[cell1_ptr]]
# CHECK-DAG: [[cell2_ptr]]
# CHECK-DAG: [[cell3_ptr]]
# CHECK-NEXT: DONE
puts [get_cells *]
puts "DONE"
