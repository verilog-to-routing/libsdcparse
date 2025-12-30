# RUN: %sdcparse-test %s 2>&1 | filecheck %s

_libsdcparse_create_port "clk1" -type INPUT

create_clock -period 1 clk1

# CHECK: set_clock_latency -source {{1.0*}} {clk1}
set_clock_latency -source 1.0 [get_clocks clk1]
