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

void print_object_id(ObjectId object_id);
void print_object_id_vec(const std::vector<ObjectId>& group);
void print_from_to_object_id_vec(const std::vector<ObjectId>& from, const std::vector<ObjectId>& to);

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
        if (!cmd.targets.empty()) {
            flushing_printf(" ");
            print_object_id_vec(cmd.targets);
        }
        if (cmd.add) {
            flushing_printf(" -add");
        }
        flushing_printf("\n");

        std::string clock_name = "";
        if (!cmd.name.empty()) {
            clock_name = cmd.name;
        } else {
            if (cmd.targets.empty()) {
                throw std::runtime_error("create_clock: clock name or target required");
            }
            // If no name is given, use the first target for the name. This
            // matches the behaviour of other tools.
            clock_name = obj_database.get_object_name(ObjectId(cmd.targets[0]));
        }
        obj_database.create_clock_object(clock_name);
    }

    void create_generated_clock(const CreateGeneratedClock& cmd) override {
        flushing_printf("#%s:%d\n", filename_.c_str(), lineno_);
        flushing_printf("create_generated_clock");
        if (!cmd.name.empty()) {
            flushing_printf(" -name %s",
                   cmd.name.c_str());
        }
        flushing_printf(" -source ");
        print_object_id_vec(cmd.sources);
        if (cmd.divide_by != sdcparse::UNINITIALIZED_INT)
            flushing_printf(" -divide_by %d", cmd.divide_by);
        if (cmd.multiply_by != sdcparse::UNINITIALIZED_INT)
            flushing_printf(" -multiply_by %d", cmd.multiply_by);
        if (cmd.add) {
            flushing_printf(" -add");
        }
        if (!cmd.targets.empty()) {
            flushing_printf(" ");
            print_object_id_vec(cmd.targets);
        }
        flushing_printf("\n");

        std::string clock_name = "";
        if (!cmd.name.empty()) {
            clock_name = cmd.name;
        } else {
            if (cmd.targets.empty()) {
                throw std::runtime_error("create_generated_clock: clock name or target required");
            }
            // If no name is given, use the first target for the name. This
            // matches the behaviour of other tools.
            clock_name = obj_database.get_object_name(ObjectId(cmd.targets[0]));
        }
        obj_database.create_clock_object(clock_name);
    }

    void set_io_delay(const SetIoDelay& cmd) override {
        flushing_printf("#%s:%d\n", filename_.c_str(), lineno_);
        if(cmd.type == IoDelayType::INPUT) {
            flushing_printf("set_input_delay");
        } else {
            flushing_printf("set_output_delay");
        }

        // The associated clocks can be empty, meaning the arrival time is
        // with respect to all clocks that arrive at the reference
        // pin.
        if (!cmd.associated_clocks.empty()) {
            flushing_printf(" -clock ");
            print_object_id_vec(cmd.associated_clocks);
        }

        if (cmd.is_max) {
            flushing_printf(" -max");
        }
        if (cmd.is_min) {
            flushing_printf(" -min");
        }
        flushing_printf(" %f ", cmd.delay);
        print_object_id_vec(cmd.target_ports);
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
            print_object_id_vec(clk_grp);
        }
        flushing_printf("\n");
    }

    void set_false_path(const SetFalsePath& cmd) override {
        flushing_printf("#%s:%d\n", filename_.c_str(), lineno_);
        flushing_printf("set_false_path ");
        print_from_to_object_id_vec(cmd.from, cmd.to);
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
        print_from_to_object_id_vec(cmd.from, cmd.to);
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
        print_from_to_object_id_vec(cmd.from, cmd.to);
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
        print_from_to_object_id_vec(cmd.from, cmd.to);
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
        print_object_id_vec(cmd.target_clocks);
        flushing_printf("\n");
    }
    void set_disable_timing(const SetDisableTiming& cmd) override {
        flushing_printf("#%s:%d\n", filename_.c_str(), lineno_);
        flushing_printf("set_disable_timing ");
        print_from_to_object_id_vec(cmd.from, cmd.to);
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
        print_object_id_vec(cmd.cell_targets);
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

    // Error message during parsing.
    void log_error_msg(const std::string& msg) override {
        flushing_fprintf(stderr, "%s", msg.c_str());
    }

    bool error() { return error_; }

private:
    std::string filename_ = "";
    int lineno_ = 0;
    bool error_ = false;

};

int main(int argc, char **argv) {
    if(argc != 2) {
        flushing_fprintf(stderr, "Usage: %s filename.sdc\n", argv[0]);
        flushing_fprintf(stderr, "\n");
        flushing_fprintf(stderr, "Reads in an SDC file into internal data structures\n");
        flushing_fprintf(stderr, "and then prints it out.\n");
        return 1;
    }

    PrintCallback callback;
    sdc_parse_filename(argv[1], callback);

    if(callback.error()) {
        return 1;
    }
    return 0;
}

void print_object_id(ObjectId object_id) {
    size_t object_id_val = static_cast<size_t>(object_id);
    flushing_printf("__vtr_obj_%lld", static_cast<long long int>(object_id_val));
}

void print_object_id_vec(const std::vector<ObjectId>& object_ids) {
    if (object_ids.empty())
        return;

    flushing_printf("{");
    for (size_t i = 0; i < object_ids.size(); ++i) {
        print_object_id(object_ids[i]);

        if (i != object_ids.size() - 1) {
            flushing_printf(" ");
        }
    }
    flushing_printf("}");
}

void print_from_to_object_id_vec(const std::vector<ObjectId>& from, const std::vector<ObjectId>& to) {
    if(!from.empty()) {
        flushing_printf("-from ");
        print_object_id_vec(from);
    }

    if(!to.empty()) {
        flushing_printf(" -to ");
        print_object_id_vec(to);
    }
}

