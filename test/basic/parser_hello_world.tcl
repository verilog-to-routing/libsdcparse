# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: Hello World!
puts "Hello World!"