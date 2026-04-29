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

/**
 * @brief Parse the given SDC file using the given callback object.
 *
 * This will invoke a TCL interpreter which will execute the SDC commands
 * in the given file, which will call the given callback methods.
 *
 *  @param filename     Path to the SDC file to interpret.
 *  @param callback     Callbacks to execute when the SDC commands are parsed.
 */
void sdc_parse_filename(const std::string& filename, Callback& callback);

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

/*
 * Structures defining different SDC commands
 */
struct CreateClock {
    std::string name = "";                      //Name of the clock
    double period = UNINITIALIZED_FLOAT;        //Clock period
    double rise_edge = UNINITIALIZED_FLOAT;     //Rise time from waveform definition
    double fall_edge = UNINITIALIZED_FLOAT;     //Fall time from waveform definition
    std::vector<ObjectId> targets;              //The list of objects indicating clock sources.
    bool is_virtual = false;                    //Identifies this as a virtual (non-netlist) clock
    bool add = false;
};

struct CreateGeneratedClock {
    std::string name = "";                      //Name of the generated clock
    std::vector<ObjectId> sources;              //The master clock pin(s)/port(s) this clock is derived from
    int divide_by = UNINITIALIZED_INT;          //Divide the master clock frequency by this integer (UNINITIALIZED_INT if unset)
    int multiply_by = UNINITIALIZED_INT;        //Multiply the master clock frequency by this integer (UNINITIALIZED_INT if unset)
                                                // Note: divide_by, multiply_by, and edges are mutually exclusive; exactly one of
                                                // divide_by, multiply_by, or edges must be provided
    bool add = false;                           //If true, add this clock to any existing clocks on the target rather
                                                // than replacing them
    std::vector<double> edges;                  //List of 3 master-clock edge indices that define the rising edge,
                                                // falling edge, and next rising edge of the generated clock waveform
    std::vector<double> edge_shift;             //Per-edge time offset (in the master clock period) applied to each
                                                // edge in the edges list; must be the same length as edges
    bool invert = false;                        //If true, invert the generated clock signal relative to the master clock;
                                                // only valid with divide_by or multiply_by
    double duty_cycle = UNINITIALIZED_FLOAT;    //Duty cycle as a percentage (0.0–100.0) for the generated clock when
                                                // using multiply_by (UNINITIALIZED_FLOAT if unset)
    std::vector<ObjectId> targets;              //The netlist objects (ports/pins/nets) on which the generated clock is created
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
    std::vector<ObjectId> associated_clocks;        //The clocks this constraint is associated with
    double delay = UNINITIALIZED_FLOAT;             //The maximum input delay allowed on the target ports
    std::vector<ObjectId> target_ports;             //The target ports
};

struct SetClockGroups {
    ClockGroupsType type = ClockGroupsType::NONE;   //The type of clock group relation being specified
    std::vector<std::vector<ObjectId>> clock_groups;//The groups of clocks
};

struct SetFalsePath {
    std::vector<ObjectId> from;                     //The source list of startpoints or clocks
    std::vector<ObjectId> to;                       //The target list of endpoints or clocks
};

struct SetMinMaxDelay {
    SetMinMaxDelay() = default;
    SetMinMaxDelay(MinMaxType delay_type)
        : type(delay_type) {}
    MinMaxType type = MinMaxType::NONE;         //Whether this is a min or max delay
    double value = UNINITIALIZED_FLOAT;         //The maximum/minimum allowed delay between the from
                                                // and to clocks
    std::vector<ObjectId> from;                 //The source list of startpoints or clocks
    std::vector<ObjectId> to;                   //The target list of endpoints or clocks
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
    std::vector<ObjectId> from;                 //The source list of startpoints or clocks
    std::vector<ObjectId> to;                   //The target list of endpoints or clocks
};

struct SetClockUncertainty {
    bool is_setup = false;                      //Does value apply for setup?
    bool is_hold = false;                       //Does value apply for hold?
                                                // Note: is_setup/is_hold correspond to whether the option was 
                                                // provided, it is up to the application to handle the case 
                                                // where both are left unspecified (which SDC treats as 
                                                // implicitly specifying both)
    float value = UNINITIALIZED_FLOAT;          //The uncertainty value

    std::vector<ObjectId> from;                 //Launch clock domain(s)
    std::vector<ObjectId> to;                   //Capture clock domain(s)
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

    std::vector<ObjectId> target_clocks;           //The target clocks
};

struct SetDisableTiming {
    std::vector<ObjectId> from;                    //The source pins
    std::vector<ObjectId> to;                      //The sink pins
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

    std::vector<ObjectId> cell_targets;         //The (possibly empty) set of target cells
};

} //namespace
