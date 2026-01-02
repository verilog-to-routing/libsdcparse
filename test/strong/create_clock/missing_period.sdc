# RUN: %not %sdcparse-test %s 2>&1 | filecheck %s

_libsdcparse_create_port "clk" -direction INPUT

# CHECK: create_clock: The argument '-period' is required.
create_clock clk
