#include <stdio.h>
#include <stdlib.h>
#include "sdc.h"


int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s filename.sdc\n", argv[0]);
        fprintf(stderr, "\n");
        fprintf(stderr, "Reads in an SDC file into internal data structures\n");
        fprintf(stderr, "and then prints it out\n");
        exit(1);
    }

    //Parse the file
    t_sdc_commands* sdc_commands = sdc_parse_file(argv[1]);

    //Print out the commands
    for(int i = 0; i < sdc_commands->num_create_clock_cmds; i++) {
        t_sdc_create_clock* sdc_create_clock = sdc_commands->create_clock_cmds[i];
        if(sdc_create_clock->is_virtual) {
            printf("create_clock -period %f -waveform {%f %f} -name %s\n",
                    sdc_create_clock->period,
                    sdc_create_clock->rise_time,
                    sdc_create_clock->fall_time,
                    sdc_create_clock->name);
        } else {
            printf("create_clock -period %f -waveform {%f %f} %s\n",
                    sdc_create_clock->period,
                    sdc_create_clock->rise_time,
                    sdc_create_clock->fall_time,
                    sdc_create_clock->target);
        }
    }

    for(int i = 0; i < sdc_commands->num_set_input_delay_cmds; i++) {
        t_sdc_set_io_delay* sdc_set_input_delay = sdc_commands->set_input_delay_cmds[i];
        printf("set_input_delay -clock %s -max %f [get_ports {", 
                    sdc_set_input_delay->clock_name,
                    sdc_set_input_delay->max_delay);
        for(int j = 0; j < sdc_set_input_delay->target_ports->num_ports; j++) {
            printf("%s ", sdc_set_input_delay->target_ports->ports[j]);
        }
        printf("} ]\n");
    }

    for(int i = 0; i < sdc_commands->num_set_output_delay_cmds; i++) {
        t_sdc_set_io_delay* sdc_set_output_delay = sdc_commands->set_output_delay_cmds[i];
        printf("set_output_delay -clock %s -max %f [get_ports {", 
                    sdc_set_output_delay->clock_name,
                    sdc_set_output_delay->max_delay);
        for(int j = 0; j < sdc_set_output_delay->target_ports->num_ports; j++) {
            printf("%s ", sdc_set_output_delay->target_ports->ports[j]);
        }
        printf("} ]\n");
    }

    for(int i = 0; i < sdc_commands->num_set_clock_groups_cmds; i++) {
        t_sdc_set_clock_groups* sdc_set_clock_groups = sdc_commands->set_clock_groups_cmds[i];
        printf("set_clock_groups -exclusive");
        for(int j = 0; j < sdc_set_clock_groups->num_clock_groups; j++) {
            t_sdc_clock_group* sdc_clock_group = sdc_set_clock_groups->clock_groups[j];
            printf(" -group {");
            for(int k = 0; k < sdc_clock_group->num_clocks; k++) {
                printf("%s ", sdc_clock_group->clocks[k]);
            }
            printf("}");
        }
        printf("\n");
    }

    for(int i = 0; i < sdc_commands->num_set_false_path_cmds; i++) {
        t_sdc_set_false_path* sdc_set_false_path = sdc_commands->set_false_path_cmds[i];
        printf("set_false_path -from [get_clocks {");
        for(int j = 0; j < sdc_set_false_path->from_clocks->num_clocks; j++) {
            printf("%s ", sdc_set_false_path->from_clocks->clocks[j]);
        }
        printf("}] -to [get_clocks {");
        for(int j = 0; j < sdc_set_false_path->to_clocks->num_clocks; j++) {
            printf("%s ", sdc_set_false_path->to_clocks->clocks[j]);
        }
        printf("}]\n");
    }

    for(int i = 0; i < sdc_commands->num_set_max_delay_cmds; i++) {
        t_sdc_set_max_delay* sdc_set_max_delay = sdc_commands->set_max_delay_cmds[i];
        printf("set_max_delay %f -from [get_clocks {", sdc_set_max_delay->max_delay);
        for(int j = 0; j < sdc_set_max_delay->from_clocks->num_clocks; j++) {
            printf("%s ", sdc_set_max_delay->from_clocks->clocks[j]);
        }
        printf("}] -to [get_clocks {");
        for(int j = 0; j < sdc_set_max_delay->to_clocks->num_clocks; j++) {
            printf("%s ", sdc_set_max_delay->to_clocks->clocks[j]);
        }
        printf("}]\n");
    }

    for(int i = 0; i < sdc_commands->num_set_multicycle_path_cmds; i++) {
        t_sdc_set_multicycle_path* sdc_set_multicycle_path = sdc_commands->set_multicycle_path_cmds[i];
        printf("set_multicycle_path %d -setup -from [get_clocks {", sdc_set_multicycle_path->mcp_value);
        for(int j = 0; j < sdc_set_multicycle_path->from_clocks->num_clocks; j++) {
            printf("%s ", sdc_set_multicycle_path->from_clocks->clocks[j]);
        }
        printf("}] -to [get_clocks {");
        for(int j = 0; j < sdc_set_multicycle_path->to_clocks->num_clocks; j++) {
            printf("%s ", sdc_set_multicycle_path->to_clocks->clocks[j]);
        }
        printf("}]\n");
    }

    return 0;
}
