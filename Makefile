CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11 -g
LDFLAGS =
RM = rm -f

# Directories
SRC_DIR = .
COMMON_DIR = common
OBJ_DIR = obj
BIN_DIR = .

# Source files for each utility
CAT_SRCS = $(SRC_DIR)/cat.c $(COMMON_DIR)/arg_parser.c $(COMMON_DIR)/file_utils.c
GREP_SRCS = $(SRC_DIR)/grep.c $(COMMON_DIR)/arg_parser.c $(COMMON_DIR)/file_utils.c

# Object files
CAT_OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(CAT_SRCS)))
GREP_OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(GREP_SRCS)))

# Targets
TARGETS = $(BIN_DIR)/s21_cat $(BIN_DIR)/s21_grep

all: $(TARGETS)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Build s21_cat
$(BIN_DIR)/s21_cat: $(OBJ_DIR)/cat.o $(OBJ_DIR)/arg_parser.o $(OBJ_DIR)/file_utils.o | $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Build s21_grep
$(BIN_DIR)/s21_grep: $(OBJ_DIR)/grep.o $(OBJ_DIR)/arg_parser.o $(OBJ_DIR)/file_utils.o | $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile rules
$(OBJ_DIR)/cat.o: $(SRC_DIR)/cat.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/grep.o: $(SRC_DIR)/grep.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/arg_parser.o: $(COMMON_DIR)/arg_parser.c $(COMMON_DIR)/arg_parser.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/file_utils.o: $(COMMON_DIR)/file_utils.c $(COMMON_DIR)/file_utils.h
	$(CC) $(CFLAGS) -c $< -o $@

# Test target
test: $(TARGETS)
	@echo "Running tests..."
	@bash test.sh || (echo "Tests failed"; exit 1)

# Clean target
clean:
	$(RM) -r $(OBJ_DIR)
	$(RM) $(TARGETS)
	$(RM) *.gcda *.gcno *.gcov

# Gcov report
gcov_report: CFLAGS += -fprofile-arcs -ftest-coverage
gcov_report: clean test
	@mkdir -p gcov_report
	@lcov --capture --directory . --output-file gcov_report/coverage.info --no-external
	@genhtml gcov_report/coverage.info --output-directory gcov_report
	@echo "Coverage report generated in gcov_report/index.html"

.PHONY: all clean test gcov_report