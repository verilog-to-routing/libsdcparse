# RUN: %sdcparse-test %s 2>&1

# COM: TODO: Add the filecheck.

# COM: TODO: We can make this more interesting.
puts [_libsdcparse_create_port "in1" -type INPUT]
puts [_libsdcparse_create_port "in2" -type INPUT]
puts [_libsdcparse_create_port "in3" -type INPUT]
puts [_libsdcparse_create_port "out1" -type OUTPUT]
puts [_libsdcparse_create_port "out2" -type OUTPUT]
puts [_libsdcparse_create_port "out3" -type OUTPUT]

create_clock -period 0 *
