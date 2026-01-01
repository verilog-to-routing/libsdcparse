# RUN: %sdcparse-test %s -legacy 2>&1
# TODO: Need to figure out how to get this testcase properly tested.
# XFAIL: *

#False Path
set_false_path -from [get_clocks{clk}] -max
