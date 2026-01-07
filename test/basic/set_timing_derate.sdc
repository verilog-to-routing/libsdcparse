# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: [[cell1_ptr:__vtr_obj_cell_[0-9]+]]
puts [_libsdcparse_create_cell "cell1"]
# CHECK: [[cell2_ptr:__vtr_obj_cell_[0-9]+]]
puts [_libsdcparse_create_cell "cell2"]

# CHECK: set_timing_derate {{1.0*}} {[[cell1_ptr]]}
set_timing_derate 1.0 [get_cells "cell1"]

# CHECK: set_timing_derate -early {{2.50*}} {[[cell1_ptr]]}
set_timing_derate 2.5 -early [get_cells "cell1"]

# CHECK: set_timing_derate -late {{3.10*}} {[[cell1_ptr]]}
set_timing_derate 3.1 -late [get_cells "cell1"]

# CHECK: set_timing_derate -early -late {{4.60*}} {[[cell1_ptr]]}
set_timing_derate 4.6 -early -late [get_cells "cell1"]

# CHECK: set_timing_derate -net_delay {{5.70*}} {[[cell1_ptr]]}
set_timing_derate 5.7 -net_delay [get_cells "cell1"]

# CHECK: set_timing_derate -cell_delay {{6.80*}} {[[cell1_ptr]]}
set_timing_derate 6.8 -cell_delay [get_cells "cell1"]

# CHECK: set_timing_derate {{7.50*}} {[[cell1_ptr]] [[cell2_ptr]]}
set_timing_derate 7.5 [get_cells {cell1 cell2}]
