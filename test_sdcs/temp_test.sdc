
create_clock -period 1.0 clk3

create_clock -period 6.0 clk4

create_clock -period 2.0 {clk3 clk4}

create_clock -period 5 -name Jeff

create_clock -period 3 -waveform {1 2} clk1

set_input_delay -clock Jeff 0 {in1 in2 in3}
