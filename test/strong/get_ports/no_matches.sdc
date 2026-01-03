# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: Warning: no matches found for get_ports missing
get_ports {missing}
