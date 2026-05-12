# Requirements for C2_SimpleBashUtils

## Functional Requirements

### s21_cat
Must support the following flags:
- `-b` (GNU: --number-nonblank): number only non-empty lines
- `-e` implies -v (GNU: -E the same but without -v): display $ at end of lines and show non-printing characters
- `-E` (GNU: --show-ends): display $ at end of lines
- `-n` (GNU: --number): number all output lines
- `-s` (GNU: --squeeze-blank): squeeze multiple adjacent blank lines
- `-t` implies -v (GNU: -T the same but without -v): display tabs as ^I and show non-printing characters
- `-T` (GNU: --show-tabs): display TAB characters as ^I
- `-v` (GNU: --show-nonprinting): display non-printing characters

### s21_grep
Must support the following flags:
- `-e`: pattern from command line (can be used multiple times)
- `-i`: ignore case distinctions
- `-v`: invert match (select non-matching lines)
- `-c`: count of matching lines
- `-l`: print only names of files with matching lines
- `-n`: print line number with output lines
- `-h`: suppress filename prefix on output
- `-s`: suppress error messages about nonexistent or unreadable files
- `-f`: read patterns from file (one per line)
- `-o`: print only the matching part of a line

## Non-Functional Requirements
- Code must compile with `-Wall -Werror -Wextra -std=c11`
- No memory leaks (verified with valgrind)
- Code coverage must be at least 80%
- Code must follow Google C Style Guide
- Must handle stdin input when no files specified or "-" given