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
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("create_clock -period %f -waveform {%f %f} ",
                cmd.period,
                cmd.rise_edge,
                cmd.fall_edge);
        if(cmd.is_virtual) {
            printf("-name %s",
                   cmd.name.c_str());
        } else {
            print_string_group(cmd.targets);
        }
        printf("\n");
    }

    void set_io_delay(const SetIoDelay& cmd) override {
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        if(cmd.type == IoDelayType::INPUT) {
            printf("set_input_delay");
        } else {
            printf("set_output_delay");
        }
        printf(" -clock %s -max %f ", 
               cmd.clock_name.c_str(),
               cmd.max_delay);
        print_string_group(cmd.target_ports);
        printf("\n");

    }
    void set_clock_groups(const SetClockGroups& cmd) override {
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("set_clock_groups ");
        if(cmd.type == ClockGroupsType::EXCLUSIVE) {
            printf(" -exclusive");
        }
        for(const auto& clk_grp : cmd.clock_groups) {
            printf(" -group ");
            print_string_group(clk_grp);
        }
        printf("\n");

    }

    void set_false_path(const SetFalsePath& cmd) override {
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("set_false_path ");
        print_from_to_group(cmd.from, cmd.to);
        printf("\n");
    }
    void set_max_delay(const SetMaxDelay& cmd) override {
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("set_max_delay %f ", cmd.max_delay);
        print_from_to_group(cmd.from, cmd.to);
        printf("\n");
    }
    void set_multicycle_path(const SetMulticyclePath& cmd) override {
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("set_multicycle_path %d ", cmd.mcp_value);
        if(cmd.type == McpType::SETUP) {
            printf("-setup ");
        }
        print_from_to_group(cmd.from, cmd.to);
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

void print_string_group(const StringGroup& group) {
    const char *start_token, *end_token;
    if(group.group_type == StringGroupType::STRING) {
        start_token = "{";
        end_token   = "}";

    } else if (group.group_type == StringGroupType::CLOCK) {
        start_token = "[get_clocks {";
        end_token   = "}]";

    } else if (group.group_type == StringGroupType::PORT) {
        start_token = "[get_ports {";
        end_token   = "}]";

    } else {
        printf("Unsupported sdc string group type\n");
        exit(1);
    }

    printf("%s", start_token);
    for(size_t i = 0; i < group.strings.size(); ++i) {
        printf("%s", group.strings[i].c_str());

        if(i != group.strings.size() - 1) {
            printf(" ");
        }
    }
    printf("%s", end_token);
}

void print_from_to_group(const StringGroup& from, const StringGroup& to) {
    printf("-from ");
    print_string_group(from);
    printf(" -to ");
    print_string_group(to);
}

