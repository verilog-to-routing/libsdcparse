# RUN: %sdcparse-test %s 2>&1 | filecheck %s
# TODO: Need to figure out how to get this testcase properly tested.
# XFAIL: *

_libsdcparse_create_port "clk" -type INPUT
create_clock -period 1.0 clk

# CHECK: Message: set_false_path: Unknown flag '-max'
set_false_path -from [get_clocks {clk}] -max
