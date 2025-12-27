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

    # Result dictionary initialization
    set results [dict create]
    foreach b $bool_flags { dict set results $b 0 }
    foreach f $val_flags  { dict set results $f "" }
    foreach p $pos_names  { dict set results $p "" }

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

# TODO: Maybe add an argument with a list of object types to try in order
#           {ports pins ... }
proc _convert_to_objects {cmd_name targets} {
    set id_targets {}
    foreach item $targets {
        if {[is_object_id_internal $item]} {
            # Already is an object.
            lappend id_targets $item
        } else {
            # Convert to an object.
            # First, try to see if this is a port.
            set found 0
            foreach id [all_ports_internal] {
                set name [get_name_internal $id]
                if {[string match $item $name]} {
                    lappend id_targets $id
                    set found 1
                }
            }
            if {$found} {continue}
            # TODO: Try to see if this is a pin.

            # If nothing can be found, raise an error.
            error "$cmd_name: Unknown target: '$item'."
        }
    }

    return $id_targets
}

proc create_clock {args} {
    # Set the line number from the caller's frame
    # TODO: Can this be made more general?
    set frame_info [info frame -1]
    set line_num [dict get $frame_info line]
    lineno_internal $line_num

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

    # TODO: Verify in TCL land that waveform has length two and maybe that
    #       the first element is smaller than the second.

    set id_targets [_convert_to_objects "create_clock" [dict get $params targets]]

    # TODO: Can we improve this function interface to wrap onto multiple lines
    #       without messing with the strings?
    create_clock_internal [dict get $params -period] [dict get $params -name] [dict get $params -waveform] [dict get $params -add] $id_targets
}

proc set_input_delay {args} {
    # Set the line number from the caller's frame
    set frame_info [info frame -1]
    set line_num [dict get $frame_info line]
    lineno_internal $line_num

    set spec {
        flags   {-clock}
        bools   {-max -min}
        pos     {delay targets}
        require {-clock targets}
        types   {delay double}
    }

    set params [generic_sdc_parser "set_input_delay" $spec $args]

    set id_targets [_convert_to_objects "set_input_delay" [dict get $params targets]]

    set_input_delay_internal [dict get $params -max] [dict get $params -min] [dict get $params -clock] [dict get $params delay] $id_targets
}

proc set_output_delay {args} {
    # Set the line number from the caller's frame
    set frame_info [info frame -1]
    set line_num [dict get $frame_info line]
    lineno_internal $line_num

    set spec {
        flags   {-clock}
        bools   {-max -min}
        pos     {delay targets}
        require {-clock targets}
        types   {delay double}
    }

    set params [generic_sdc_parser "set_output_delay" $spec $args]

    set id_targets [_convert_to_objects "set_input_delay" [dict get $params targets]]

    set_output_delay_internal [dict get $params -max] [dict get $params -min] [dict get $params -clock] [dict get $params delay] $id_targets
}

proc get_name {object_id} {
    # Set the line number from the caller's frame
    set frame_info [info frame -1]
    set line_num [dict get $frame_info line]
    lineno_internal $line_num

    get_name_internal object_id
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
            if {[dict get $params -regexp]} {
                if {[regexp {*}$search_options -- $pattern $name]} {
                    set match 1
                    break;
                }
            } else {
                if {[string match {*}$search_options $pattern $name]} {
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
    # Set the line number from the caller's frame
    set frame_info [info frame -1]
    set line_num [dict get $frame_info line]
    lineno_internal $line_num

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
    # Set the line number from the caller's frame
    set frame_info [info frame -1]
    set line_num [dict get $frame_info line]
    lineno_internal $line_num

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

proc all_clocks {} {
    # Set the line number from the caller's frame
    set frame_info [info frame -1]
    set line_num [dict get $frame_info line]
    lineno_internal $line_num

    return [all_clocks_internal]
}
