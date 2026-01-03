# RUN: %not %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: Custom Error at line [[# @LINE + 1]]: get_property: Unknown targets: does_not_exist.
puts [get_name "does_not_exist"]
