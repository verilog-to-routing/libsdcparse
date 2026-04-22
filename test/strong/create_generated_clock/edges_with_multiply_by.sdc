# RUN: %not %sdcparse-test %s 2>&1 | filecheck %s

libsdcparse_create_port "clk" -direction INPUT
libsdcparse_create_port "gen_clk" -direction INPUT

create_clock -period 1 clk

# CHECK: Custom Error at line [[# @LINE + 1]]: create_generated_clock: -edges cannot be used with -divide_by or -multiply_by
create_generated_clock -source clk -edges {1 3 5} -multiply_by 2 gen_clk
