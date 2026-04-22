# RUN: %not %sdcparse-test %s 2>&1 | filecheck %s

libsdcparse_create_port "clk" -direction INPUT
libsdcparse_create_port "gen_clk" -direction INPUT

create_clock -period 1 clk

# CHECK: Custom Error at line [[# @LINE + 1]]: create_generated_clock: -duty_cycle expected to be a percentage from 0.0 to 100.0
create_generated_clock -source clk -multiply_by 2 -duty_cycle 150.0 gen_clk
