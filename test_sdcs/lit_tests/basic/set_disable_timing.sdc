# RUN: %sdcparse-test %s 2>&1 | filecheck %s

_libsdcparse_create_pin "FF.Q0" -type OUTPUT
_libsdcparse_create_pin "to_FFD.in0" -type OUTPUT

# CHECK: set_disable_timing -from {FF.Q0} -to {to_FFD.in0}
set_disable_timing -from [get_pins {FF.Q0}] -to [get_pins {to_FFD.in0}]
