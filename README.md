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
$ time ./locc linux-master

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


real    0m0.699s
user    0m3.462s
sys     0m0.558s

$ time ./locc -q linux-master
21171057
real    0m0.690s
user    0m3.431s
sys     0m0.583s
```

This repo (at some point in the past)

```
$ ./locc -v -e=.md .

 | File                                         | LOC | Total | Comments |
 -------------------------------------------------------------------------
 | ./CMakeLists.txt                             |  49 |    56 |        0 |
 | ./README.md                                  |  84 |   112 |        0 |
 | ./src/app/common.hpp                         | 102 |   119 |        0 |
 | ./src/app/config.hpp                         |  65 |    74 |        0 |
 | ./src/app/file_list_generator.cpp            |  99 |   101 |        0 |
 | ./src/app/file_list_generator.hpp            |  10 |    13 |        0 |
 | ./src/app/line_counter.cpp                   | 228 |   240 |        0 |
 | ./src/app/line_counter.hpp                   |  10 |    13 |        0 |
 | ./src/kt/args_parser/args_parser.hpp         | 112 |   150 |       29 |
 | ./src/kt/async_queue/async_queue.hpp         | 100 |   162 |       49 |
 | ./src/main.cpp                               | 101 |   105 |        0 |
 | ./src/ui/table_formatter/table_formatter.cpp |  95 |   103 |        0 |
 | ./src/ui/table_formatter/table_formatter.hpp | 116 |   133 |        0 |
 | ./src/ui/ui.cpp                              | 136 |   142 |        0 |
 | ./src/ui/ui.hpp                              |  99 |   121 |        0 |


 | Extension |  LOC | Total | Comments | Files | Ratio |
 -------------------------------------------------------
 | .cpp      |  659 |   691 |        0 |     5 | 0.469 |
 | .hpp      |  614 |   785 |       78 |     8 | 0.437 |
 | .md       |   84 |   112 |        0 |     1 | 0.060 |
 | .txt      |   49 |    56 |        0 |     1 | 0.035 |
 | Total     | 1406 |  1644 |       78 |    15 | 1.000 |

```

### Options

`locc` supports a few command line parameters:

- `--skip-substr=<substr0>[,substr1,...]`: Add path substring pattern to ignore
- `-e=` / `--extensions=<.ext0>[,.ext1,...]`: Additional extensions to count in implicit mode
- `-o` / `--one-thread`: Run everything on the main thread
- `-b` / `--blanks`: Include blank lines as lines of code
- `-q` / `--quiet`: Print nothing but total lines of code
- `-v` / `--verbose`: Lots of output
- `-d` / `--debug`: Misc debug output
- `-h` / `--help`: Print help
- `--version`: Print version

> _More switches and a data driven approach are in development._

### Building

Use CMake and a generator of your choice.
Requirements:

- C++17 compiler (and stdlib)

### Contributing

WIP
