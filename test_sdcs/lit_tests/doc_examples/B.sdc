# RUN: %sdcparse-test %s 2>&1 | filecheck %s

_libsdcparse_create_port "clk" -type INPUT
_libsdcparse_create_port "clk2" -type INPUT

# CHECK: create_clock -period {{2.0*}} -waveform {{{0.0*}} {{1.0*}}} {clk}
create_clock -period 2 clk

# CHECK: create_clock -period {{3.0*}} -waveform {{{0.0*}} {{1.50*}}} {clk2}
create_clock -period 3 clk2

# CHECK: set_clock_groups -group {clk} -group {clk2}
set_clock_groups -exclusive -group {clk} -group {clk2}
