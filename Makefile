CC = g++
AR = ar

EXE=sdc_parse_test
STATIC_LIB=libsdc.a

WARN_FLAGS = -Wall -Wpointer-arith -Wcast-qual -D__USE_FIXED_PROTOTYPES__ -ansi -pedantic -Wshadow -Wcast-align -D_POSIX_SOURCE -Wno-write-strings

DEBUG_FLAGS = -g -ggdb3 -g3 -O0 -fno-inline

OPT_FLAGS = -O0

SRC_DIR = src
OBJ_DIR = obj

INC_FLAGS = -I$(SRC_DIR) -I$(OBJ_DIR)

main_src = $(SRC_DIR)/main.c
main_obj := $(patsubst %.c, %.o, $(main_src))
main_obj := $(foreach obj, $(main_obj), $(patsubst $(SRC_DIR)/%.o, $(OBJ_DIR)/%.o, $(obj)))

lib_src = $(SRC_DIR)/sdc_common.c $(SRC_DIR)/sdc.c
lib_obj := $(patsubst %.c, %.o, $(lib_src))
lib_obj := $(foreach obj, $(lib_obj), $(patsubst $(SRC_DIR)/%.o, $(OBJ_DIR)/%.o, $(obj)))


LEXER_SRC = $(SRC_DIR)/sdc_parse.l
LEXER_GEN_SRC = $(OBJ_DIR)/sdc_parse.lex.c
LEXER_GEN_OBJ = $(patsubst %.c, %.o, $(LEXER_GEN_SRC))

PARSER_SRC = $(SRC_DIR)/sdc_parse.y
PARSER_GEN_SRC = $(OBJ_DIR)/sdc_parse.tab.c
PARSER_GEN_OBJ = $(patsubst %.c, %.o, $(PARSER_GEN_SRC))

OBJECTS_LIB = $(lib_obj) $(LEXER_GEN_OBJ) $(PARSER_GEN_OBJ)
OBJECTS_EXE = $(main_obj) $(OBJECTS_LIB)

CFLAGS = $(WARN_FLAGS) $(DEBUG_FLAGS) $(OPT_FLAGS) $(INC_FLAGS)

all: $(EXE) $(STATIC_LIB)

$(EXE): $(OBJECTS_EXE)
	@echo "Linking executable: $@"
	@$(CC) $(CFLAGS) -o $@ $(OBJECTS_EXE) -lfl

$(STATIC_LIB): $(OBJECTS_LIB)
	@echo "Linking static libarary: $@"
	@$(AR) rcs $@ $(OBJECTS_LIB)

$(LEXER_GEN_SRC): $(SRC_DIR)/sdc_parse.l $(OBJ_DIR)
	@echo "Generating Lexer $<..."
	@flex -o $@ $<

$(PARSER_GEN_SRC): $(SRC_DIR)/sdc_parse.y $(OBJ_DIR)
	@echo "Generating Parser $<..."
	@bison -d -o $@ $<

$(LEXER_GEN_OBJ): $(LEXER_GEN_SRC) $(PARSER_GEN_SRC)
	@echo "Compiling Lexer $<..."
	@$(CXX) $(CFLAGS) -c $< -o $@

$(PARSER_GEN_OBJ): $(PARSER_GEN_SRC)
	@echo "Compiling Parser $<..."
	@$(CXX) $(CFLAGS) -c $< -o $@

$(lib_obj): $(OBJ_DIR)/%.o:$(SRC_DIR)/%.c $(OBJ_DIR) $(PARSER_GEN_SRC)
	@echo "Compiling Source $<..."
	@$(CXX) $(CFLAGS) -c $< -o $@

$(main_obj): $(OBJ_DIR)/%.o:$(SRC_DIR)/%.c $(OBJ_DIR) $(OBJECTS_LIB)
	@echo "Compiling Main $<..."
	@$(CXX) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@ mkdir -p $@

clean:
	rm -f $(LEXER_GEN_SRC)
	rm -f $(PARSER_GEN_SRC)
	rm -rf $(OBJ_DIR)
	rm -f $(EXE)
	rm -f $(STATIC_LIB)
