# RUN: %sdcparse-test %s 2>&1 | filecheck %s

_libsdcparse_create_port "clk1" -type INPUT
_libsdcparse_create_port "in1" -type INPUT
_libsdcparse_create_port "in2" -type INPUT
_libsdcparse_create_port "in3" -type INPUT
_libsdcparse_create_port "in4" -type INPUT
_libsdcparse_create_port "A1" -type INPUT
_libsdcparse_create_port "A2" -type INPUT
_libsdcparse_create_port "A3" -type INPUT
_libsdcparse_create_port "A4" -type INPUT

# CHECK: create_clock -period {{1.0*}} -waveform {{{0.0*}} {{0.50*}}} {clk1}
create_clock -period 1.0 clk1

# CHECK: set_input_delay -clock clk1 {{0.0*}} {in1}
set_input_delay -clock clk1 0 in1

# CHECK: create_clock -period {{2.0*}} -waveform {{{0.0*}} {{1.0*}}} -name virtual_clock
create_clock -name virtual_clock -period 2.0

# CHECK: set_input_delay -clock virtual_clock {{1.0*}} {in2}
set_input_delay -clock virtual_clock 1.0 in2

# CHECK: set_input_delay -clock clk1 -max {{0.50*}} {in3}
set_input_delay -clock clk1 -max 0.5 in3

# CHECK: set_input_delay -clock clk1 -min {{1.0*}} {in4}
set_input_delay -clock clk1 -min 1 in4

# TODO: The order may not be guarenteed. Need to make this more stable.
# CHECK: set_input_delay -clock virtual_clock {{0.0*}} {A1 A2 A3 A4}
set_input_delay -clock virtual_clock 0.0 [get_ports A*]
