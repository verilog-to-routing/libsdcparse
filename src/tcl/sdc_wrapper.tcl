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
            break
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
    set positional_values $remaining
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

proc create_clock {args} {
    # Set the line number from the caller's frame
    # TODO: Can this be made more general?
    set frame_info [info frame -1]
    set line_num [dict get $frame_info line]
    lineno_internal $line_num

    # TODO: Either target or name is required. Need to improve the parser to handle
    #       this.
    # TODO: The add command appears to only work if it comes before the positional
    #       arguments. This should be supported. Thus the generic parser needs to
    #       be corrected.
    set spec {
        flags   {-period -waveform -name}
        bools   {-add}
        pos     {targets}
        require {-period}
        types   {-period double}
    }

    # TODO: If this command is to fail, is there a way for us to print the line
    #       number of the calling function? The user would never see this file.

    set params [generic_sdc_parser "create_clock" $spec $args]

    # Internal logic call
    # puts "Executing internal create_clock..."
    # puts "  Period:  [dict get $params -period]"
    # puts "  Targets: [dict get $params targets]"
    # if {[dict get $params -add]} { puts "  Mode:    Add (True)" }

    # TODO: Verify in TCL land that waveform has length two and maybe that
    #       the first element is smaller than the second.


    # TODO: Can we improve this function interface to wrap onto multiple lines
    #       without messing with the strings?
    create_clock_internal [dict get $params -period] [dict get $params -name] [dict get $params -waveform] [dict get $params -add] [dict get $params targets]
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

    set_input_delay_internal [dict get $params -max] [dict get $params -min] [dict get $params -clock] [dict get $params delay] [dict get $params targets]
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

    set_output_delay_internal [dict get $params -max] [dict get $params -min] [dict get $params -clock] [dict get $params delay] [dict get $params targets]
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

    # TODO: Handle regexp, nocase, and quiet args

    set search_options {-all -inline}
    if {[dict get $params -regexp]} {
        lappend search_options -regexp
    }
    if {[dict get $params -nocase]} {
        lappend search_options -nocase
    }

    # TODO: This may become very slow if we need to get all the ports everytime
    #       we search. We may be able to create a global variable which gets loaded
    #       once per circuit.
    set matched_items {}

    foreach pattern [dict get $params patterns] {
        set matches [lsearch {*}$search_options [all_ports_internal] $pattern]
        lappend matched_items {*}$matches
    }
    set unique_matches [lsort -unique $matched_items]

    # If unique matches is empty, raise error unless quiet is active.
    if {[llength $unique_matches] == 0 && ![dict get $params -quiet]} {
        puts "Warning: no matches found for get_ports $args"
    }

    return $unique_matches
}
