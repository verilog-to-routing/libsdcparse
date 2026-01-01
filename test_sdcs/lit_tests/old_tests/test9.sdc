# RUN: %sdcparse-test %s 2>&1

# COM: TODO: Add the filecheck.

puts [_libsdcparse_create_cell "werty/asdr/q"]

set_timing_derate -early 0.9
set_timing_derate -late 1.1

set_timing_derate -early 0.9 -cell_delay
set_timing_derate -late 1.1 -cell_delay

set_timing_derate -early 0.9 -net_delay
set_timing_derate -late 1.1 -net_delay

set_timing_derate -late 1.1 -cell_delay [get_cells werty/asdr/q ]
