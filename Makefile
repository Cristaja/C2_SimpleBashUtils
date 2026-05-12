CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11 -g
LDFLAGS =
RM = rm -f

OBJ_DIR = obj
COMMON_DIR = common
BIN_DIR = .

TARGETS = $(BIN_DIR)/s21_cat $(BIN_DIR)/s21_grep

all: $(TARGETS)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/common:
	mkdir -p $(OBJ_DIR)/common

# Build s21_cat
$(BIN_DIR)/s21_cat: $(OBJ_DIR)/cat.o $(OBJ_DIR)/arg_parser.o $(OBJ_DIR)/file_utils.o | $(OBJ_DIR) $(OBJ_DIR)/common
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Build s21_grep
$(BIN_DIR)/s21_grep: $(OBJ_DIR)/grep.o $(OBJ_DIR)/arg_parser.o $(OBJ_DIR)/file_utils.o | $(OBJ_DIR) $(OBJ_DIR)/common
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile rules
$(OBJ_DIR)/cat.o: cat.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/grep.o: grep.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/arg_parser.o: $(COMMON_DIR)/arg_parser.c $(COMMON_DIR)/arg_parser.h | $(OBJ_DIR)/common
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/file_utils.o: $(COMMON_DIR)/file_utils.c $(COMMON_DIR)/file_utils.h | $(OBJ_DIR)/common
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGETS)
	@echo "Running tests..."
	@if [ -f test.sh ]; then bash test.sh; else echo "No test.sh found"; fi

clean:
	$(RM) -r $(OBJ_DIR)
	$(RM) $(TARGETS)
	$(RM) *.gcda *.gcno *.gcov

gcov_report: CFLAGS += -fprofile-arcs -ftest-coverage
gcov_report: clean test
	@mkdir -p gcov_report
	@if command -v lcov >/dev/null 2>&1 && command -v genhtml >/dev/null 2>&1; then \
	  lcov --capture --directory . --output-file gcov_report/coverage.info --no-external; \
	  genhtml gcov_report/coverage.info --output-directory gcov_report; \
	  echo "Coverage report generated in gcov_report/index.html"; \
	else \
	  echo "lcov/genhtml not installed, skipping coverage report generation"; \
	fi

install: $(TARGETS)
	cp $(BIN_DIR)/s21_cat /usr/local/bin/
	cp $(BIN_DIR)/s21_grep /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/s21_cat /usr/local/bin/s21_grep

dvi:
	@echo "Documentation for s21_cat and s21_grep"
	@echo "See man pages: man cat, man grep"

dist: clean
	mkdir -p ../C2_SimpleBashUtils_dist
	cp -r . ../C2_SimpleBashUtils_dist/
	tar -czf ../C2_SimpleBashUtils_dist.tar.gz -C .. C2_SimpleBashUtils_dist
	rm -rf ../C2_SimpleBashUtils_dist

.PHONY: all clean test gcov_report install uninstall dvi dist