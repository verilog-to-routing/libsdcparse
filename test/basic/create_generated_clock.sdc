# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: [[clk1_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "clk1" -direction INPUT]
# CHECK: [[gen_clk1_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "gen_clk1" -direction INPUT]
# CHECK: [[gen_clk2_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "gen_clk2" -direction INPUT]
# CHECK: [[gen_clk3_port_ptr:__vtr_obj_port_[0-9]+]]
puts [_libsdcparse_create_port "gen_clk3" -direction INPUT]

create_clock -period 1.0 clk1

# CHECK: create_generated_clock -source clk1 -divide_by 4 {[[gen_clk1_port_ptr]]}
create_generated_clock -source clk1 -divide_by 4 gen_clk1

# CHECK: gen_clk1: {{__vtr_obj_clock_[0-9]+}}
puts "gen_clk1: [get_clocks gen_clk1]"

# CHECK: create_generated_clock -source clk1 -multiply_by 5 {[[gen_clk2_port_ptr]]}
create_generated_clock -source clk1 -multiply_by 5 gen_clk2

# CHECK: create_generated_clock -name gen_clk_custom_name -source clk1 -multiply_by 6 {[[gen_clk3_port_ptr]]}
create_generated_clock -name gen_clk_custom_name -source clk1 -multiply_by 6 gen_clk3

# CHECK: create_generated_clock -name gen_clk4 -source clk1 -multiply_by 8 -add {[[gen_clk2_port_ptr]]}
create_generated_clock -name gen_clk4 -source clk1 -multiply_by 8 gen_clk2 -add
