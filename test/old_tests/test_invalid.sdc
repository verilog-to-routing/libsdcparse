# RUN: %not %sdcparse-test %s 2>&1 | filecheck %s

_libsdcparse_create_port "clk" -type INPUT
create_clock -period 1.0 clk

# CHECK: set_false_path: Unknown flag '-max'
set_false_path -from [get_clocks {clk}] -max
