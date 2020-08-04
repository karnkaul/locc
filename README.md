## Lines of Code Counter

[![Build status](https://ci.appveyor.com/api/projects/status/056xjwjht5fwxf4n?svg=true)](https://ci.appveyor.com/project/karnkaul/locc)

This is a hobby application designed to count lines of code given a list of files or a directory.

### Usage

Syntax:

```
locc [options] <file0> [file1 file2...] (1)
locc [options] <directory> (2)

(1) explicit mode: provide an explicit list of files to count (will count all files passed)
(2) implicit mode: provide a root directory to recursively traverse (will only count matching extensions)
```

### Output

Examples of output (may be out of date until release):

Linux kernel (20 million LOC)

```
$ time locc linux-master

 | Extension |      LOC |    Total | Comments | Files | Ratio |
 --------------------------------------------------------------
 | .c        | 15994571 | 19768868 |    35390 | 28751 | 0.755 |
 | .h        |  4899262 |  6066731 |   139553 | 21033 | 0.231 |
 | .txt      |   194380 |   240694 |     4561 |  3220 | 0.009 |
 | .sh       |    55046 |    78839 |    10064 |   495 | 0.003 |
 | .py       |    25662 |    32936 |     2716 |   106 | 0.001 |
 | .cc       |     1681 |     1972 |       17 |     1 | 0.000 |
 | .cpp      |      251 |      314 |        2 |     3 | 0.000 |
 | .css      |      105 |      138 |        0 |     1 | 0.000 |
 | .inl      |       99 |      127 |        0 |     2 | 0.000 |
 | Total     | 21171057 | 26190619 |   192303 | 53612 | 1.000 |


real    0m0.747s
user    0m3.521s
sys     0m0.592s

$ time locc -q linux-master
21171057
real    0m0.724s
user    0m3.580s
sys     0m0.564s
```

This repo (at some point in the past)

```
locc -v .
 | File                          | LOC | Comments | Total |
 ----------------------------------------------------------
 | ./src/file_list_generator.hpp |   7 |        0 |     8 |
 | ./CMakeLists.txt              |  25 |        0 |    30 |
 | ./src/line_counter.hpp        |   7 |        0 |     8 |
 | ./src/args_parser.hpp         |  12 |        0 |    14 |
 | ./src/args_parser.cpp         |  71 |        0 |    73 |
 | ./src/async_queue.hpp         |  65 |        0 |    72 |
 | ./src/file_list_generator.cpp |  99 |        0 |   101 |
 | ./src/table_formatter.hpp     | 139 |        0 |   156 |
 | ./src/table_formatter.cpp     | 109 |        0 |   118 |
 | ./src/main.cpp                | 224 |        0 |   232 |
 | ./src/common.hpp              | 269 |        0 |   317 |
 | ./src/line_counter.cpp        | 223 |        0 |   234 |


 | Extension |  LOC | Total | Comments | Files | Ratio |
 -------------------------------------------------------
 | .cpp      |  726 |   758 |        0 |     5 | 0.581 |
 | .hpp      |  499 |   575 |        0 |     6 | 0.399 |
 | .txt      |   25 |    30 |        0 |     1 | 0.020 |
 | Total     | 1250 |  1363 |        0 |    12 | 1.000 |
```

### Options

`locc` supports a few command line parameters:

- `--skip-substr=[substr0,substr1,...]`: Add path substring pattern to ignore
- `--skip-ext=[ext0,ext1,...]`: Add specific extensions to ignore
- `[-o|--one-thread]`: Run everything on the main thread
- `[-b|--blanks]`: Include blank lines as lines of code
- `[-q|--quiet]`: Print nothing but total lines of code
- `[-v|--verbose]`: Lots of output
- `[-d|--debug]`: Random debug output
- `[-h|--help]`: Display help

> _More switches and a data driven approach are in development._

### Building

Use CMake and a generator of your choice.
Requirements:

- C++17 compiler (and stdlib)

### Contributing

WIP
