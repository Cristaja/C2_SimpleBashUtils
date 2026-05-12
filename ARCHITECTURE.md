# Архитектура проекта

## Структура каталогов

```
src/
├── cat/
│   ├── s21_cat.c          # основная логика cat
│   ├── cat_processor.c    # обработка флагов и вывод
│   ├── cat_processor.h
│   └── Makefile           # цели для cat
├── grep/
│   ├── s21_grep.c         # основная логика grep
│   ├── grep_processor.c   # парсинг шаблонов, поиск
│   ├── grep_processor.h
│   └── Makefile
├── common/
│   ├── file_utils.c       # общие функции работы с файлами
│   ├── file_utils.h
│   ├── arg_parser.c       # универсальный парсер аргументов
│   └── arg_parser.h
└── tests/
    ├── test_cat.sh
    ├── test_grep.sh
    └── test_common.sh
```

## Модули и интерфейсы

### `common/arg_parser`
- Разбор аргументов командной строки, сбор флагов и списка файлов.
- Для cat: структура `cat_flags` с полями `b, e, E, n, s, t, T, v`.
- Для grep: структура `grep_flags` с полями `e, i, v, c, l, n` (и опционально `h, s, f, o`).
- Функции: `parse_cat_args()`, `parse_grep_args()`.

### `common/file_utils`
- Открытие файла, чтение построчно, обработка ошибок (нет файла / нет прав).
- Функции: `file_open_safe()`, `file_read_line()`, `file_close()`.
- При необходимости – поддержка чтения из `stdin`.

### `cat/cat_processor`
- Логика применения флагов к строкам:
  - `process_cat_file()` – принимает флаги и файл, выводит результат.
  - Внутренние функции: `number_lines()`, `squeeze_blank()`, `print_nonprintable()` и т.д.
- Обработка флага `-e` / `-E` (добавление `$`).

### `grep/grep_processor`
- Компиляция регулярных выражений (через `regcomp()`).
- Управление несколькими шаблонами (список строк).
- Основные функции:
  - `compile_patterns()` – компиляция всех шаблонов.
  - `grep_file()` – поиск в одном файле с учётом флагов.
  - Поддержка `-c` (count), `-l` (files with match), `-n` (line numbers), `-v` (invert), `-i` (case insens.).
- Для бонусов: `-f` (читать шаблоны из файла), `-o` (выводить только совпадения).

## Система сборки (Makefile)

### Корневой Makefile (в `src/`) – не обязателен, но удобен:
```makefile
all: s21_cat s21_grep

s21_cat:
	make -C cat

s21_grep:
	make -C grep

clean:
	make -C cat clean
	make -C grep clean
	rm -f tests/*.log

test:
	./tests/test_cat.sh
	./tests/test_grep.sh
```

### Makefile для `cat` (`src/cat/Makefile`):
- Цели: `s21_cat`, `clean`, `rebuild`, `test` (если локально).
- Компиляция: `gcc -Wall -Werror -Wextra -std=c11 -I../common s21_cat.c cat_processor.c ../common/file_utils.c ../common/arg_parser.c -o s21_cat`
- Переменные: `CC = gcc`, `CFLAGS`, `LDFLAGS`.

### Makefile для `grep` (`src/grep/Makefile`):
- Аналогично, с добавлением флага `-D_GNU_SOURCE` для поддержки расширенных regex.
- Линковка с `-l` не требуется (regex в libc).

## Компилятор и флаги
- Обязательные флаги: `-Wall -Werror -Wextra -std=c11`.
- Дополнительно: `-pedantic`, `-D_POSIX_C_SOURCE=200809L`.
- Для отладки: `-g -O0`.

## Кодирование и стиль
- Имена переменных и функций: `snake_case`.
- Отступы: 2 пробела (согласно Google Style).
- Максимальная длина строки: 80 символов.
- Комментарии для публичных функций в `.h` файлах.

## Тестирование
- Скрипты на bash (или на C с `assert`) сравнивают вывод с оригинальными утилитами.
- Тестовые файлы: `tests/test_cat/` с текстовыми файлами разных размеров, включая спецсимволы.
- Для каждого флага и комбинации флагов запускается сравнение.
- Ожидается 100% покрытие всех указанных флагов (базовых и бонусных).