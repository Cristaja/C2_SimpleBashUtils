#!/bin/bash

# Color helpers
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

# Paths to our utilities
CAT_EXEC="./s21_cat"
GREP_EXEC="./s21_grep"
REF_CAT="cat"
REF_GREP="grep"

# Temporary files
TEST_DIR=$(mktemp -d)
trap "rm -rf $TEST_DIR" EXIT

# Helper to compare two commands and count differences
compare_cmd() {
    local cmd1="$1"
    local cmd2="$2"
    local desc="$3"
    
    eval "$cmd1" > "$TEST_DIR/out1" 2> "$TEST_DIR/err1"
    eval "$cmd2" > "$TEST_DIR/out2" 2> "$TEST_DIR/err2"
    
    diff -q "$TEST_DIR/out1" "$TEST_DIR/out2" > /dev/null
    local out_diff=$?
    diff -q "$TEST_DIR/err1" "$TEST_DIR/err2" > /dev/null
    local err_diff=$?
    
    if [ $out_diff -eq 0 ] && [ $err_diff -eq 0 ]; then
        echo -e "${GREEN}✓${NC} $desc"
        return 0
    else
        echo -e "${RED}✗${NC} $desc"
        echo "  stdout diff:"
        diff "$TEST_DIR/out1" "$TEST_DIR/out2" | head -n 10
        echo "  stderr diff:"
        diff "$TEST_DIR/err1" "$TEST_DIR/err2" | head -n 10
        return 1
    fi
}

# Create test files
echo "Line one" > "$TEST_DIR/f1"
echo "" >> "$TEST_DIR/f1"
echo "Line three" >> "$TEST_DIR/f1"
echo "  tabbed line" >> "$TEST_DIR/f1"
printf "Line with \x07 bell" > "$TEST_DIR/binary"
echo "apple banana apple" > "$TEST_DIR/words"
echo "Banana Apple" >> "$TEST_DIR/words"
echo "no match here" >> "$TEST_DIR/words"

# ----------------------------------------------------------------------
# CAT tests
# ----------------------------------------------------------------------
echo "===== TESTING s21_cat ====="
FAIL_CAT=0

compare_cmd "$CAT_EXEC $TEST_DIR/f1" "$REF_CAT $TEST_DIR/f1" "cat: no flags" || ((FAIL_CAT++))

compare_cmd "$CAT_EXEC -n $TEST_DIR/f1" "$REF_CAT -n $TEST_DIR/f1" "cat: -n" || ((FAIL_CAT++))

compare_cmd "$CAT_EXEC -b $TEST_DIR/f1" "$REF_CAT -b $TEST_DIR/f1" "cat: -b" || ((FAIL_CAT++))

compare_cmd "$CAT_EXEC -s $TEST_DIR/f1" "$REF_CAT -s $TEST_DIR/f1" "cat: -s" || ((FAIL_CAT++))

compare_cmd "$CAT_EXEC -E $TEST_DIR/f1" "$REF_CAT -E $TEST_DIR/f1" "cat: -E" || ((FAIL_CAT++))

compare_cmd "$CAT_EXEC -T $TEST_DIR/f1" "$REF_CAT -T $TEST_DIR/f1" "cat: -T" || ((FAIL_CAT++))

compare_cmd "$CAT_EXEC -v $TEST_DIR/binary" "$REF_CAT -v $TEST_DIR/binary" "cat: -v (binary)" || ((FAIL_CAT++))

compare_cmd "$CAT_EXEC -ben $TEST_DIR/f1" "$REF_CAT -ben $TEST_DIR/f1" "cat: combined -ben" || ((FAIL_CAT++))

echo "Cat failures: $FAIL_CAT"

# ----------------------------------------------------------------------
# GREP tests
# ----------------------------------------------------------------------
echo "===== TESTING s21_grep ====="
FAIL_GREP=0

compare_cmd "$GREP_EXEC 'apple' $TEST_DIR/words" "$REF_GREP 'apple' $TEST_DIR/words" "grep: basic pattern" || ((FAIL_GREP++))

compare_cmd "$GREP_EXEC -i 'apple' $TEST_DIR/words" "$REF_GREP -i 'apple' $TEST_DIR/words" "grep: -i" || ((FAIL_GREP++))

compare_cmd "$GREP_EXEC -v 'apple' $TEST_DIR/words" "$REF_GREP -v 'apple' $TEST_DIR/words" "grep: -v" || ((FAIL_GREP++))

compare_cmd "$GREP_EXEC -c 'apple' $TEST_DIR/words" "$REF_GREP -c 'apple' $TEST_DIR/words" "grep: -c" || ((FAIL_GREP++))

compare_cmd "$GREP_EXEC -l 'apple' $TEST_DIR/words $TEST_DIR/f1" "$REF_GREP -l 'apple' $TEST_DIR/words $TEST_DIR/f1" "grep: -l" || ((FAIL_GREP++))

compare_cmd "$GREP_EXEC -n 'apple' $TEST_DIR/words" "$REF_GREP -n 'apple' $TEST_DIR/words" "grep: -n" || ((FAIL_GREP++))

compare_cmd "$GREP_EXEC -h 'apple' $TEST_DIR/words $TEST_DIR/f1" "$REF_GREP -h 'apple' $TEST_DIR/words $TEST_DIR/f1" "grep: -h" || ((FAIL_GREP++))

compare_cmd "$GREP_EXEC -s 'apple' nonexistent" "$REF_GREP -s 'apple' nonexistent" "grep: -s (silent)" || ((FAIL_GREP++))

# Multiple patterns (-e)
compare_cmd "$GREP_EXEC -e 'apple' -e 'Banana' $TEST_DIR/words" "$REF_GREP -e 'apple' -e 'Banana' $TEST_DIR/words" "grep: -e multiple" || ((FAIL_GREP++))

# Pattern from file (-f)
echo "apple" > "$TEST_DIR/pat"
echo "Banana" >> "$TEST_DIR/pat"
compare_cmd "$GREP_EXEC -f $TEST_DIR/pat $TEST_DIR/words" "$REF_GREP -f $TEST_DIR/pat $TEST_DIR/words" "grep: -f from file" || ((FAIL_GREP++))

# Only matching (-o) – note that reference grep behaves differently (prints each match on separate line)
compare_cmd "$GREP_EXEC -o 'apple' $TEST_DIR/words" "$REF_GREP -o 'apple' $TEST_DIR/words" "grep: -o (single match)" || ((FAIL_GREP++))

# Combination: -iv (invert + case-insensitive)
compare_cmd "$GREP_EXEC -iv 'apple' $TEST_DIR/words" "$REF_GREP -iv 'apple' $TEST_DIR/words" "grep: -iv combination" || ((FAIL_GREP++))

echo "Grep failures: $FAIL_GREP"

# ----------------------------------------------------------------------
# Final verdict
# ----------------------------------------------------------------------
TOTAL_FAIL=$((FAIL_CAT + FAIL_GREP))
if [ $TOTAL_FAIL -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed. Total failures: $TOTAL_FAIL${NC}"
    exit 1
fi