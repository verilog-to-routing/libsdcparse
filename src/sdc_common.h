#ifndef SDC_COMMON
#define SDC_COMMON
/*
 * Forward declarations
 */
typedef struct s_sdc_commands t_sdc_commands;

typedef struct s_sdc_create_clock t_sdc_create_clock;
typedef struct s_sdc_set_io_delay t_sdc_set_io_delay;
typedef struct s_sdc_set_clock_groups t_sdc_set_clock_groups;
typedef struct s_sdc_set_false_path t_sdc_set_false_path;
typedef struct s_sdc_set_max_delay t_sdc_set_max_delay;
typedef struct s_sdc_set_multicycle_path t_sdc_set_multicycle_path;

typedef struct s_sdc_clock_group t_sdc_clock_group;
typedef struct s_sdc_port_group t_sdc_port_group;
typedef struct s_sdc_string_group t_sdc_string_group;

/*
 * Enumerations to describe specific SDC command types
 */
typedef enum e_io_delay_type {INPUT_DELAY, OUTPUT_DELAY} t_sdc_io_delay_type;
typedef enum e_clock_groups_type {CG_NONE, CG_EXCLUSIVE} t_sdc_clock_groups_type;
typedef enum e_clock_group_dir {TO, FROM} t_sdc_clock_group_dir;
typedef enum e_mcp_type {MCP_NONE, MCP_SETUP} t_sdc_mcp_type;

/*
 * Collection of SDC commands
 */
struct s_sdc_commands {

    int num_create_clock_cmds;                              //Number of create_clock commands
    t_sdc_create_clock** create_clock_cmds;                 //Array of create_clock commands [0..num_create_clock_cmds-1]

    int num_set_input_delay_cmds;                           //Number of set_input_delay commands
    t_sdc_set_io_delay** set_input_delay_cmds;              //Array of set_input_delay commands [0..num_set_input_delay_cmds-1]

    int num_set_output_delay_cmds;                          //Number of set_output_delay commands
    t_sdc_set_io_delay** set_output_delay_cmds;             //Array of set_output_delay commands [0..num_set_output_delay_cmds-1]

    int num_set_clock_groups_cmds;                          //Number of set_clock_groups commands
    t_sdc_set_clock_groups** set_clock_groups_cmds;         //Array of set_clock_groups commands [0..num_set_clock_groups_cmds-1]

    int num_set_false_path_cmds;                            //Number of set_false_path commands
    t_sdc_set_false_path** set_false_path_cmds;             //Array of set_false_path commands [0..num_set_false_path_cmds-1]

    int num_set_max_delay_cmds;                             //Number of set_max_delay commands
    t_sdc_set_max_delay** set_max_delay_cmds;               //Array of set_max_delay commands [0..num_set_max_delay_cmds-1]

    int num_set_multicycle_path_cmds;                       //Number of set_multicycle_path commands
    t_sdc_set_multicycle_path** set_multicycle_path_cmds;   //Array of set_multicycle_path commands [0..num_set_multicycle_path_cmds-1]
};

/*
 * Common SDC data structures
 */
struct s_sdc_clock_group {
    int num_clocks; //Number of clocks in this group
    char** clocks;  //Array of clock names [0..num_clocks-1]
                    //May be exact string matches or regexs.
};

struct s_sdc_port_group {
    int num_ports;    //Number of ports in this group
    char** ports;     //Array of ports names [0..num_ports-1]. 
                      //May be exact string matches or regexs.
};

struct s_sdc_string_group {
    int num_strings;    //Number of strings in this group
    char** strings;     //Array of ports names [0..num_strings-1]. 
                        //May be exact string matches or regexs.
};

/*
 * Structures defining different SDC commands
 */
struct s_sdc_create_clock {
    double period;      //Clock period
    char* name;         //Name of the clock
    double rise_time;   //Rise time from waveform definition
    double fall_time;   //Fall time from waveform definition
    char* target;       //String or regex indicating clock source location
    bool is_virtual;    //Identifies this as a virtual (non-netlist) clock
};

struct s_sdc_set_io_delay {
    t_sdc_io_delay_type type;
    char* clock_name;   //Name of the clock this constraint is associated with
    double max_delay;   //The maximum input delay allowed on the target ports
    t_sdc_port_group* target_ports; //The target ports
};

struct s_sdc_set_clock_groups {
    t_sdc_clock_groups_type type;     //The type of clock group relation being specified
    int num_clock_groups;                  //The number of clock groups (must be >= 2)  
    t_sdc_clock_group** clock_groups; //The array of clock groups [0..num_clock_groups-1]
};

struct s_sdc_set_false_path {
    t_sdc_clock_group* from_clocks;  //The source clock group
    t_sdc_clock_group* to_clocks;    //The target clock group
};

struct s_sdc_set_max_delay {
    double max_delay;               //The maximum allowed delay between the from
                                    //and to clocks
    t_sdc_clock_group* from_clocks; //The source clock group
    t_sdc_clock_group* to_clocks;   //The target clock group
};

struct s_sdc_set_multicycle_path {
    t_sdc_mcp_type type;            //The type of the mcp
    double mcp_value;               //The number of cycles specifed
    t_sdc_clock_group* from_clocks; //The source clock group
    t_sdc_clock_group* to_clocks;   //The target clock group
};

/*
 * Parser info
 */
extern int yylineno;
extern char* yytext;

extern t_sdc_commands* g_sdc_commands;

/*
 * Function Declarations
 */
//SDC Command List manipulation
t_sdc_commands* alloc_sdc_commands();
void free_sdc_commands(t_sdc_commands* sdc_commands);

//create_clock
t_sdc_create_clock* alloc_sdc_create_clock();
void free_sdc_create_clock(t_sdc_create_clock* sdc_create_clock);
t_sdc_create_clock* sdc_create_clock_set_period(t_sdc_create_clock* sdc_create_clock, double period);
t_sdc_create_clock* sdc_create_clock_set_name(t_sdc_create_clock* sdc_create_clock, char* name);
t_sdc_create_clock* sdc_create_clock_set_waveform(t_sdc_create_clock* sdc_create_clock, double rise_time, double fall_time);
t_sdc_create_clock* sdc_create_clock_set_target(t_sdc_create_clock* sdc_create_clock, char* target);
t_sdc_commands* add_sdc_create_clock(t_sdc_commands* sdc_commands, t_sdc_create_clock* sdc_create_clock);

//set_input_delay & set_output_delay
t_sdc_set_io_delay* alloc_sdc_set_io_delay(t_sdc_io_delay_type type);
void free_sdc_set_io_delay(t_sdc_set_io_delay* sdc_set_io_delay);
t_sdc_set_io_delay* sdc_set_io_delay_set_clock(t_sdc_set_io_delay* sdc_set_io_delay, char* clock_name);
t_sdc_set_io_delay* sdc_set_io_delay_set_max_delay(t_sdc_set_io_delay* sdc_set_io_delay, double max_delay);
t_sdc_set_io_delay* sdc_set_io_delay_set_ports(t_sdc_set_io_delay* sdc_set_io_delay, t_sdc_port_group* ports);
t_sdc_commands* add_sdc_set_io_delay(t_sdc_commands* sdc_commands, t_sdc_set_io_delay* sdc_set_io_delay);

//set_clock_groups
t_sdc_set_clock_groups* alloc_sdc_set_clock_groups();
void free_sdc_set_clock_groups(t_sdc_set_clock_groups* sdc_set_clock_groups);
t_sdc_set_clock_groups* sdc_set_clock_groups_set_type(t_sdc_set_clock_groups* sdc_set_clock_groups, t_sdc_clock_groups_type type);
t_sdc_set_clock_groups* sdc_set_clock_groups_add_group(t_sdc_set_clock_groups* sdc_set_clock_groups, t_sdc_clock_group* clock_group);
t_sdc_commands* add_sdc_set_clock_groups(t_sdc_commands* sdc_commands, t_sdc_set_clock_groups* sdc_set_clock_groups);

//set_false_path
t_sdc_set_false_path* alloc_sdc_set_false_path();
void free_sdc_set_false_path(t_sdc_set_false_path* sdc_set_false_path);
t_sdc_set_false_path* sdc_set_false_path_add_group(t_sdc_set_false_path* sdc_set_false_path, t_sdc_clock_group* clock_group, t_sdc_clock_group_dir clock_group_dir);
t_sdc_commands* add_sdc_set_false_path(t_sdc_commands* sdc_commands, t_sdc_set_false_path* sdc_set_false_path);

//set_max_delay
t_sdc_set_max_delay* alloc_sdc_set_max_delay();
void free_sdc_set_max_delay(t_sdc_set_max_delay* sdc_set_max_delay);
t_sdc_set_max_delay* sdc_set_max_delay_set_max_delay(t_sdc_set_max_delay* sdc_set_max_delay, double max_delay);
t_sdc_set_max_delay* sdc_set_max_delay_add_group(t_sdc_set_max_delay* sdc_set_max_delay, t_sdc_clock_group* clock_group, t_sdc_clock_group_dir clock_group_dir);
t_sdc_commands* add_sdc_set_max_delay(t_sdc_commands* sdc_commands, t_sdc_set_max_delay* sdc_set_max_delay);

//set_multicycle_path
t_sdc_set_multicycle_path* alloc_sdc_set_multicycle_path();
void free_sdc_set_multicycle_path(t_sdc_set_multicycle_path* sdc_set_multicycle_path);
t_sdc_set_multicycle_path* sdc_set_multicycle_path_set_type(t_sdc_set_multicycle_path* sdc_set_multicycle_path, t_sdc_mcp_type type);
t_sdc_set_multicycle_path* sdc_set_multicycle_path_set_mcp_value(t_sdc_set_multicycle_path* sdc_set_multicycle_path, double mcp_value);
t_sdc_set_multicycle_path* sdc_set_multicycle_path_add_group(t_sdc_set_multicycle_path* sdc_set_multicycle_path, t_sdc_clock_group* clock_group, t_sdc_clock_group_dir clock_group_dir);
t_sdc_commands* add_sdc_set_multicycle_path(t_sdc_commands* sdc_commands, t_sdc_set_multicycle_path* sdc_set_multicycle_path);



//get_ports
t_sdc_port_group* alloc_sdc_get_ports();
t_sdc_port_group* duplicate_port_group(t_sdc_port_group* port_group);
void free_sdc_port_group(t_sdc_port_group* sdc_port_group);
t_sdc_port_group* sdc_get_ports_add_ports(t_sdc_port_group* sdc_port_group, t_sdc_string_group* port_list);
t_sdc_port_group* sdc_get_ports_add_port(t_sdc_port_group* sdc_port_group, char* port_name);

//get_clocks
t_sdc_clock_group* alloc_sdc_get_clocks();
t_sdc_clock_group* duplicate_clock_group(t_sdc_clock_group* clock_group);
void free_sdc_clock_group(t_sdc_clock_group* sdc_clock_group);
t_sdc_clock_group* sdc_get_clocks_add_clocks(t_sdc_clock_group* sdc_clock_group, t_sdc_string_group* clock_list);
t_sdc_clock_group* sdc_get_clocks_add_clock(t_sdc_clock_group* sdc_clock_group, char* clock_name);

//string_group
t_sdc_string_group* alloc_sdc_string_group();
void free_sdc_string_group(t_sdc_string_group* sdc_string_group);
t_sdc_string_group* sdc_string_group_add_string(t_sdc_string_group* sdc_string_group, char* string);

#endif
