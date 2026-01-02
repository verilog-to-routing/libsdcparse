# RUN: %not %sdcparse-test %s -legacy 2>&1

#False Path
set_false_path -from [get_clocks{clk}] -max
