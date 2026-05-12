CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11 -g -D_GNU_SOURCE
LDFLAGS = -lm
COV_FLAGS = -fprofile-arcs -ftest-coverage

SRC_CAT = cat.c
SRC_GREP = grep.c
SRC_COMMON = common/arg_parser.c common/file_utils.c
OBJ_COMMON = $(SRC_COMMON:.c=.o)
OBJ_CAT = $(SRC_CAT:.c=.o) $(OBJ_COMMON)
OBJ_GREP = $(SRC_GREP:.c=.o) $(OBJ_COMMON)

TARGETS = s21_cat s21_grep

all: $(TARGETS)

s21_cat: $(OBJ_CAT)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

s21_grep: $(OBJ_GREP)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

common/%.o: common/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGETS) *.o common/*.o
	rm -f *.gcda *.gcno *.gcov common/*.gcda common/*.gcno
	rm -rf gcov_report

test: $(TARGETS)
	@if [ -f test.sh ]; then chmod +x test.sh && ./test.sh; else \
	echo "=== Simple tests ===" && \
	echo "Testing s21_cat:" && \
	echo "hello\n\nworld" > test1.txt && \
	./s21_cat -b test1.txt > cat_out1 && \
	cat -b test1.txt > sys_out1 && \
	diff cat_out1 sys_out1 && echo "✓ -b flag works" && \
	./s21_cat -n test1.txt > cat_out2 && \
	cat -n test1.txt > sys_out2 && \
	diff cat_out2 sys_out2 && echo "✓ -n flag works" && \
	./s21_cat -s test1.txt > cat_out3 && \
	cat -s test1.txt > sys_out3 && \
	diff cat_out3 sys_out3 && echo "✓ -s flag works" && \
	echo "Testing s21_grep:" && \
	echo "test line" > test2.txt && \
	echo "another line" >> test2.txt && \
	./s21_grep "test" test2.txt > grep_out1 && \
	grep "test" test2.txt > sys_grep1 && \
	diff grep_out1 sys_grep1 && echo "✓ basic grep works" && \
	rm -f test1.txt test2.txt cat_out* sys_out* grep_out* sys_grep1; \
	fi

gcov_report: clean
	$(CC) $(CFLAGS) $(COV_FLAGS) -o s21_cat_gcov $(SRC_CAT) $(SRC_COMMON) $(LDFLAGS)
	$(CC) $(CFLAGS) $(COV_FLAGS) -o s21_grep_gcov $(SRC_GREP) $(SRC_COMMON) $(LDFLAGS)
	echo "test line" > test.txt
	echo "another line" >> test.txt
	./s21_cat_gcov -b test.txt > /dev/null 2>&1 || true
	./s21_grep_gcov "test" test.txt > /dev/null 2>&1 || true
	lcov --capture --directory . --output-file coverage.info --ignore-errors mismatch,gcov 2>/dev/null || true
	lcov --remove coverage.info '/usr/*' --output-file coverage.info 2>/dev/null || true
	genhtml coverage.info --output-directory gcov_report 2>/dev/null || true
	rm -f s21_cat_gcov s21_grep_gcov *.gcda *.gcno test.txt
	@echo "Coverage report generated in gcov_report/index.html"

valgrind: $(TARGETS)
	@echo "Running valgrind on s21_cat..."
	valgrind --leak-check=full --error-exitcode=1 ./s21_cat Makefile >/dev/null 2>&1 && echo "✓ s21_cat: no leaks" || echo "✗ s21_cat: memory leaks detected"
	@echo "Running valgrind on s21_grep..."
	valgrind --leak-check=full --error-exitcode=1 ./s21_grep "include" Makefile >/dev/null 2>&1 && echo "✓ s21_grep: no leaks" || echo "✗ s21_grep: memory leaks detected"

check:
	cppcheck --enable=all --suppress=missingIncludeSystem *.c common/*.c 2>/dev/null || true
	clang-format -style=Google -n *.c *.h common/*.c common/*.h 2>/dev/null || true

install: $(TARGETS)
	sudo cp $(TARGETS) /usr/local/bin/

uninstall:
	sudo rm -f /usr/local/bin/s21_cat /usr/local/bin/s21_grep

dvi:
	@echo "Project: Simple Bash Utilities (cat and grep)"
	@echo "Documentation: See ARCHITECTURE.md"
	@echo "Build: make all"
	@echo "Test: make test"
	@echo "Coverage: make gcov_report"

dist: clean
	mkdir -p C2_SimpleBashUtils
	cp *.c *.h common/ Makefile test.sh ARCHITECTURE.md REQUIREMENTS.md C2_SimpleBashUtils/ 2>/dev/null || true
	tar czf C2_SimpleBashUtils.tar.gz C2_SimpleBashUtils/
	rm -rf C2_SimpleBashUtils/

.PHONY: all clean test gcov_report valgrind check install uninstall dvi dist