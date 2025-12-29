#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "sdcparse.hpp"

using namespace sdcparse;

void print_string_group(const StringGroup& group, const TimingObjectDatabase& db);
void print_from_to_group(const StringGroup& from, const StringGroup& to, const TimingObjectDatabase& db);

class PrintCallback : public Callback {
public:
    //Start of parsing
    void start_parse() override {
    }

    //Sets current filename
    void filename(std::string fname) override { filename_ = fname; }

    //Sets current line number
    void lineno(int line_num) override { lineno_ = line_num; }

    void create_clock(const CreateClock& cmd) override {
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
            print_string_group(cmd.targets, obj_database);
        }
        if (cmd.add) {
            printf(" -add");
        }
        printf("\n");

        std::string clock_name = "";
        if (!cmd.name.empty()) {
            clock_name = cmd.name;
        } else if (cmd.targets.strings.size() == 1) {
            if (cmd.targets.type == StringGroupType::OBJECT) {
                clock_name = obj_database.get_object_name(cmd.targets.strings[0]);
            } else {
                clock_name = cmd.targets.strings[0];
            }
        } else {
            // TODO: Figure out what to do in this case. Should we just not create
            //       the clock object? Or maybe give it a random name?
            // I wonder if each target would get its own clock?
            printf("Cannot create a name for the clock.\n");
            exit(1);
        }
        obj_database.create_clock_object(clock_name);
    }

    void set_io_delay(const SetIoDelay& cmd) override {
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
        print_string_group(cmd.target_ports, obj_database);
        printf("\n");

    }
    void set_clock_groups(const SetClockGroups& cmd) override {
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("set_clock_groups");
        if(cmd.type == ClockGroupsType::EXCLUSIVE) {
            printf(" -exclusive");
        }
        for(const auto& clk_grp : cmd.clock_groups) {
            printf(" -group ");
            print_string_group(clk_grp, obj_database);
        }
        printf("\n");

    }

    void set_false_path(const SetFalsePath& cmd) override {
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("set_false_path ");
        print_from_to_group(cmd.from, cmd.to, obj_database);
        printf("\n");
    }
    void set_min_max_delay(const SetMinMaxDelay& cmd) override {
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        if(cmd.type == MinMaxType::MAX) {
            printf("set_max_delay");
        } else {
            printf("set_min_delay");
        }
        printf(" %f ", cmd.value);
        print_from_to_group(cmd.from, cmd.to, obj_database);
        printf("\n");
    }
    void set_multicycle_path(const SetMulticyclePath& cmd) override {
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("set_multicycle_path %d ", cmd.mcp_value);
        if(cmd.is_setup) {
            printf("-setup ");
        }
        if(cmd.is_hold) {
            printf("-hold ");
        }
        print_from_to_group(cmd.from, cmd.to, obj_database);
        printf("\n");
    }
    void set_clock_uncertainty(const SetClockUncertainty& cmd) override {
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("set_clock_uncertainty ");
        if(cmd.is_setup) {
            printf("-setup ");
        } 
        if(cmd.is_hold) {
            printf("-hold ");
        }
        print_from_to_group(cmd.from, cmd.to, obj_database);
        printf(" %f ", cmd.value);
        printf("\n");
    }
    void set_clock_latency(const SetClockLatency& cmd) override {
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
        print_string_group(cmd.target_clocks, obj_database);
        printf("\n");
    }
    void set_disable_timing(const SetDisableTiming& cmd) override {
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("set_disable_timing ");
        print_from_to_group(cmd.from, cmd.to, obj_database);
        printf("\n");

    }
    void set_timing_derate(const SetTimingDerate& cmd) override {
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
        print_string_group(cmd.cell_targets, obj_database);
        printf("\n");
    }

    //End of parsing
    void finish_parse() override { }

    //Error during parsing
    void parse_error(const int curr_lineno, const std::string& near_text, const std::string& msg) override {
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
    }

    bool error() { return error_; }

private:
    std::string filename_ = "";
    int lineno_ = 0;
    bool error_ = false;

};

int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s filename.sdc\n", argv[0]);
        fprintf(stderr, "\n");
        fprintf(stderr, "Reads in an SDC file into internal data structures\n");
        fprintf(stderr, "and then prints it out\n");
        exit(1);
    }

    PrintCallback callback;
    sdc_parse_filename(argv[1], callback);

    if(callback.error()) {
        return 1;
    }
    return 0;
}

void print_string_group(const StringGroup& group, const TimingObjectDatabase& db) {
    const char *start_token, *end_token;
    if(group.type == StringGroupType::STRING || group.type == StringGroupType::OBJECT) {
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
            if (group.type == StringGroupType::OBJECT) {
                printf("%s", db.get_object_name(group.strings[i]).c_str());
            } else {
                printf("%s", group.strings[i].c_str());
            }

            if(i != group.strings.size() - 1) {
                printf(" ");
            }
        }
        printf("%s", end_token);
    }
}

void print_from_to_group(const StringGroup& from, const StringGroup& to, const TimingObjectDatabase& db) {
    if(!from.strings.empty()) {
        printf("-from ");
        print_string_group(from, db);
    }

    if(!to.strings.empty()) {
        printf(" -to ");
        print_string_group(to, db);
    }
}

