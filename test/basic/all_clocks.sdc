# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: START
# CHECK-EMPTY:
# CHECK-NEXT: END
puts "START"
puts [all_clocks]
puts "END"

libsdcparse_create_port "clk1" -direction INPUT
create_clock -period 1.0 [get_ports clk1]

# CHECK: clk1: [[clk1_ptr:__vtr_obj_clock_[0-9]+]]
puts "clk1: [get_clocks clk1]"

# CHECK: [[clk1_ptr]]
puts [all_clocks]

libsdcparse_create_port "clk2" -direction INPUT
create_clock -period 1.0 [get_ports clk2]

# CHECK: clk2: [[clk2_ptr:__vtr_obj_clock_[0-9]+]]
puts "clk2: [get_clocks clk2]"

# CHECK-DAG: [[clk1_ptr]]
# CHECK-DAG: [[clk2_ptr]]
# CHECK-NEXT: DONE
puts [all_clocks]
puts DONE