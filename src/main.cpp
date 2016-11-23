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

    void create_clock(std::string name, double period, double rise_edge, double fall_edge, StringGroup targets, bool is_virtual) override {
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("create_clock -period %f -waveform {%f %f} ",
                period,
                rise_edge,
                fall_edge);
        if(is_virtual) {
            printf("-name %s",
                   name.c_str());
        } else {
            print_string_group(targets);
        }
        printf("\n");
    }

    void set_io_delay(IoDelayType type, std::string clock_name, double max_delay, StringGroup target_ports) override {
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        if(type == IoDelayType::INPUT) {
            printf("set_input_delay");
        } else {
            printf("set_output_delay");
        }
        printf(" -clock %s -max %f ", 
               clock_name.c_str(),
               max_delay);
        print_string_group(target_ports);
        printf("\n");

    }
    void set_clock_groups(ClockGroupsType cg_type, std::vector<StringGroup> clock_groups) override {
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("set_clock_groups ");
        if(cg_type == ClockGroupsType::EXCLUSIVE) {
            printf(" -exclusive");
        }
        for(const auto& clk_grp : clock_groups) {
            printf(" -group ");
            print_string_group(clk_grp);
        }
        printf("\n");

    }

    void set_false_path(StringGroup from, StringGroup to) override {
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("set_false_path ");
        print_from_to_group(from, to);
        printf("\n");
    }
    void set_max_delay(double max_delay, StringGroup from, StringGroup to) override {
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("set_max_delay %f ", max_delay);
        print_from_to_group(from, to);
        printf("\n");
    }
    void set_multicycle_path(McpType mcp_type, int mcp_value, StringGroup from, StringGroup to) override {
        printf("#%s:%d\n", filename_.c_str(), lineno_);
        printf("set_multicycle_path %d ", mcp_value);
        if(mcp_type == McpType::SETUP) {
            printf("-setup ");
        }
        print_from_to_group(from, to);
        printf("\n");
    }

    //End of parsing
    void finish_parse() override { }

    //Error during parsing
    void parse_error(const int curr_lineno, const std::string& near_text, const std::string& msg) override {
        fprintf(stderr, "Custom Error at line %d near '%s': %s\n", curr_lineno, near_text.c_str(), msg.c_str());
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

