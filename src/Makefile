CC = g++

WARN_FLAGS = -Wall -Wpointer-arith -Wcast-qual -D__USE_FIXED_PROTOTYPES__ -ansi -pedantic -Wshadow -Wcast-align -D_POSIX_SOURCE -Wno-write-strings

DEBUG_FLAGS = -g -ggdb3 -g3 -O0 -fno-inline

OPT_FLAGS = -O0

CFLAGS = $(WARN_FLAGS) $(DEBUG_FLAGS) $(OPT_FLAGS)

sdc_parse: sdc_parse.l sdc_parse.y sdc_common.o
	bison -d -v sdc_parse.y
	flex sdc_parse.l
	$(CC) $(CFLAGS) -o $@ sdc_parse.tab.c lex.yy.c sdc_common.o -lfl

sdc_common.o: sdc_common.h sdc_common.c
	$(CC) $(CFLAGS) -c sdc_common.c
