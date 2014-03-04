/* simplest version of calculator */
%{
#include <stdio.h>
extern int yylineno;
extern char* yytext;
%}

%union {
    char* strVal;
    double floatVal;
}

/* declare constant */
%token CMD_CREATE_CLOCK
%token CMD_SET_CLOCK_GROUPS
%token CMD_SET_FALSE_PATH
%token CMD_SET_MAX_DELAY
%token CMD_SET_MULTICYCLE_PATH
%token CMD_SET_INPUT_DELAY
%token CMD_GET_PORTS
%token CMD_SET_OUTPUT_DELAY

%token ARG_PERIOD
%token ARG_WAVEFORM
%token ARG_NAME
%token ARG_EXCLUSIVE
%token ARG_GROUP
%token ARG_FROM
%token ARG_TO
%token ARG_SETUP
%token ARG_CLOCK
%token ARG_MAX

/* declare variable tokens */
%token <strVal> BARE_STRING
%token <strVal> BARE_CHAR
%token <floatVal> BARE_NUMBER
%type <strVal> string
%type <floatVal> number


%%
cmdlist: /*empty*/
    | cmdlist cmd;

cmd: cmd_create_clock
    ;

cmd_create_clock: CMD_CREATE_CLOCK ARG_PERIOD number ARG_NAME string { printf("P: create_clock %f %s\n", $3, $5); }
    ;

string: BARE_STRING { $$ = $1; }
    | '"' BARE_STRING '"' { $$ = $2; }
    | '\'' BARE_STRING '\'' { $$ = $2; }
    | '{' BARE_STRING '}' { $$ = $2; }
    ;

number: BARE_NUMBER { $$ = $1; }
    ;

%%

int main(int argc, char **argv) {
    yyparse();
    return 0;
}

int yyerror(char *msg) {
    fprintf(stderr, "%s at line %d: %s\n", msg, yylineno, yytext);
    return 1;
}
