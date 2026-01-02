
rename unknown _original_unknown

# Check if the given string looks like a bus, i.e. [0], [*], [15:0], etc.
proc _is_bus_str { str } {
    if {[string is integer $str]} { return 1 }
    if {[string equal $str "*"]} { return 1 }
    if {[regexp {[0-9]+:[0-9]+} $str]} { return 1 }
    return 0
}

# This is a massive hack that we need to do to make parsing busses actually work.
# The issue is that some people like to write get_ports A[*], but this causes
# problems because TCL tries to interpret the square brackets as a method. Here,
# we override TCL's behaviour such that when it tries to execute the [*] command,
# and fails, we check if it looks like a bus and instead return the string literal.
# This appears to be the common way of dealing with this.
proc unknown {args} {
    set cmd [lindex $args 0]
    if { [llength $args] == 1 && [_is_bus_str $cmd] } {
        return "\[$cmd\]"
    }

    uplevel 1 _original_unknown $args
}

# =====================================================================
# Generic SDC Argument Parser
# =====================================================================
# This procedure handles the heavy lifting for parsing Tcl arguments.
#
# Parameters:
#   cmd_name  - Name of the command (for error reporting)
#   spec      - A dictionary defining the argument schema:
#               flags:   Flags that require a value (e.g., -period)
#               bools:   Boolean flags (e.g., -add)
#               pos:     List of names for positional arguments
#               require: List of flags/positional names that are mandatory
#               types:   (Optional) Dictionary mapping arg names to types
#                        Supported types: double, integer, boolean
#   raw_args  - The actual 'args' list passed to the command
# =====================================================================
proc generic_sdc_parser {cmd_name spec raw_args} {
    # Extract schema details
    set val_flags [dict get $spec flags]
    set bool_flags [dict get $spec bools]
    set pos_names [dict get $spec pos]
    set required [dict get $spec require]
    
    set arg_types {}
    if {[dict exists $spec types]} {
        set arg_types [dict get $spec types]
    }

    set list_flags {}
    if {[dict exists $spec list_flags]} {
        set list_flags [dict get $spec list_flags]
    }

    # Result dictionary initialization
    set results [dict create]
    foreach b $bool_flags { dict set results $b 0 }
    foreach f $val_flags  { dict set results $f "" }
    foreach p $pos_names  { dict set results $p "" }
    foreach l $list_flags { dict set results $l {} }

    set remaining $raw_args
    set positional_values {}

    # --- 1. Parse Flags ---
    while {[llength $remaining] > 0} {
        set arg [lindex $remaining 0]
        
        if {[string index $arg 0] ne "-"} {
            # Not a flag, move to positional processing
            lappend positional_values $arg
            set remaining [lrange $remaining 1 end]
            continue
        }

        if {$arg in $val_flags} {
            if {[llength $remaining] < 2} {
                error "$cmd_name: Missing value for flag '$arg'"
            }
            dict set results $arg [lindex $remaining 1]
            set remaining [lrange $remaining 2 end]
        }  elseif {$arg in $list_flags} {
            if {[llength $remaining] < 2} {
                error "$cmd_name: Missing value for list flag '$arg'"
            }
            set curr_list [dict get $results $arg]
            lappend curr_list [lindex $remaining 1]
            dict set results $arg $curr_list
            set remaining [lrange $remaining 2 end]
        } elseif {$arg in $bool_flags} {
            dict set results $arg 1
            set remaining [lrange $remaining 1 end]
        } else {
            error "$cmd_name: Unknown flag '$arg'"
        }
    }

    # --- 2. Parse Positional Arguments ---
    set i 0
    foreach name $pos_names {
        if {$i < [llength $positional_values]} {
            dict set results $name [lindex $positional_values $i]
        }
        incr i
    }

    # --- 3. Validation ---
    
    # Check Required arguments
    foreach req $required {
        if {[dict get $results $req] eq ""} {
            error "$cmd_name: The argument '$req' is required."
        }
        if {$req in $list_flags} {
            if {[dict get $results $req] eq {}} {
                error "$cmd_name: The argument '$req' is required."
            }
        }
    }

    # Type Checking
    dict for {arg_name type} $arg_types {
        if {[dict exists $results $arg_name]} {
            set val [dict get $results $arg_name]
            
            # Skip check if the value is empty and not required (already handled above)
            if {$val eq ""} continue

            switch -- $type {
                "double" {
                    if {![string is double -strict $val]} {
                        error "$cmd_name: Argument '$arg_name' must be a double (received '$val')."
                    }
                }
                "integer" {
                    if {![string is integer -strict $val]} {
                        error "$cmd_name: Argument '$arg_name' must be an integer (received '$val')."
                    }
                }
                "boolean" {
                    if {![string is boolean -strict $val]} {
                        error "$cmd_name: Argument '$arg_name' must be a boolean (received '$val')."
                    }
                }
            }
        }
    }

    return $results
}

proc _convert_to_objects {cmd_name targets object_type_list} {
    set id_targets {}
    foreach item $targets {
        if {[is_object_id_internal $item]} {
            # Already is an object.
            lappend id_targets $item
        } else {
            # Convert to object(s).
            # NOTE: Here we are trying to find all matches in the object type list.
            # TODO: Need to verify if this is the correct functionality.
            set params [dict create]
            dict set params -quiet 1
            dict set params -nocase 0
            dict set params -regexp 0
            dict set params patterns $item
            foreach object_type $object_type_list {
                switch -- $object_type {
                    "ports" {
                        set matches [_query_get_impl $cmd_name all_ports_internal $params]
                        lappend id_targets {*}$matches
                    }
                    "clocks" {
                        set matches [_query_get_impl $cmd_name all_clocks_internal $params]
                        lappend id_targets {*}$matches
                    }
                    "pins" {
                        set matches [_query_get_impl $cmd_name all_pins_internal $params]
                        lappend id_targets {*}$matches
                    }
                    "cells" {
                        set matches [_query_get_impl $cmd_name all_cells_internal $params]
                        lappend id_targets {*}$matches
                    }
                }
            }
        }
    }

    # TODO: Should the error be raised here?
    if {[llength $targets] != 0 && [llength $id_targets] == 0} {
        error "$cmd_name: Unknown targets: $targets."
    }

    return $id_targets
}

proc _libsdcparse_set_lineno {} {
    # Set the line number from the caller's frame
    # TODO: Can this be made more general?
    set frame_info [info frame -2]
    set line_num [dict get $frame_info line]
    lineno_internal $line_num
}

proc create_clock {args} {
    _libsdcparse_set_lineno

    # TODO: Either target or name is required. Need to improve the parser to handle
    #       this.
    set spec {
        flags   {-period -waveform -name}
        bools   {-add}
        pos     {targets}
        require {-period}
        types   {-period double}
    }

    set params [generic_sdc_parser "create_clock" $spec $args]

    set period [dict get $params -period]

    set name [dict get $params -name]

    # Get the waveform. If the waveform is unset, it is defaulted to a 50% duty-cycle.
    set waveform [dict get $params -waveform]
    if {$waveform == {}} {
        set waveform [list "0" [expr { $period / double(2) }]]
    } elseif {[llength $waveform] != 2} {
        # TODO: This check should be in the parser for better generality.
        #       This check should include that this is a list of doubles.
        error "create_clock: Waveform must have rise_time and fall_time."
    }

    set id_targets [_convert_to_objects "create_clock" [dict get $params targets] {ports}]

    # TODO: This should be added to the parser for better generality.
    if {$name == "" && [llength $id_targets] == 0} {
        error "create_clock: Either name or target must be provided."
    }

    # TODO: Can we improve this function interface to wrap onto multiple lines
    #       without messing with the strings?
    create_clock_internal $period $name $waveform [dict get $params -add] $id_targets
}

proc set_clock_groups {args} {
    _libsdcparse_set_lineno

    # TODO: Need to ensure the number of groups is more than 1 I think.
    set spec {
        flags        {}
        list_flags   {-group}
        bools        {-exclusive}
        pos          {}
        require      {-exclusive -group}
        types        {}
    }

    set params [generic_sdc_parser "set_clock_groups" $spec $args]

    # TODO: There has to be a cleaner way of doing this. Vector of vectors are
    #       very tricky with SWIG. Decided to linearize the container instead.
    set clock_list {}
    set clock_group_start_pos { 0 }
    set i 0
    foreach group [dict get $params -group] {
        foreach clk [_convert_to_objects "set_clock_groups" $group {clocks}] {
            lappend clock_list $clk
            incr i
        }
        lappend clock_group_start_pos $i
    }

    set_clock_groups_internal $clock_list $clock_group_start_pos
}

proc set_false_path {args} {
    _libsdcparse_set_lineno

    set spec {
        flags   {-from -to}
        bools   {}
        pos     {}
        require {}
        types   {}
    }

    set params [generic_sdc_parser "set_false_path" $spec $args]

    set from_list [_convert_to_objects "set_false_path" [dict get $params -from] {clocks cells ports pins}]
    set to_list [_convert_to_objects "set_false_path" [dict get $params -to] {clocks cells ports pins}]

    set_false_path_internal $from_list $to_list
}

proc set_max_delay {args} {
    _libsdcparse_set_lineno

    set spec {
        flags   {-from -to}
        bools   {}
        pos     {delay}
        require {delay}
        types   {delay double}
    }

    set params [generic_sdc_parser "set_max_delay" $spec $args]

    set from_list [_convert_to_objects "set_max_delay" [dict get $params -from] {clocks cells ports pins}]
    set to_list [_convert_to_objects "set_max_delay" [dict get $params -to] {clocks cells ports pins}]

    set_max_delay_internal [dict get $params delay] $from_list $to_list
}

proc set_min_delay {args} {
    _libsdcparse_set_lineno

    set spec {
        flags   {-from -to}
        bools   {}
        pos     {delay}
        require {delay}
        types   {delay double}
    }

    set params [generic_sdc_parser "set_min_delay" $spec $args]

    set from_list [_convert_to_objects "set_min_delay" [dict get $params -from] {clocks cells ports pins}]
    set to_list [_convert_to_objects "set_min_delay" [dict get $params -to] {clocks cells ports pins}]

    set_min_delay_internal [dict get $params delay] $from_list $to_list
}

proc set_multicycle_path {args} {
    _libsdcparse_set_lineno

    set spec {
        flags   {-from -to}
        bools   {-setup -hold}
        pos     {path_multiplier}
        require {path_multiplier}
        types   {path_multiplier integer}
    }

    set params [generic_sdc_parser "set_multicycle_path" $spec $args]

    set from_list [_convert_to_objects "set_multicycle_path" [dict get $params -from] {clocks cells ports pins}]
    set to_list [_convert_to_objects "set_multicycle_path" [dict get $params -to] {clocks cells ports pins}]

    set_multicycle_path_internal [dict get $params -setup] [dict get $params -hold] $from_list $to_list [dict get $params path_multiplier]
}

proc set_input_delay {args} {
    _libsdcparse_set_lineno

    set spec {
        flags   {-clock}
        bools   {-max -min}
        pos     {delay targets}
        require {-clock targets}
        types   {delay double}
    }

    set params [generic_sdc_parser "set_input_delay" $spec $args]

    set id_targets [_convert_to_objects "set_input_delay" [dict get $params targets] {ports pins}]

    set_input_delay_internal [dict get $params -max] [dict get $params -min] [dict get $params -clock] [dict get $params delay] $id_targets
}

proc set_output_delay {args} {
    _libsdcparse_set_lineno

    set spec {
        flags   {-clock}
        bools   {-max -min}
        pos     {delay targets}
        require {-clock targets}
        types   {delay double}
    }

    set params [generic_sdc_parser "set_output_delay" $spec $args]

    set id_targets [_convert_to_objects "set_input_delay" [dict get $params targets] {ports pins}]

    set_output_delay_internal [dict get $params -max] [dict get $params -min] [dict get $params -clock] [dict get $params delay] $id_targets
}

proc set_clock_uncertainty {args} {
    _libsdcparse_set_lineno

    set spec {
        flags   {-from -to}
        bools   {-setup -hold}
        pos     {uncertainty}
        require {uncertainty}
        types   {uncertainty double}
    }

    set params [generic_sdc_parser "set_clock_uncertainty" $spec $args]

    set from_list [_convert_to_objects "set_clock_uncertainty" [dict get $params -from] {clocks}]
    set to_list [_convert_to_objects "set_clock_uncertainty" [dict get $params -to] {clocks}]

    set_clock_uncertainty_internal [dict get $params -setup] [dict get $params -hold] $from_list $to_list [dict get $params uncertainty]
}

proc set_clock_latency {args} {
    _libsdcparse_set_lineno

    set spec {
        flags   {}
        bools   {-source -early -late}
        pos     {latency targets}
        require {-source latency targets}
        types   {latency double}
    }

    set params [generic_sdc_parser "set_clock_latency" $spec $args]

    set target_list [_convert_to_objects "set_clock_latency" [dict get $params targets] {clocks}]

    set_clock_latency_internal [dict get $params -source] [dict get $params -early] [dict get $params -late] [dict get $params latency] $target_list
}

proc set_disable_timing {args} {
    _libsdcparse_set_lineno

    set spec {
        flags   {-from -to}
        bools   {}
        pos     {}
        require {}
        types   {}
    }

    set params [generic_sdc_parser "set_disable_timing" $spec $args]

    set from_list [_convert_to_objects "set_disable_timing" [dict get $params -from] {pins}]
    set to_list [_convert_to_objects "set_disable_timing" [dict get $params -to] {pins}]

    set_disable_timing_internal $from_list $to_list
}

proc set_timing_derate {args} {
    _libsdcparse_set_lineno

    set spec {
        flags   {}
        bools   {-early -late -net_delay -cell_delay}
        pos     {derate targets}
        require {derate}
        types   {derate double}
    }

    set params [generic_sdc_parser "set_timing_derate" $spec $args]

    set targets [_convert_to_objects "set_timing_derate" [dict get $params targets] {cells}]

    set_timing_derate_internal [dict get $params -early] [dict get $params -late] [dict get $params -net_delay] [dict get $params -cell_delay] [dict get $params derate] $targets
}

proc get_name {args} {
    _libsdcparse_set_lineno

    set spec {
        flags   {}
        bools   {}
        pos     {object}
        require {object}
        types   {}
    }

    set params [generic_sdc_parser "get_name" $spec $args]

    set object [dict get $params object]

    if {![is_object_id_internal $object]} {
        error "get_name: Not an object: '$object'"
    }

    # TODO: This should really become [get_property $object name]
    return [get_name_internal $object]
}

proc _query_get_impl {cmd_name all_func params} {
    # Create the options for the search.
    set search_options {}
    if {[dict get $params -nocase]} {
        lappend search_options -nocase
    }

    set matches [lmap id [$all_func] {
        set name [get_name_internal $id]

        set match 0
        foreach pattern [dict get $params patterns] {
            # Since square brackets are used for ports, and are not used for
            # any other pattern as far as I am aware, we can just always assume
            # that the square brackets should be escaped. Any square brackets
            # that are not escaped, make them escaped.
            # TODO: Is there a cleaner way to do this?
            # TODO: Ports should just be handled special. We could match for the
            #       port pattern and make special patterns which get the correct
            #       ports.
            set tmp [string map {\\\[ "___ESC_BRACKET___"} $pattern]
            set tmp [string map {[ "\\\["} $tmp]
            set escaped_pattern [string map {"___ESC_BRACKET___" \\\[} $tmp]

            if {[dict get $params -regexp]} {
                if {[regexp {*}$search_options -- $escaped_pattern $name]} {
                    set match 1
                    break;
                }
            } else {
                if {[string match {*}$search_options $escaped_pattern $name]} {
                    set match 1
                    break;
                }
            }
        }
        if {$match} {
            set id
        } else {
            continue
        }
    }]

    # If unique matches is empty, raise error unless quiet is active.
    if {[llength $matches] == 0 && ![dict get $params -quiet]} {
        puts "Warning: no matches found for $cmd_name [dict get $params patterns]"
    }

    return $matches
}

proc get_ports {args} {
    _libsdcparse_set_lineno

    set spec {
        flags   {}
        bools   {-regexp -nocase -quiet}
        pos     {patterns}
        require {patterns}
        types   {}
    }

    set params [generic_sdc_parser "get_ports" $spec $args]

    set matches [_query_get_impl "get_ports" all_ports_internal $params]

    return $matches
}

proc get_clocks {args} {
    _libsdcparse_set_lineno

    set spec {
        flags   {}
        bools   {-regexp -nocase -quiet}
        pos     {patterns}
        require {patterns}
        types   {}
    }

    set params [generic_sdc_parser "get_clocks" $spec $args]

    set matches [_query_get_impl "get_clocks" all_clocks_internal $params]

    return $matches
}

proc get_pins {args} {
    _libsdcparse_set_lineno

    set spec {
        flags   {}
        bools   {-regexp -nocase -quiet}
        pos     {patterns}
        require {patterns}
        types   {}
    }

    set params [generic_sdc_parser "get_pins" $spec $args]

    set matches [_query_get_impl "get_pins" all_pins_internal $params]

    return $matches
}

proc get_cells {args} {
    _libsdcparse_set_lineno

    set spec {
        flags   {}
        bools   {-regexp -nocase -quiet}
        pos     {patterns}
        require {patterns}
        types   {}
    }

    set params [generic_sdc_parser "get_cells" $spec $args]

    set matches [_query_get_impl "get_cells" all_cells_internal $params]

    return $matches
}

proc all_inputs {} {
    _libsdcparse_set_lineno

    # TODO: Handle -no_clocks bool

    return [all_inputs_internal]
}

proc all_outputs {} {
    _libsdcparse_set_lineno

    return [all_outputs_internal]
}

proc all_clocks {} {
    _libsdcparse_set_lineno

    return [all_clocks_internal]
}

proc _libsdcparse_create_port {args} {
    _libsdcparse_set_lineno

    set spec {
        flags   {-type}
        bools   {}
        pos     {port_name}
        require {-type}
        types   {}
    }

    set params [generic_sdc_parser "create_port" $spec $args]

    return [_libsdcparse_create_port_internal [dict get $params port_name] [dict get $params -type]]
}

proc _libsdcparse_create_pin {args} {
    _libsdcparse_set_lineno

    set spec {
        flags   {-type}
        bools   {}
        pos     {pin_name}
        require {pin_name -type}
        types   {}
    }

    set params [generic_sdc_parser "create_pin" $spec $args]

    # NOTE: Right now we ignore the pin type. Eventually we will need the port
    #       types.
    return [_libsdcparse_create_pin_internal [dict get $params pin_name]]
}

proc _libsdcparse_create_cell {args} {
    _libsdcparse_set_lineno

    set spec {
        flags   {}
        bools   {}
        pos     {cell_name}
        require {cell_name}
        types   {}
    }

    set params [generic_sdc_parser "create_cell" $spec $args]

    return [_libsdcparse_create_cell_internal [dict get $params cell_name]]
}
