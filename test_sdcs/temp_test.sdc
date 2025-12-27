
create_clock -period 1.0 clk3

create_clock -period 6.0 clk4 -add

create_clock -period 7.0 -add clk5

create_clock -period 2.0 {clk3 clk4} -name Test

create_clock -period 5 -name Jeff

create_clock -period 3 -waveform {1 2} clk1

set_input_delay -clock Jeff 0 {in1 in2 in3}

set_input_delay -clock Jeff 0 [get_ports {in1}]
set_input_delay -clock Jeff 0 [get_ports {in1 in2}]
set_input_delay -clock Jeff 0 [get_ports {in*}]
set_input_delay -clock Jeff 0 [get_ports -regexp {in.*}]
set_input_delay -clock Jeff 0 [get_ports -nocase {IN1}]
set_input_delay -clock Jeff 0 [get_ports -regexp -nocase {IN.*}]
set_input_delay -clock Jerry 0 [get_ports *]
set temp [get_ports out1]
set temp2 [get_ports -quiet out2]

set_clock_groups -exclusive -group {clk3 clk4} -group {clk1 clk5}
set_clock_groups -exclusive -group [get_clocks Jeff] -group [get_clocks Test] -group clk1

set_false_path -from [get_clocks clk1] -to [get_clocks clk3]

set_max_delay 17 -from [get_clocks clk1] -to [get_clocks clk3]
set_min_delay 2 -from [get_clocks clk1] -to [get_clocks clk3]

set_multicycle_path -from [get_clocks clk1] -to [get_clocks clk3] 4
set_multicycle_path -setup -from [get_clocks {clk1}] -to [get_clocks {clk3}] 3
set_multicycle_path -hold -from [get_clocks {clk1}] -to [get_clocks {clk3}] 2

set_clock_uncertainty 0.025
set_clock_uncertainty -from [get_clocks {clk1}] 0.05
set_clock_uncertainty -from [get_clocks {clk1}]  -to [get_clocks {clk3}] 0.75

set_clock_latency -source 1.0 [get_clocks {clk1}]

set_disable_timing -from [get_pins {FFA.Q0}] -to [get_pins {to_FFD.in0}]
