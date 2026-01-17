# RUN: %not %sdcparse-test %s 2>&1 | filecheck %s

libsdcparse_create_port "clk" -direction INPUT

# CHECK: create_clock: Argument '-period' must be a double (received 'period').
create_clock -period period clk
