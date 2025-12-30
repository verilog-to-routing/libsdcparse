# RUN: %sdcparse-test %s 2>&1 | filecheck %s

_libsdcparse_create_port "in1" -type INPUT
_libsdcparse_create_port "in2" -type INPUT
_libsdcparse_create_port "out1" -type OUTPUT
_libsdcparse_create_port "out2" -type OUTPUT
_libsdcparse_create_port "clk" -type INPUT
_libsdcparse_create_port "clk2" -type INPUT

# CHECK: create_clock -period {{2.0*}} -waveform {{{0.0*}} {{1.0*}}} {clk}
create_clock -period 2 clk

# CHECK: create_clock -period {{3.0*}} -waveform {{{0.0*}} {{1.50*}}} {clk2}
create_clock -period 3 clk2

# CHECK: create_clock -period {{3.50*}} -waveform {{{0.0*}} {{1.750*}}} -name virtual_io_clock
create_clock -period 3.5 -name virtual_io_clock

# CHECK: set_clock_groups -group {clk} -group {clk2}
set_clock_groups -exclusive -group {clk} -group {clk2}

# TODO: This is not technically correct. We need to produce warnings and only
#       set the input delays on the input ports.
# CHECK: set_input_delay -clock virtual_io_clock -max {{0.0*}} {in1 in2 out1 out2 clk clk2}
set_input_delay -clock virtual_io_clock -max 0 [get_ports {*}]

# CHECK: set_output_delay -clock virtual_io_clock -max {{0.0*}} {in1 in2 out1 out2 clk clk2}
set_output_delay -clock virtual_io_clock -max 0 [get_ports {*}]
