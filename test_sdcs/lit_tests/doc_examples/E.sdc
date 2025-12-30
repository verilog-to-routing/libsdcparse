# RUN: %sdcparse-test %s 2>&1 | filecheck %s

_libsdcparse_create_port "in1" -type INPUT
_libsdcparse_create_port "in2" -type INPUT
_libsdcparse_create_port "in3" -type INPUT
_libsdcparse_create_port "out1" -type OUTPUT
_libsdcparse_create_port "out2" -type OUTPUT
_libsdcparse_create_port "clk" -type INPUT
_libsdcparse_create_port "clk2" -type INPUT

# CHECK: create_clock -period {{3.0*}} -waveform {{{1.250*}} {{2.750*}}} {clk}
create_clock -period 3 -waveform {1.25 2.75} clk

# CHECK: create_clock -period {{2.0*}} -waveform {{{0.0*}} {{1.0*}}} {clk2}
create_clock -period 2 clk2

# CHECK: create_clock -period {{1.0*}} -waveform {{{0.0*}} {{0.50*}}} -name input_clk
create_clock -period 1 -name input_clk

# CHECK: create_clock -period {{0.0*}} -waveform {{{0.0*}} {{0.0*}}} -name output_clk
create_clock -period 0 -name output_clk

# TODO: We should really make these functions return pointers. How do we know that
#       the clk2 that is returned is the port or the clock?
# CHECK: set_clock_groups -group {input_clk} -group {clk2}
set_clock_groups -exclusive -group input_clk -group clk2

# CHECK: set_false_path -from {clk} -to {output_clk}
set_false_path -from [get_clocks {clk}] -to [get_clocks {output_clk}]

# CHECK: set_max_delay {{17.0*}} -from {input_clk} -to {output_clk}
set_max_delay 17 -from [get_clocks {input_clk}] -to [get_clocks {output_clk}]

# CHECK: set_multicycle_path 3 -setup -from {clk} -to {clk2}
set_multicycle_path -setup -from [get_clocks {clk}] -to [get_clocks {clk2}] 3

# TODO: The order here is not guarenteed.
# CHECK: set_input_delay -clock input_clk -max {{0.50*}} {in1 in2 in3}
set_input_delay -clock input_clk -max 0.5 [get_ports {in1 in2 in3}]

# TODO: The order here is not guarenteed.
# CHECK: set_output_delay -clock output_clk -max {{1.0*}} {out1 out2}
set_output_delay -clock output_clk -max 1 [get_ports {out*}]
