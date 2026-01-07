#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "sdcparse.hpp"

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
        fflush(stdout);
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("create_clock -period %f -waveform {%f %f}",
                cmd.period,
                cmd.rise_edge,
                cmd.fall_edge);
        if (!cmd.name.empty()) {
            printf(" -name %s",
                   cmd.name.c_str());
        }
        if (!cmd.targets.strings.empty()) {
            printf(" ");
            print_string_group(cmd.targets);
        }
        if (cmd.add) {
            printf(" -add");
        }
        printf("\n");

        std::string clock_name = "";
        if (!cmd.name.empty()) {
            clock_name = cmd.name;
            obj_database.create_clock_object(clock_name);
        } else if (cmd.targets.strings.size() == 1) {
            if (cmd.targets.type == StringGroupType::OBJECT) {
                clock_name = obj_database.get_object_name(ObjectId(cmd.targets.strings[0]));
            } else {
                clock_name = cmd.targets.strings[0];
            }
            obj_database.create_clock_object(clock_name);
        } else {
            // If no name is given, create a clock for each target.
            // TODO: Verify that this is correct.
            for (const auto& target: cmd.targets.strings) {
                if (cmd.targets.type == StringGroupType::OBJECT) {
                    clock_name = obj_database.get_object_name(ObjectId(target));
                } else {
                    clock_name = target;
                }
                obj_database.create_clock_object(clock_name);
            }
        }
        fflush(stdout);
    }

    void create_generated_clock(const CreateGeneratedClock& cmd) override {
        fflush(stdout);
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("create_generated_clock");
        if (!cmd.name.empty()) {
            printf(" -name %s",
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
        printf(" -source %s", source_name.c_str());
        if (cmd.divide_by != sdcparse::UNINITIALIZED_INT)
            printf(" -divide_by %d", cmd.divide_by);
        if (cmd.multiply_by != sdcparse::UNINITIALIZED_INT)
            printf(" -multiply_by %d", cmd.multiply_by);
        if (cmd.add) {
            printf(" -add");
        }
        if (!cmd.targets.strings.empty()) {
            printf(" ");
            print_string_group(cmd.targets);
        }
        printf("\n");

        std::string clock_name = "";
        if (!cmd.name.empty()) {
            clock_name = cmd.name;
            obj_database.create_clock_object(clock_name);
        } else if (cmd.targets.strings.size() == 1) {
            if (cmd.targets.type == StringGroupType::OBJECT) {
                clock_name = obj_database.get_object_name(ObjectId(cmd.targets.strings[0]));
            } else {
                clock_name = cmd.targets.strings[0];
            }
            obj_database.create_clock_object(clock_name);
        } else {
            // If no name is given, create a clock for each target.
            // TODO: Verify that this is correct.
            for (const auto& target: cmd.targets.strings) {
                if (cmd.targets.type == StringGroupType::OBJECT) {
                    clock_name = obj_database.get_object_name(ObjectId(target));
                } else {
                    clock_name = target;
                }
                obj_database.create_clock_object(clock_name);
            }
        }
        fflush(stdout);
    }

    void set_io_delay(const SetIoDelay& cmd) override {
        fflush(stdout);
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        if(cmd.type == IoDelayType::INPUT) {
            printf("set_input_delay");
        } else {
            printf("set_output_delay");
        }
        printf(" -clock %s", cmd.clock_name.c_str());

        if (cmd.is_max) {
            printf(" -max");
        }
        if (cmd.is_min) {
            printf(" -min");
        }
        printf(" %f ", cmd.delay);
        print_string_group(cmd.target_ports);
        printf("\n");

        fflush(stdout);
    }
    void set_clock_groups(const SetClockGroups& cmd) override {
        fflush(stdout);
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("set_clock_groups");
        if (cmd.type == ClockGroupsType::LOGICALLY_EXCLUSIVE)
            printf(" -logically_exclusive");
        if (cmd.type == ClockGroupsType::PHYSICALLY_EXCLUSIVE)
            printf(" -physically_exclusive");
        if (cmd.type == ClockGroupsType::ASYNCHRONOUS)
            printf(" -asynchronous");
        // NOTE: This is deprecated and will be removed.
        if (cmd.type == ClockGroupsType::EXCLUSIVE)
            printf(" -exclusive");
        for(const auto& clk_grp : cmd.clock_groups) {
            printf(" -group ");
            print_string_group(clk_grp);
        }
        printf("\n");

        fflush(stdout);
    }

    void set_false_path(const SetFalsePath& cmd) override {
        fflush(stdout);
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("set_false_path ");
        print_from_to_group(cmd.from, cmd.to);
        printf("\n");
        fflush(stdout);
    }
    void set_min_max_delay(const SetMinMaxDelay& cmd) override {
        fflush(stdout);
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        if(cmd.type == MinMaxType::MAX) {
            printf("set_max_delay");
        } else {
            printf("set_min_delay");
        }
        printf(" %f ", cmd.value);
        print_from_to_group(cmd.from, cmd.to);
        printf("\n");
        fflush(stdout);
    }
    void set_multicycle_path(const SetMulticyclePath& cmd) override {
        fflush(stdout);
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("set_multicycle_path %d ", cmd.mcp_value);
        if(cmd.is_setup) {
            printf("-setup ");
        }
        if(cmd.is_hold) {
            printf("-hold ");
        }
        print_from_to_group(cmd.from, cmd.to);
        printf("\n");
        fflush(stdout);
    }
    void set_clock_uncertainty(const SetClockUncertainty& cmd) override {
        fflush(stdout);
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("set_clock_uncertainty ");
        if(cmd.is_setup) {
            printf("-setup ");
        } 
        if(cmd.is_hold) {
            printf("-hold ");
        }
        print_from_to_group(cmd.from, cmd.to);
        printf(" %f ", cmd.value);
        printf("\n");
        fflush(stdout);
    }
    void set_clock_latency(const SetClockLatency& cmd) override {
        fflush(stdout);
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("set_clock_latency ");
        if(cmd.type == ClockLatencyType::SOURCE) {
            printf("-source ");
        }
        if(cmd.is_early) {
            printf("-early ");
        }
        if(cmd.is_late) {
            printf("-late ");
        }
        printf("%f ", cmd.value);
        print_string_group(cmd.target_clocks);
        printf("\n");
        fflush(stdout);
    }
    void set_disable_timing(const SetDisableTiming& cmd) override {
        fflush(stdout);
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("set_disable_timing ");
        print_from_to_group(cmd.from, cmd.to);
        printf("\n");

        fflush(stdout);
    }
    void set_timing_derate(const SetTimingDerate& cmd) override {
        fflush(stdout);
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("set_timing_derate ");
        if(cmd.is_early) {
            printf("-early ");
        }
        if(cmd.is_late) {
            printf("-late ");
        }

        if(cmd.derate_nets) {
            printf("-net_delay ");
        }
        if(cmd.derate_cells) {
            printf("-cell_delay ");
        }
        printf("%f ", cmd.value);
        print_string_group(cmd.cell_targets);
        printf("\n");
        fflush(stdout);
    }

    //End of parsing
    void finish_parse() override { }

    //Error during parsing
    void parse_error(const int curr_lineno, const std::string& near_text, const std::string& msg) override {
        fflush(stdout);
        fprintf(stderr, "Custom Error");
        if(curr_lineno > 0) {
            fprintf(stderr, " at line %d", curr_lineno);
        }
        if(near_text != "") {
            if(near_text == "\n") {
                fprintf(stderr, " near '\\n'");
            } else if(near_text == "\n\r") {
                fprintf(stderr, " near '\\n\\r'");
            } else {
                fprintf(stderr, " near '%s'", near_text.c_str());
            }
        }
        fprintf(stderr, ": %s\n", msg.c_str());
        error_ = true;
        fflush(stdout);
    }

    // Warning during parsing
    void parse_warning(const std::string& msg) override {
        fflush(stdout);
        std::cout << "Warning at line " << lineno_ << ": " << msg << std::endl;
        fflush(stdout);
    }

    bool error() { return error_; }

private:
    std::string filename_ = "";
    int lineno_ = 0;
    bool error_ = false;

};

int main(int argc, char **argv) {
    if(argc != 2 && argc != 3) {
        fprintf(stderr, "Usage: %s filename.sdc [-legacy]\n", argv[0]);
        fprintf(stderr, "\n");
        fprintf(stderr, "Reads in an SDC file into internal data structures\n");
        fprintf(stderr, "and then prints it out.\n");
        fprintf(stderr, "Use -legacy to use the legacy (non-tcl) parser.\n");
        return 1;
    }

    bool use_tcl_interp = true;
    for (int i = 2; i < argc; i++) {
        if (std::string(argv[i]) == "-legacy") {
            use_tcl_interp = false;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
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
        printf("Unsupported sdc string group type\n");
        exit(1);
    }

    if(!group.strings.empty()) {
        printf("%s", start_token);
        for(size_t i = 0; i < group.strings.size(); ++i) {
            printf("%s", group.strings[i].c_str());

            if(i != group.strings.size() - 1) {
                printf(" ");
            }
        }
        printf("%s", end_token);
    }
}

void print_from_to_group(const StringGroup& from, const StringGroup& to) {
    if(!from.strings.empty()) {
        printf("-from ");
        print_string_group(from);
    }

    if(!to.strings.empty()) {
        printf(" -to ");
        print_string_group(to);
    }
}

