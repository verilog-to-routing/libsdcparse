# RUN: %not %sdcparse-test %s 2>&1 | filecheck %s

libsdcparse_create_cell "cell1"

# CHECK: Custom Error at line [[# @LINE + 1]]: create_clock: Target is not the correct type
create_clock -period 1 [get_cells cell1]
