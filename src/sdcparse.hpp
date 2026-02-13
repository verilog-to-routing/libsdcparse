#pragma once
/*
 * libsdcparse - Kevin E. Murray 2014
 *             - Updated by Alex Singer 2026
 *
 * Released under MIT License see LICENSE.txt for details.
 *
 * OVERVIEW
 * --------------------------
 * This library provides basic parsing capabilities for a subset of commands in
 * Synopsys Design Constraint (SDC) files.  SDC files are typically used to 
 * set timing constraints on digital circuits.
 *
 * USING THIS LIBRARY
 * --------------------------
 * This library uses a full TCL interpreter to parse incoming SDC files. It includes
 * an internal database of netlist objects and their names. At the start of parsing,
 * the application will update the database with all of the netlist objects that
 * it expects to be referenced in the SDC file. The parser will then match for these
 * objects. The parser will communicate these objects with the application using
 * unique object IDs, where an ID refers to some database object created by the
 * application.
 *
 * After parsing, each SDC command is represented as a C struct.  Typically each
 * command is parsed into a unique type of struct, however some closely related commands
 * (such as set_input_delay and set_output_delay) may share a struct and be identified
 * by a 'type' field in the struct.
 *
 * All supported SDC commands are collected into a commands struct which
 * represents the entire SDC file.
 *
 * See the associated main.c for example usage.
 *
 * EXTENDING THE LIBRARY
 * --------------------------
 * The parser uses a full TCL interpreter to parse and execute SDC files. This allows
 * support for the complete SDC specification including dynamic behavior and TCL constructs.
 *
 * To extend the library with new SDC commands or options, follow these steps:
 *
 *      1) To add a new option to an existing command
 *          a) Identify the corresponding TCL command implementation in the codebase
 *             (typically in src/tcl/sdc_commands.cpp).
 *          b) Extend the TCL command handler to recognize and process the new option.
 *          c) Update the corresponding C struct (if needed) to store the new option value.
 *          d) Ensure the option is properly validated and any conflicts with existing
 *             options are detected and reported. Use the callback mechanism to report
 *             errors via parse_error() or parse_warning().
 *          e) Update the Callback interface if new data needs to be passed to consuming code.
 *
 *      2) To add a new command
 *          a) Create a new TCL command handler and register it with the TCL interpreter
 *             (typically in src/tcl/sdc_commands.cpp).
 *          b) Create a new C struct to represent the command data.
 *          c) Implement the TCL command to parse its arguments and populate the struct.
 *          d) Add a new callback method to the Callback interface
 *             (e.g., virtual void your_command(const YourCommand& cmd) = 0;).
 *          e) Invoke the callback from the TCL command handler to notify consuming code
 *             of the parsed command.
 *
 */
#include <cassert>
#include <vector>
#include <string>
#include <limits>

#include "sdc_timing_object_database.h"

namespace sdcparse {
/*
 * Forward declarations
 */
enum class IoDelayType;
enum class ClockGroupsType;
enum class FromToType;
enum class McpType;
enum class StringGroupType;

struct CreateClock;
struct CreateGeneratedClock;
struct SetIoDelay;
struct SetClockGroups;
struct SetFalsePath;
struct SetMinMaxDelay;
struct SetMulticyclePath;
struct SetClockUncertainty;
struct SetClockLatency;
struct SetDisableTiming;
struct SetTimingDerate;

struct StringGroup;

class Callback {

    public:
        virtual ~Callback() {}

        //Start of parsing
        virtual void start_parse() = 0;

        //Sets current filename
        virtual void filename(std::string fname) = 0;

        //Sets current line number
        virtual void lineno(int line_num) = 0;

        virtual void create_clock(const CreateClock& cmd) = 0;
        virtual void create_generated_clock(const CreateGeneratedClock& cmd) = 0;
        virtual void set_io_delay(const SetIoDelay& cmd) = 0;
        virtual void set_clock_groups(const SetClockGroups& cmd) = 0;
        virtual void set_false_path(const SetFalsePath& cmd) = 0;
        virtual void set_min_max_delay(const SetMinMaxDelay& cmd) = 0;
        virtual void set_multicycle_path(const SetMulticyclePath& cmd) = 0;
        virtual void set_clock_uncertainty(const SetClockUncertainty& cmd) = 0;
        virtual void set_clock_latency(const SetClockLatency& cmd) = 0;
        virtual void set_disable_timing(const SetDisableTiming& cmd) = 0;
        virtual void set_timing_derate(const SetTimingDerate& cmd) = 0;

        //End of parsing
        virtual void finish_parse() = 0;

        //Error during parsing
        virtual void parse_error(const int curr_lineno, const std::string& near_text, const std::string& msg) = 0;

        virtual void parse_warning(const std::string& msg) = 0;

        // Log error message in downstream tool.
        virtual void log_error_msg(const std::string& msg) = 0;

    public:
        TimingObjectDatabase obj_database;
};

/*
 * External functions for loading an SDC file
 */
void sdc_parse_filename(std::string filename, Callback& callback);
void sdc_parse_filename(const char* filename, Callback& callback);

/*
 * Sentinal values
 */
constexpr double UNINITIALIZED_FLOAT = std::numeric_limits<double>::quiet_NaN();
constexpr int UNINITIALIZED_INT = -1;

/*
 * Enumerations to describe specific SDC command types and attributes
 */
enum class IoDelayType {
    INPUT, 
    OUTPUT
};

enum class MinMaxType {
    MIN,
    MAX,
    NONE
};

enum class ClockGroupsType {
    NONE,
    LOGICALLY_EXCLUSIVE,
    PHYSICALLY_EXCLUSIVE,
    ASYNCHRONOUS,
    // NOTE: This is deprecated and will be removed.
    EXCLUSIVE,
};

enum class FromToType {
    FROM,
    TO
};

enum class ClockLatencyType {
    SOURCE,
    NONE
};

enum class StringGroupType {
    STRING, 
    PORT, 
    CLOCK,
    CELL,
    PIN,
    OBJECT
};

/*
 * Common SDC data structures
 */

struct StringGroup {
    StringGroup() = default;
    StringGroup(StringGroupType group_type)
        : type(group_type) {}

    StringGroupType type = StringGroupType::STRING;   //The type of the string group, default is STRING. 
                                                            // Groups derived from 'calls' to [get_clocks {...}] 
                                                            // and [get_ports {...}] will have types SDC_CLOCK 
                                                            // and SDC_PORT respectively.
    std::vector<std::string> strings;                       //The strings in the group
};

/*
 * Structures defining different SDC commands
 */
struct CreateClock {
    std::string name = "";                      //Name of the clock
    double period = UNINITIALIZED_FLOAT;        //Clock period
    double rise_edge = UNINITIALIZED_FLOAT;     //Rise time from waveform definition
    double fall_edge = UNINITIALIZED_FLOAT;     //Fall time from waveform definition
    StringGroup targets;                        //The set of strings indicating clock sources.
                                                // May be explicit strings or regexs.
    bool is_virtual = false;                    //Identifies this as a virtual (non-netlist) clock
    bool add = false;
};

struct CreateGeneratedClock {
    std::string name = "";
    std::string source = "";
    StringGroupType source_string_type = StringGroupType::STRING;
    int divide_by = UNINITIALIZED_INT;
    int multiply_by = UNINITIALIZED_INT;
    bool add = false;
    StringGroup targets;
};

struct SetIoDelay {
    SetIoDelay() = default;
    SetIoDelay(IoDelayType io_type)
        : type(io_type) {}

    IoDelayType type = IoDelayType::INPUT;          //Identifies whether this represents a
                                                    // set_input_delay or set_output delay
                                                    // command.
    bool is_min = false;                            //Does delay apply for maximum delays?
    bool is_max = false;                            //Does delay apply for minimum delays?
                                                    // Note: is_min/is_max correspond to whether the option was 
                                                    // provided, it is up to the application to handle the case 
                                                    // where both are left unspecified (which SDC treats as 
                                                    // implicitly specifying both)
    std::string clock_name = "";                    //Name of the clock this constraint is associated with
    double delay = UNINITIALIZED_FLOAT;             //The maximum input delay allowed on the target ports
    StringGroup target_ports;                       //The target ports
};

struct SetClockGroups {
    ClockGroupsType type = ClockGroupsType::NONE;   //The type of clock group relation being specified
    std::vector<StringGroup> clock_groups;          //The groups of clocks
};

struct SetFalsePath {
    StringGroup from;                           //The source list of startpoints or clocks
    StringGroup to;                             //The target list of endpoints or clocks
};

struct SetMinMaxDelay {
    SetMinMaxDelay() = default;
    SetMinMaxDelay(MinMaxType delay_type)
        : type(delay_type) {}
    MinMaxType type = MinMaxType::NONE;         //Whether this is a min or max delay
    double value = UNINITIALIZED_FLOAT;         //The maximum/minimum allowed delay between the from
                                                // and to clocks
    StringGroup from;                           //The source list of startpoints or clocks
    StringGroup to;                             //The target list of endpoints or clocks
};

struct SetMulticyclePath {
    bool is_setup = false;                      //Does mcp_value apply for setup?
    bool is_hold = false;                       //Does mcp_value apply for hold?
                                                // Note: is_setup/is_hold correspond to whether the option was 
                                                // provided, it is up to the application to handle the case 
                                                // where both are left unspecified (which SDC treats as
                                                // applying mcp_value for the setup mcp, and 0 for the hold 
                                                // mcp)
    int mcp_value = UNINITIALIZED_INT;          //The number of cycles specifed
    StringGroup from;                           //The source list of startpoints or clocks
    StringGroup to;                             //The target list of endpoints or clocks
};

struct SetClockUncertainty {
    bool is_setup = false;                      //Does value apply for setup?
    bool is_hold = false;                       //Does value apply for hold?
                                                // Note: is_setup/is_hold correspond to whether the option was 
                                                // provided, it is up to the application to handle the case 
                                                // where both are left unspecified (which SDC treats as 
                                                // implicitly specifying both)
    float value = UNINITIALIZED_FLOAT;          //The uncertainty value

    StringGroup from;                           //Launch clock domain(s)
    StringGroup to;                             //Capture clock domain(s)
};

struct SetClockLatency {
    ClockLatencyType type = ClockLatencyType::NONE;//Latency type
    bool is_early = false;                         //Does value apply for early transitions?
    bool is_late = false;                          //Does value apply for late transitions?
                                                   // Note: is_early/is_late correspond to whether the option was 
                                                   // provided, it is up to the application to handle the case 
                                                   // where both are left unspecified (which SDC treats as 
                                                   // implicitly specifying both)
    float value = UNINITIALIZED_FLOAT;             //The latency value

    StringGroup target_clocks;                     //The target clocks
};

struct SetDisableTiming {
    StringGroup from;                           //The source pins
    StringGroup to;                             //The sink pins
};

struct SetTimingDerate {
    bool is_early = false;                      //Does value apply for early transitions?
    bool is_late = false;                       //Does value apply for late transitions?
                                                // Note: is_early/is_late correspond to whether the option was 
                                                // provided, it is up to the application to handle the case 
                                                // where both are left unspecified (which SDC treats as 
                                                // implicitly specifying both)
    bool derate_nets = false;                   //Should nets be derated?
    bool derate_cells = false;                  //Should cells be derated?

    float value = UNINITIALIZED_FLOAT;          //The derate value

    StringGroup cell_targets;                   //The (possibly empty) set of target cells
};

} //namespace
