# RUN: %sdcparse-test %s 2>&1 | filecheck %s

_libsdcparse_create_port "in1" -type INPUT
_libsdcparse_create_port "in2" -type INPUT
_libsdcparse_create_port "out1" -type OUTPUT
_libsdcparse_create_port "out2" -type OUTPUT
_libsdcparse_create_port "clk" -type INPUT
_libsdcparse_create_port "clk2" -type INPUT

# CHECK: create_clock -period {{3.0*}} -waveform {{{1.250*}} {{2.750*}}} {clk}
create_clock -period 3 -waveform {1.25 2.75} clk

# CHECK: create_clock -period {{2.0*}} -waveform {{{0.0*}} {{1.0*}}} {clk2}
create_clock -period 2 clk2

# CHECK: create_clock -period {{2.50*}} -waveform {{{0.0*}} {{1.250*}}} -name virtual_io_clock
create_clock -period 2.5 -name virtual_io_clock

# TODO: This is not technically correct. We need to produce warnings and only
#       set the input delays on the input ports.
# CHECK: set_input_delay -clock virtual_io_clock -max {{1.0*}} {in1 in2 out1 out2 clk clk2}
set_input_delay -clock virtual_io_clock -max 1 [get_ports {*}]

# CHECK: set_output_delay -clock virtual_io_clock -max {{0.50*}} {in1 in2 out1 out2 clk clk2}
set_output_delay -clock virtual_io_clock -max 0.5 [get_ports {*}]
