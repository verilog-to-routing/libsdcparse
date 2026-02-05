# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: [[LSB_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port {Lsquare[bracket} -direction INPUT]
# CHECK: [[RSB_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port {Rsquare]bracket} -direction INPUT]
# CHECK: [[SS_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port {slash/symbol} -direction INPUT]
# CHECK: [[PS_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port {period.symbol} -direction INPUT]
# CHECK: [[MM_ptr:__vtr_obj_port_[0-9]+]]
puts [libsdcparse_create_port {money$money} -direction INPUT]

# CHECK-DAG: Lsquare[bracket
# CHECK-DAG: Rsquare]bracket
# CHECK-DAG: slash/symbol
# CHECK-DAG: period.symbol
# CHECK-DAG: money$money
foreach port [get_ports *] {
    puts [get_name $port]
}

# CHECK: [[LSB_ptr]]
puts [get_ports Lsquare\[bracket]
# CHECK: [[RSB_ptr]]
puts [get_ports Rsquare\]bracket]
# CHECK: [[SS_ptr]]
puts [get_ports slash/symbol]
# CHECK: [[PS_ptr]]
puts [get_ports period.symbol]
# CHECK: [[MM_ptr]]
puts [get_ports money\$money]

# CHECK: [[LSB_ptr]]
puts [get_ports {Lsquare[bracket}]
# CHECK: [[RSB_ptr]]
puts [get_ports {Rsquare]bracket}]
# CHECK: [[SS_ptr]]
puts [get_ports {slash/symbol}]
# CHECK: [[PS_ptr]]
puts [get_ports {period.symbol}]
# CHECK: [[MM_ptr]]
puts [get_ports {money$money}]

# CHECK: create_clock -period {{1.0*}} -waveform {{{0.0*}} {{0.50*}}} {[[LSB_ptr]]}
create_clock -period 1.0 Lsquare\[bracket
# CHECK: create_clock -period {{1.0*}} -waveform {{{0.0*}} {{0.50*}}} {[[RSB_ptr]]}
create_clock -period 1.0 Rsquare\]bracket
# CHECK: create_clock -period {{1.0*}} -waveform {{{0.0*}} {{0.50*}}} {[[SS_ptr]]}
create_clock -period 1.0 slash/symbol
# CHECK: create_clock -period {{1.0*}} -waveform {{{0.0*}} {{0.50*}}} {[[PS_ptr]]}
create_clock -period 1.0 period.symbol
# CHECK: create_clock -period {{1.0*}} -waveform {{{0.0*}} {{0.50*}}} {[[MM_ptr]]}
create_clock -period 1.0 money\$money

# CHECK: create_clock -period {{1.0*}} -waveform {{{0.0*}} {{0.50*}}} {[[LSB_ptr]]}
create_clock -period 1.0 {Lsquare\[bracket}
# CHECK: create_clock -period {{1.0*}} -waveform {{{0.0*}} {{0.50*}}} {[[RSB_ptr]]}
create_clock -period 1.0 {Rsquare\]bracket}
# CHECK: create_clock -period {{1.0*}} -waveform {{{0.0*}} {{0.50*}}} {[[SS_ptr]]}
create_clock -period 1.0 {slash/symbol}
# CHECK: create_clock -period {{1.0*}} -waveform {{{0.0*}} {{0.50*}}} {[[PS_ptr]]}
create_clock -period 1.0 {period.symbol}
# CHECK: create_clock -period {{1.0*}} -waveform {{{0.0*}} {{0.50*}}} {[[MM_ptr]]}
create_clock -period 1.0 {money\$money}
