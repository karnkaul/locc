## Lines of Code Counter

[![Build status](https://ci.appveyor.com/api/projects/status/056xjwjht5fwxf4n?svg=true)](https://ci.appveyor.com/project/karnkaul/locc)

`locc` prints a small report summarising the counts of lines of code, given a root directory.

### Usage

Syntax:

```
locc [options] <directory>
```

### Output

Examples of output (may be out of date until release):

Linux kernel (20 million LOC)

```
$ time locc linux-master

 | File          |     Code |    Total | Comments | Files | Ratio |
 ------------------------------------------------------------------
 | C             | 16088983 | 19879024 |    35616 | 28969 | 0.748 |
 | C header      |  4909329 |  6079780 |   139577 | 21112 | 0.228 |
 | Plain Text    |   200603 |   242803 |        0 |  3230 | 0.009 |
 | JSON          |   175200 |   175200 |        0 |   328 | 0.008 |
 | Shell script  |    57795 |    82865 |    10413 |   517 | 0.003 |
 | Makefile      |    57108 |    66883 |        0 |  2581 | 0.003 |
 | Python        |    26771 |    34364 |     2778 |   113 | 0.001 |
 | C++           |     1993 |     2360 |       23 |     8 | 0.000 |
 | gitignore     |     1693 |     1716 |        0 |   254 | 0.000 |
 | CSV           |      209 |      210 |        0 |     6 | 0.000 |
 | CSS           |      105 |      138 |        0 |     1 | 0.000 |
 | C++ header    |       99 |      127 |        0 |     2 | 0.000 |
 | INI           |        7 |        8 |        0 |     1 | 0.000 |
 | gitattributes |        7 |        7 |        0 |     2 | 0.000 |
 | Total         | 21519902 | 26565485 |   188407 | 57124 | 1.000 |


real    0m0.729s
user    0m4.091s
sys     0m0.827s

```

This repo (v0.1)

```
$ ./locc --verbose -e=.md .

 | File                                         | LOC | Total | Comments |
 -------------------------------------------------------------------------
 | ./.appveyor.yml                              |  58 |    60 |        0 |
 | ./.gitattributes                             |   4 |     5 |        0 |
 | ./.gitignore                                 |  10 |    10 |        0 |
 | ./.gitmodules                                |  13 |    13 |        0 |
 | ./CMakeLists.txt                             |  61 |    72 |        0 |
 | ./CMakePresets.json                          |  45 |    45 |        0 |
 | ./README.md                                  |  95 |   126 |        0 |
 | ./compile_commands.json                      |  28 |    28 |        0 |
 | ./libs/clap/CMakeLists.txt                   |  13 |    18 |        0 |
 | ./libs/clap/README.md                        | 101 |   145 |        0 |
 | ./libs/clap/example/CMakeLists.txt           |   7 |     9 |        0 |
 | ./libs/clap/example/clapp.cpp                |  75 |    88 |        5 |
 | ./libs/clap/include/clap/interpreter.hpp     | 356 |   486 |       85 |
 | ./libs/clap/include/clap/parser.hpp          | 113 |   161 |       34 |
 | ./locc_settings.json                         |  81 |    81 |        0 |
 | ./out/db/cmake_install.cmake                 |  51 |    61 |        0 |
 | ./out/db/compile_commands.json               |  28 |    28 |        0 |
 | ./out/db/libs/clap/cmake_install.cmake       |  38 |    45 |        0 |
 | ./out/db/src/build_version.hpp               |  10 |    12 |        0 |
 | ./out/rl/cmake_install.cmake                 |  51 |    61 |        0 |
 | ./out/rl/libs/clap/cmake_install.cmake       |  38 |    45 |        0 |
 | ./out/rl/src/build_version.hpp               |  10 |    12 |        0 |
 | ./src/app/common.hpp                         |  81 |    97 |        0 |
 | ./src/app/config.cpp                         |  76 |    80 |        0 |
 | ./src/app/config.hpp                         |  76 |    88 |        2 |
 | ./src/app/line_counter.cpp                   | 144 |   155 |        0 |
 | ./src/app/line_counter.hpp                   |  24 |    31 |        0 |
 | ./src/kt/async_queue/async_queue.hpp         | 103 |   169 |       52 |
 | ./src/kt/async_queue/lockable.hpp            |  13 |    24 |        8 |
 | ./src/kt/async_queue/locker.hpp              |  53 |    86 |       16 |
 | ./src/kt/kthread/kthread.hpp                 |  52 |    78 |       18 |
 | ./src/kt/str_format/str_format.hpp           |  27 |    34 |        2 |
 | ./src/main.cpp                               |  51 |    53 |        0 |
 | ./src/ui/table_formatter/table_formatter.cpp |  83 |    91 |        0 |
 | ./src/ui/table_formatter/table_formatter.hpp | 109 |   127 |        0 |
 | ./src/ui/ui.cpp                              | 147 |   153 |        0 |
 | ./src/ui/ui.hpp                              |  51 |    62 |        0 |


 | File          | Code | Total | Comments | Files | Ratio |
 -----------------------------------------------------------
 | C++ header    | 1078 |  1467 |      217 |    14 | 0.454 |
 | C++           |  576 |   620 |        5 |     6 | 0.242 |
 | CMake script  |  259 |   311 |        0 |     7 | 0.109 |
 | Markdown      |  196 |   271 |        0 |     2 | 0.082 |
 | JSON          |  182 |   182 |        0 |     4 | 0.077 |
 | YAML          |   58 |    60 |        0 |     1 | 0.024 |
 | .gitmodules   |   13 |    13 |        0 |     1 | 0.005 |
 | gitignore     |   10 |    10 |        0 |     1 | 0.004 |
 | gitattributes |    4 |     5 |        0 |     1 | 0.002 |
 | Total         | 2376 |  2939 |      222 |    37 | 1.000 |
```

### Performance

`locc` is incredibly lightweight when compared to `loc` / `scc` / etc (orders of magnitude smaller), and generally runs faster than them as well, albeit with the least amount of stats / customisation.

Stats on a 12 thread CPU with 32GB RAM, run on the Linux repo:

```
 | Program |    Size |     Execution Time     |
 ----------------------------------------------
 | loc     | 4786KiB |  758.000ms (+/-19.7ms) |
 | scc     | 4561KiB | 1135.667ms (+/-6.7ms)  |
 | locc    |  298KiB |  724.136ms (+/-4.7ms)  |
```

### Options

`locc` supports a few command line parameters:

- `--skip-substr=<substr0>[,substr1,...]`: Add path substring pattern to ignore
- `-e=` / `--extensions=<.ext0>[,.ext1,...]`: Additional extensions to count in implicit mode
- `--sort-by=<column_name>`: Sort results by column (lowercase, `code` by default)
- `-o` / `--one-thread`: Run everything on the main thread
- `-b` / `--blanks`: Include blank lines as lines of code
- `-q` / `--quiet`: Print nothing but total lines of code
- `--verbose`: Lots of output
- `-d` / `--debug`: Misc debug output
- `-h` / `--help`: Print help
- `--version`: Print version

### Building

Use CMake and a generator of your choice.
Requirements:

- Windows / Linux (any architecture)
- C++20 compiler (and stdlib)

### Contributing

Pull/merge requests are welcome.

[**Original Repository**](https://github.com/karnkaul/locc)
