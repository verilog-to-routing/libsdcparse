CC = g++

EXE=sdc_parse_test
STATIC_LIB=libsdc.a

WARN_FLAGS = -Wall -Wpointer-arith -Wcast-qual -D__USE_FIXED_PROTOTYPES__ -ansi -pedantic -Wshadow -Wcast-align -D_POSIX_SOURCE -Wno-write-strings

DEBUG_FLAGS = -g -ggdb3 -g3 -O0 -fno-inline

OPT_FLAGS = -O0

SRC_DIR = src
OBJ_DIR = obj

DIRS = $(SRC_DIR)
INC_FLAGS = $(foreach inc_dir, $(DIRS), $(patsubst %, -I%, $(inc_dir)))

src_c = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))
objects_c := $(patsubst %.c, %.o, $(src_c))
objects_c := $(foreach obj, $(objects_c), $(patsubst $(SRC_DIR)/%.o, $(OBJ_DIR)/%.o, $(obj)))



LEXER_SRC = $(SRC_DIR)/sdc_parse.l
LEXER_GEN_SRC = $(OBJ_DIR)/sdc_parse.lex.c

PARSER_SRC = $(SRC_DIR)/sdc_parse.y
PARSER_GEN_SRC = $(OBJ_DIR)/sdc_parse.tab.c

CFLAGS = $(WARN_FLAGS) $(DEBUG_FLAGS) $(OPT_FLAGS) $(INC_FLAGS)

all: $(EXE) $(STATIC_LIB)

$(EXE): $(PARSER_GEN_SRC) $(LEXER_GEN_SRC) sdc_common.o
	$(CC) $(CFLAGS) -o $@ $< -lfl

$(PARSER_GEN_SRC): $(SRC_DIR)/sdc_parse.y $(OBJ_DIR)
	bison -d -o $@ $<

$(LEXER_GEN_SRC): $(SRC_DIR)/sdc_parse.l $(OBJ_DIR)
	flex -o $@ $<

$(objects_c): $(OBJ_DIR)/%.o:$(SRC_DIR)/%.c 
	@echo "Compiling $<..."
	@$(CXX) -MMD -MP $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@ mkdir -p $@

clean:
	rm -f $(LEXER_GEN_SRC)
	rm -f $(PARSER_GEN_SRC)
	rm -rf $(OBJ_DIR)
