# RUN: %not %sdcparse-test %s 2>&1 | filecheck %s

libsdcparse_create_port "clk" -direction INPUT
libsdcparse_create_port "gen_clk" -direction INPUT

create_clock -period 1 clk

# CHECK: Custom Error at line [[# @LINE + 1]]: create_generated_clock: -edge_shift expected to be a list of doubles, given a non-double shift: abc
create_generated_clock -source clk -edges {1 3 5} -edge_shift {1.0 abc -0.5} gen_clk
