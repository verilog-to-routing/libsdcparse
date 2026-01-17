#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "sdcparse.hpp"

// The TCL parser uses its own buffers when printing to stdout. This means that
// when the sdcparse_test executable (main) prints to stdout, the order of
// printed messages may get reordered. We rely on order in the testcases to
// ensure that the proper output is generated. To fix this issue, we flush
// stdout before and after printing to stdout in this file. The first flush
// ensures that any messages from the TCL-side are ordered before the current
// print and the second flush ensures that any future tcl prints are ordered
// after the current print.
#define flushing_printf(...) do { \
    fflush(stdout);               \
    printf(__VA_ARGS__);          \
    fflush(stdout);               \
} while (0)

// This is the same as above, however it uses fprintf.
#define flushing_fprintf(file, ...) do { \
    fflush(file);                        \
    fprintf(file, __VA_ARGS__);          \
    fflush(file);                        \
} while (0)

using namespace sdcparse;

void print_string_group(const StringGroup& group);
void print_from_to_group(const StringGroup& from, const StringGroup& to);

class PrintCallback : public Callback {
public:
    //Start of parsing
    void start_parse() override { }

    //Sets current filename
    void filename(std::string fname) override { filename_ = fname; }

    //Sets current line number
    void lineno(int line_num) override { lineno_ = line_num; }

    void create_clock(const CreateClock& cmd) override {
        flushing_printf("#%s:%d\n", filename_.c_str(), lineno_);
        flushing_printf("create_clock -period %f -waveform {%f %f}",
                cmd.period,
                cmd.rise_edge,
                cmd.fall_edge);
        if (!cmd.name.empty()) {
            flushing_printf(" -name %s",
                   cmd.name.c_str());
        }
        if (!cmd.targets.strings.empty()) {
            flushing_printf(" ");
            print_string_group(cmd.targets);
        }
        if (cmd.add) {
            flushing_printf(" -add");
        }
        flushing_printf("\n");

        std::string clock_name = "";
        if (!cmd.name.empty()) {
            clock_name = cmd.name;
            obj_database.create_clock_object(clock_name);
        } else {
            if (cmd.targets.strings.empty()) {
                throw std::runtime_error("create_clock: clock name or target required");
            }
            // If no name is given, use the first target for the name. This
            // matches the behaviour of other tools.
            if (cmd.targets.type == StringGroupType::OBJECT) {
                clock_name = obj_database.get_object_name(ObjectId(cmd.targets.strings[0]));
            } else {
                clock_name = cmd.targets.strings[0];
            }
            obj_database.create_clock_object(clock_name);
        }
    }

    void create_generated_clock(const CreateGeneratedClock& cmd) override {
        flushing_printf("#%s:%d\n", filename_.c_str(), lineno_);
        flushing_printf("create_generated_clock");
        if (!cmd.name.empty()) {
            flushing_printf(" -name %s",
                   cmd.name.c_str());
        }
        std::string source_name;
        if (cmd.source_string_type == StringGroupType::OBJECT) {
            source_name = obj_database.get_object_name(ObjectId(cmd.source));
        } else if (cmd.source_string_type == StringGroupType::STRING) {
            source_name = cmd.source;
        } else {
            throw std::runtime_error("Unsupported string type group");
        }
        flushing_printf(" -source %s", source_name.c_str());
        if (cmd.divide_by != sdcparse::UNINITIALIZED_INT)
            flushing_printf(" -divide_by %d", cmd.divide_by);
        if (cmd.multiply_by != sdcparse::UNINITIALIZED_INT)
            flushing_printf(" -multiply_by %d", cmd.multiply_by);
        if (cmd.add) {
            flushing_printf(" -add");
        }
        if (!cmd.targets.strings.empty()) {
            flushing_printf(" ");
            print_string_group(cmd.targets);
        }
        flushing_printf("\n");

        std::string clock_name = "";
        if (!cmd.name.empty()) {
            clock_name = cmd.name;
            obj_database.create_clock_object(clock_name);
        } else {
            if (cmd.targets.strings.empty()) {
                throw std::runtime_error("create_clock: clock name or target required");
            }
            // If no name is given, use the first target for the name. This
            // matches the behaviour of other tools.
            if (cmd.targets.type == StringGroupType::OBJECT) {
                clock_name = obj_database.get_object_name(ObjectId(cmd.targets.strings[0]));
            } else {
                clock_name = cmd.targets.strings[0];
            }
            obj_database.create_clock_object(clock_name);
        }
    }

    void set_io_delay(const SetIoDelay& cmd) override {
        flushing_printf("#%s:%d\n", filename_.c_str(), lineno_);
        if(cmd.type == IoDelayType::INPUT) {
            flushing_printf("set_input_delay");
        } else {
            flushing_printf("set_output_delay");
        }

        // The clock name can be empty, meaning the arrival time is
        // with respect to all clocks that arrive at the reference
        // pin.
        if (!cmd.clock_name.empty())
            flushing_printf(" -clock %s", cmd.clock_name.c_str());

        if (cmd.is_max) {
            flushing_printf(" -max");
        }
        if (cmd.is_min) {
            flushing_printf(" -min");
        }
        flushing_printf(" %f ", cmd.delay);
        print_string_group(cmd.target_ports);
        flushing_printf("\n");
    }
    void set_clock_groups(const SetClockGroups& cmd) override {
        flushing_printf("#%s:%d\n", filename_.c_str(), lineno_);
        flushing_printf("set_clock_groups");
        if (cmd.type == ClockGroupsType::LOGICALLY_EXCLUSIVE)
            flushing_printf(" -logically_exclusive");
        if (cmd.type == ClockGroupsType::PHYSICALLY_EXCLUSIVE)
            flushing_printf(" -physically_exclusive");
        if (cmd.type == ClockGroupsType::ASYNCHRONOUS)
            flushing_printf(" -asynchronous");
        // NOTE: This is deprecated and will be removed.
        if (cmd.type == ClockGroupsType::EXCLUSIVE)
            flushing_printf(" -exclusive");
        for(const auto& clk_grp : cmd.clock_groups) {
            flushing_printf(" -group ");
            print_string_group(clk_grp);
        }
        flushing_printf("\n");
    }

    void set_false_path(const SetFalsePath& cmd) override {
        flushing_printf("#%s:%d\n", filename_.c_str(), lineno_);
        flushing_printf("set_false_path ");
        print_from_to_group(cmd.from, cmd.to);
        flushing_printf("\n");
    }
    void set_min_max_delay(const SetMinMaxDelay& cmd) override {
        flushing_printf("#%s:%d\n", filename_.c_str(), lineno_);
        if(cmd.type == MinMaxType::MAX) {
            flushing_printf("set_max_delay");
        } else {
            flushing_printf("set_min_delay");
        }
        flushing_printf(" %f ", cmd.value);
        print_from_to_group(cmd.from, cmd.to);
        flushing_printf("\n");
    }
    void set_multicycle_path(const SetMulticyclePath& cmd) override {
        flushing_printf("#%s:%d\n", filename_.c_str(), lineno_);
        flushing_printf("set_multicycle_path %d ", cmd.mcp_value);
        if(cmd.is_setup) {
            flushing_printf("-setup ");
        }
        if(cmd.is_hold) {
            flushing_printf("-hold ");
        }
        print_from_to_group(cmd.from, cmd.to);
        flushing_printf("\n");
    }
    void set_clock_uncertainty(const SetClockUncertainty& cmd) override {
        flushing_printf("#%s:%d\n", filename_.c_str(), lineno_);
        flushing_printf("set_clock_uncertainty ");
        if(cmd.is_setup) {
            flushing_printf("-setup ");
        } 
        if(cmd.is_hold) {
            flushing_printf("-hold ");
        }
        print_from_to_group(cmd.from, cmd.to);
        flushing_printf(" %f ", cmd.value);
        flushing_printf("\n");
    }
    void set_clock_latency(const SetClockLatency& cmd) override {
        flushing_printf("#%s:%d\n", filename_.c_str(), lineno_);
        flushing_printf("set_clock_latency ");
        if(cmd.type == ClockLatencyType::SOURCE) {
            flushing_printf("-source ");
        }
        if(cmd.is_early) {
            flushing_printf("-early ");
        }
        if(cmd.is_late) {
            flushing_printf("-late ");
        }
        flushing_printf("%f ", cmd.value);
        print_string_group(cmd.target_clocks);
        flushing_printf("\n");
    }
    void set_disable_timing(const SetDisableTiming& cmd) override {
        flushing_printf("#%s:%d\n", filename_.c_str(), lineno_);
        flushing_printf("set_disable_timing ");
        print_from_to_group(cmd.from, cmd.to);
        flushing_printf("\n");
    }
    void set_timing_derate(const SetTimingDerate& cmd) override {
        flushing_printf("#%s:%d\n", filename_.c_str(), lineno_);
        flushing_printf("set_timing_derate ");
        if(cmd.is_early) {
            flushing_printf("-early ");
        }
        if(cmd.is_late) {
            flushing_printf("-late ");
        }

        if(cmd.derate_nets) {
            flushing_printf("-net_delay ");
        }
        if(cmd.derate_cells) {
            flushing_printf("-cell_delay ");
        }
        flushing_printf("%f ", cmd.value);
        print_string_group(cmd.cell_targets);
        flushing_printf("\n");
    }

    //End of parsing
    void finish_parse() override { }

    //Error during parsing
    void parse_error(const int curr_lineno, const std::string& near_text, const std::string& msg) override {
        flushing_fprintf(stderr, "Custom Error");
        if(curr_lineno > 0) {
            flushing_fprintf(stderr, " at line %d", curr_lineno);
        }
        if(near_text != "") {
            if(near_text == "\n") {
                flushing_fprintf(stderr, " near '\\n'");
            } else if(near_text == "\n\r") {
                flushing_fprintf(stderr, " near '\\n\\r'");
            } else {
                flushing_fprintf(stderr, " near '%s'", near_text.c_str());
            }
        }
        flushing_fprintf(stderr, ": %s\n", msg.c_str());
        error_ = true;
    }

    // Warning during parsing
    void parse_warning(const std::string& msg) override {
        flushing_printf("Warning at line %d: %s\n", lineno_, msg.c_str());
    }

    bool error() { return error_; }

private:
    std::string filename_ = "";
    int lineno_ = 0;
    bool error_ = false;

};

int main(int argc, char **argv) {
    if(argc != 2 && argc != 3) {
        flushing_fprintf(stderr, "Usage: %s filename.sdc [-legacy]\n", argv[0]);
        flushing_fprintf(stderr, "\n");
        flushing_fprintf(stderr, "Reads in an SDC file into internal data structures\n");
        flushing_fprintf(stderr, "and then prints it out.\n");
        flushing_fprintf(stderr, "Use -legacy to use the legacy (non-tcl) parser.\n");
        return 1;
    }

    bool use_tcl_interp = true;
    for (int i = 2; i < argc; i++) {
        if (std::string(argv[i]) == "-legacy") {
            use_tcl_interp = false;
        } else {
            flushing_fprintf(stderr, "Unknown option: %s\n", argv[i]);
            return 1;
        }
    }

    PrintCallback callback;
    sdc_parse_filename(argv[1], callback, use_tcl_interp);

    if(callback.error()) {
        return 1;
    }
    return 0;
}

void print_string_group(const StringGroup& group) {
    const char *start_token, *end_token;
    if(group.type == StringGroupType::STRING) {
        start_token = "{";
        end_token   = "}";
    } else if (group.type == StringGroupType::OBJECT) {
        start_token = "{";
        end_token   = "}";
    } else if (group.type == StringGroupType::CLOCK) {
        start_token = "[get_clocks {";
        end_token   = "}]";

    } else if (group.type == StringGroupType::PORT) {
        start_token = "[get_ports {";
        end_token   = "}]";

    } else if (group.type == StringGroupType::CELL) {
        start_token = "[get_cells {";
        end_token   = "}]";
    } else if (group.type == StringGroupType::PIN) {
        start_token = "[get_pins {";
        end_token   = "}]";

    } else {
        flushing_printf("Unsupported sdc string group type\n");
        exit(1);
    }

    if(!group.strings.empty()) {
        flushing_printf("%s", start_token);
        for(size_t i = 0; i < group.strings.size(); ++i) {
            flushing_printf("%s", group.strings[i].c_str());

            if(i != group.strings.size() - 1) {
                flushing_printf(" ");
            }
        }
        flushing_printf("%s", end_token);
    }
}

void print_from_to_group(const StringGroup& from, const StringGroup& to) {
    if(!from.strings.empty()) {
        flushing_printf("-from ");
        print_string_group(from);
    }

    if(!to.strings.empty()) {
        flushing_printf(" -to ");
        print_string_group(to);
    }
}

