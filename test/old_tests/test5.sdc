# RUN: %sdcparse-test %s 2>&1

# COM: TODO: Add the filecheck.

# COM: TODO: We can make this more interesting.
puts [_libsdcparse_create_port "in1" -direction INPUT]
puts [_libsdcparse_create_port "in2" -direction INPUT]
puts [_libsdcparse_create_port "in3" -direction INPUT]
puts [_libsdcparse_create_port "out1" -direction OUTPUT]
puts [_libsdcparse_create_port "out2" -direction OUTPUT]
puts [_libsdcparse_create_port "out3" -direction OUTPUT]

create_clock -period 0 *
