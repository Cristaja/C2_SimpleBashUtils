#!/bin/bash

# Simple test script for s21_cat and s21_grep
# Requires that s21_cat and s21_grep are built

set -e

echo "=== Testing s21_cat ==="

# Create test files
echo "hello world" > test1.txt
echo "hello again" >> test1.txt
echo "" >> test1.txt
echo "hello hello" >> test1.txt
echo "goodbye" >> test1.txt

# Test -b (number non-empty lines)
./s21_cat -b test1.txt > cat_out.txt
cat -b test1.txt > sys_out.txt
diff cat_out.txt sys_out.txt && echo "✓ -b flag works" || echo "✗ -b flag failed"

# Test -n (number all lines)
./s21_cat -n test1.txt > cat_out.txt
cat -n test1.txt > sys_out.txt
diff cat_out.txt sys_out.txt && echo "✓ -n flag works" || echo "✗ -n flag failed"

# Test -s (squeeze empty lines)
echo -e "line1\n\n\nline2" > test2.txt
./s21_cat -s test2.txt > cat_out.txt
cat -s test2.txt > sys_out.txt
diff cat_out.txt sys_out.txt && echo "✓ -s flag works" || echo "✗ -s flag failed"

# Test -e (show ends)
echo "line1" > test3.txt
echo "line2" >> test3.txt
./s21_cat -e test3.txt > cat_out.txt
cat -e test3.txt > sys_out.txt
diff cat_out.txt sys_out.txt && echo "✓ -e flag works" || echo "✗ -e flag failed"

echo ""
echo "=== Testing s21_grep ==="

# Test basic grep
./s21_grep "hello" test1.txt > grep_out.txt
grep "hello" test1.txt > sys_grep.txt
diff grep_out.txt sys_grep.txt && echo "✓ basic pattern works" || echo "✗ basic pattern failed"

# Test -i (ignore case)
./s21_grep -i "HELLO" test1.txt > grep_out.txt
grep -i "HELLO" test1.txt > sys_grep.txt
diff grep_out.txt sys_grep.txt && echo "✓ -i flag works" || echo "✗ -i flag failed"

# Test -v (invert match)
./s21_grep -v "hello" test1.txt > grep_out.txt
grep -v "hello" test1.txt > sys_grep.txt
diff grep_out.txt sys_grep.txt && echo "✓ -v flag works" || echo "✗ -v flag failed"

# Test -c (count)
./s21_grep -c "hello" test1.txt > grep_out.txt
grep -c "hello" test1.txt > sys_grep.txt
diff grep_out.txt sys_grep.txt && echo "✓ -c flag works" || echo "✗ -c flag failed"

# Test -l (files with match)
./s21_grep -l "hello" test1.txt > grep_out.txt
grep -l "hello" test1.txt > sys_grep.txt
diff grep_out.txt sys_grep.txt && echo "✓ -l flag works" || echo "✗ -l flag failed"

# Cleanup
rm -f test1.txt test2.txt test3.txt cat_out.txt sys_out.txt grep_out.txt sys_grep.txt

echo ""
echo "=== All tests completed ==="