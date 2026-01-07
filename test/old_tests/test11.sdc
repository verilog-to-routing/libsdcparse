# RUN: %sdcparse-test %s 2>&1

# COM: TODO: Add the filecheck.

puts [_libsdcparse_create_pin "asdf/qwer/cin" -direction INPUT]
puts [_libsdcparse_create_pin "awer/zxc/guy3w" -direction INPUT]

set_disable_timing -from {asdf/qwer/cin} -to {awer/zxc/guy3w}
set_disable_timing -from asdf/qwer/cin -to awer/zxc/guy3w
set_disable_timing -from [get_pins {asdf/qwer/cin}] -to awer/zxc/guy3w
