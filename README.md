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

 | File         |      LOC |    Total | Comments | Files | Ratio |
 -----------------------------------------------------------------
 | C            | 16088983 | 19879024 |    35616 | 28969 | 0.754 |
 | C header     |  4909329 |  6079780 |   139577 | 21112 | 0.230 |
 | Plain text   |   196016 |   242803 |     4587 |  3230 | 0.009 |
 | Makefile     |    57108 |    66883 |        0 |  2581 | 0.003 |
 | Shell script |    57795 |    82865 |    10413 |   517 | 0.003 |
 | Python       |    26771 |    34364 |     2778 |   113 | 0.001 |
 | C++          |     1993 |     2360 |       23 |     8 | 0.000 |
 | C++ header   |       99 |      127 |        0 |     2 | 0.000 |
 | CSS          |      105 |      138 |        0 |     1 | 0.000 |
 | Total        | 21338199 | 26388344 |   192994 | 56533 | 1.000 |


real    0m0.956s
user    0m3.884s
sys     0m0.660s

$ time ./locc -q linux-master
21336187
real    0m0.936s
user    0m3.640s
sys     0m0.702s
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

### Performance

`locc` is incredibly lightweight when compared to `loc` / `scc` / etc (orders of magnitude smaller), and generally runs faster than them as well, albeit with the least amount of stats / customisation.

Stats on a 12 thread CPU with 32GB RAM, run on the Linux repo:

```
 | Program |    Size |     Execution Time     |
 ----------------------------------------------
 | loc     | 4786KiB | 1001.67ms (+/-73.65ms) |
 | scc     | 4561KiB | 1346.00ms (+/-43.59ms) |
 | locc    |  298KiB |  960.00ms (+/-14.42ms) |
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
