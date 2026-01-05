# RUN: %not %sdcparse-test %s 2>&1 | filecheck %s

_libsdcparse_create_port "clk" -direction INPUT
_libsdcparse_create_port "gen_clk" -direction INPUT

create_clock -period 1 clk

# CHECK: Custom Error at line [[# @LINE + 1]]: create_generated_clock: The argument '-source' is required.
create_generated_clock -divide_by 4 gen_clk
