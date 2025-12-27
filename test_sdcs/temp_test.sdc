
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
