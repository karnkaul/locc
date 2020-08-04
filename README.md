## Lines of Code Counter

[![Build status](https://ci.appveyor.com/api/projects/status/056xjwjht5fwxf4n?svg=true)](https://ci.appveyor.com/project/karnkaul/locc)

This is a hobby application designed to count lines of code given a list of files or a directory.

### Usage

Syntax:

```
loc [options] <file0> [file1 file2...] (1)
loc [options] <directory> (2)

(1) explicit mode: provide an explicit list of files to count (will count all files passed)
(2) implicit mode: provide a root directory to recursively traverse (will only count matching extensions)
```

### Output

Examples of output (may be out of date until release):

Linux kernel (20 million LOC)

```
locc linux-master

 | Extension |      LOC |    Total | Comments | Ratio |
 ------------------------------------------------------
 | .c        | 15994571 | 19768868 |    35390 |     0 |
 | .cc       |     1681 |     1972 |       17 |     0 |
 | .cpp      |      251 |      314 |        2 |     0 |
 | .h        |  4899262 |  6066731 |   139553 |     0 |
 | .inl      |       99 |      127 |        0 |     0 |
 | Total     | 20895864 | 25838012 |   174962 |     0 |


real    0m0.680s
user    0m3.423s
sys     0m0.530s
```

This repo (at some point in the past)

```
locc -dv .
  -- tracking ./src/line_counter.hpp
  -- tracking ./src/args_parser.hpp
  -- tracking ./src/async_queue.hpp
  -- tracking ./src/args_parser.cpp
  -- tracking ./src/main.cpp
  -- tracking ./src/file_list_generator.cpp
  -- tracking ./src/table_formatter.cpp
  -- tracking ./src/file_list_generator.hpp
  -- tracking ./src/line_counter.cpp
  -- tracking ./src/common.hpp
  -- tracking ./src/table_formatter.hpp

  -- flags: verbose debug
  -- mode: implicit

  -- parsing 11 files
  -- launching 10 worker threads
  -- worker threads completed


 |  File                         | LOC | Comments | Total |
 ----------------------------------------------------------
 | ./src/line_counter.hpp        |   7 |        0 |     8 |
 | ./src/file_list_generator.hpp |   7 |        0 |     8 |
 | ./src/args_parser.hpp         |  12 |        0 |    14 |
 | ./src/args_parser.cpp         |  71 |        0 |    73 |
 | ./src/async_queue.hpp         |  65 |        0 |    72 |
 | ./src/table_formatter.cpp     |  90 |        0 |    99 |
 | ./src/file_list_generator.cpp |  99 |        0 |   101 |
 | ./src/table_formatter.hpp     | 138 |        0 |   155 |
 | ./src/main.cpp                | 223 |        0 |   231 |
 | ./src/line_counter.cpp        | 219 |        0 |   230 |
 | ./src/common.hpp              | 247 |        0 |   292 |


 | Extension |  LOC | Total | Comments | Ratio |
 -----------------------------------------------
 | .cpp      |  702 |   734 |        0 |     0 |
 | .hpp      |  476 |   549 |        0 |     0 |
 | Total     | 1178 |  1283 |        0 |     0 |

```

> _Ratio is WIP..._

### Options

`locc` supports a few command line parameters:

- \*`[-i|--ignore]=[line|block-start,block-end]`: Replace ignore delimiters (`//` and `/*,*/` by default)
- \*`--skip-substr=[...]`: Add path substring pattern to ignore
- \*`--skip-ext=[...]`: Add specific extensions to ignore
- `[-o|--one-thread]`: Run everything on the main thread
- `[-b|--blanks]`: Include blank lines as lines of code
- `[-q|--quiet]`: Print nothing but total lines of code
- `[-v|--verbose]`: Lots of output
- `[-d|--debug]`: Random debug output
- `[-h|--help]`: Display help

> _\*Supports multiple instances of the same argument_

More switches and a data driven approach are in development.

### Building

Use CMake and a generator of your choice.
Requirements:

- C++17 compiler (and stdlib)

### Contributing

WIP
