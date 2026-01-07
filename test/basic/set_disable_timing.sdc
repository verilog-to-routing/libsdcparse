# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: [[FFQ0_ptr:__vtr_obj_pin_[0-9]+]]
puts [_libsdcparse_create_pin "FF.Q0" -direction OUTPUT]
# CHECK: [[FFDIN0_ptr:__vtr_obj_pin_[0-9]+]]
puts [_libsdcparse_create_pin "to_FFD.in0" -direction OUTPUT]

# CHECK: set_disable_timing -from {[[FFQ0_ptr]]} -to {[[FFDIN0_ptr]]}
set_disable_timing -from [get_pins {FF.Q0}] -to [get_pins {to_FFD.in0}]
