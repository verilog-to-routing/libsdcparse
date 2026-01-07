# RUN: %sdcparse-test %s 2>&1

# COM: TODO: Add the filecheck.

#VPR compatible SDC file for benchmark circuit 'radar20'

# Creates an external virtual clock 'virtual_io_clock', and non-virtual clocks for each netlist clock (each with 1ns target clock period).
# Paths between netlist clock domains are not analyzed, but paths to/from the 'virtual_io_clock' and netlist clocks are analyzed.

puts [_libsdcparse_create_port "n101" -direction INPUT]
puts [_libsdcparse_create_port "clock" -direction INPUT]
puts [_libsdcparse_create_port "clk[0]" -direction INPUT]
puts [_libsdcparse_create_port "obj_q:obj_q\|altclklock0_wrap:b2v_inst10\|altclklock0:component1\|altpll:altpll_component\|altpll_bnf2:auto_generated\|clk\[0\]" -direction INPUT]

# COM: TODO: We should use the actual radar20 port names!
puts [_libsdcparse_create_port "in1" -direction INPUT]
puts [_libsdcparse_create_port "in2" -direction INPUT]
puts [_libsdcparse_create_port "in3" -direction INPUT]
puts [_libsdcparse_create_port "out1" -direction OUTPUT]
puts [_libsdcparse_create_port "out2" -direction OUTPUT]
puts [_libsdcparse_create_port "out3" -direction OUTPUT]

#**************************************************************
# Unit Information
#**************************************************************
#VPR assumes time unit is nanoseconds

#**************************************************************
# Create Clock
#**************************************************************
create_clock -period 1.000 -name virtual_io_clock
create_clock -period 1.000 {n101}
create_clock -period 1.000 {clock}
create_clock -period 1.000 {clk[0]}
create_clock -period 1.000 {clk\[0\]}
create_clock -period 1.000 {obj_q:obj_q\|altclklock0_wrap:b2v_inst10\|altclklock0:component1\|altpll:altpll_component\|altpll_bnf2:auto_generated\|clk\[0\]}
