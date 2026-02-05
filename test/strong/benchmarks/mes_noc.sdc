# RUN: %sdcparse-test %s 2>&1 | filecheck %s

# CHECK: [[net1_ptr:__vtr_obj_net_[0-9]+]]
puts [libsdcparse_create_net {pll_noc_type2:\using_pll:use_noc_pll_2|altpll:altpll_component|pll_noc_type2_altpll:auto_generated|wire_pll1_clk[0]}]

# CHECK: pll_noc_type2:\using_pll:use_noc_pll_2|altpll:altpll_component|pll_noc_type2_altpll:auto_generated|wire_pll1_clk[0]
puts [get_name [get_nets *]]

# CHECK: [[net1_ptr]]
puts [get_nets {pll_noc_type2:\\using_pll:use_noc_pll_2\|altpll:altpll_component\|pll_noc_type2_altpll:auto_generated\|wire_pll1_clk\[0\]}]

# CHECK: create_clock -period {{1.0*}} -waveform {{{0.0*}} {{0.50*}}} {[[net1_ptr]]}
create_clock -period 1.000 {pll_noc_type2:\\using_pll:use_noc_pll_2\|altpll:altpll_component\|pll_noc_type2_altpll:auto_generated\|wire_pll1_clk\[0\]}
