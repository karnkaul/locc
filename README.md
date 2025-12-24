# Lines of Code Counter v0.5

[![Build status](https://github.com/karnkaul/locc/actions/workflows/ci.yml/badge.svg)](https://github.com/karnkaul/locc/actions/workflows/ci.yml)

`locc` prints a small report summarising the counts of lines of code, given a path.

## Usage

Syntax:

```
locc [OPTIONS...] [PATH(=.)]
```

## Output

Examples of output:

Linux kernel (38 million lines counted)

```
$ time ./locc linux/
+-------------------------------------------------------------------------+
| Category     |  Files |      Lines |       Code |  Comments |     Empty |
---------------------------------------------------------------------------
| C Source     | 36,236 | 25,790,524 | 19,712,859 | 2,324,330 | 3,753,335 |
| C/C++ Header | 27,531 | 10,561,465 |  8,387,779 | 1,357,596 |   816,090 |
| YAML         |  5,329 |    607,444 |    479,939 |    24,166 |   103,339 |
| JSON         |    983 |    585,490 |          0 |         0 |       943 |
| Shell        |  1,012 |    200,532 |    139,031 |    24,734 |    36,767 |
| Rust         |    338 |    135,991 |     89,251 |    35,290 |    11,450 |
| Plain Text   |  1,035 |    117,769 |          0 |         0 |    21,986 |
| Python       |    346 |    101,745 |     78,547 |     6,630 |    16,568 |
| Makefile     |  3,303 |     91,383 |     62,197 |    13,270 |    15,916 |
| XML          |     31 |     24,249 |     21,628 |     1,122 |     1,499 |
| C++ Source   |      4 |      2,251 |      1,836 |        80 |       335 |
| Markdown     |      6 |        634 |        475 |         0 |       159 |
| C++ Header   |      2 |        127 |         63 |        43 |        21 |
| HTML         |      2 |         36 |         30 |         0 |         6 |
| INI          |      2 |         15 |          6 |         5 |         4 |
| Total        | 76,160 | 38,219,655 | 28,973,641 | 3,787,266 | 4,778,418 |
+-------------------------------------------------------------------------+

./locc linux  1.56s user 0.51s system 780% cpu 0.266 total
```

This repo (v0.5)

```
$ ./locc --log-level=info --heuristic=precision --threads=4 --sort-by=category
[I] [locc::cli::App/00] log level set to: Info [20:57:48] [app.cpp:80]
[I] [locc::cli::App/00] locc v0.5.1 [20:57:48] [app.cpp:81]
[I] [locc::detail::(anonymous namespace)::Instance/00] threads: 4, Heuristic: Precision, flags: None [20:57:48] [instance.cpp:38]
[I] [locc::detail::Executor/00] processing path: /home/karnage/cpp/locc [20:57:48] [executor.cpp:22]
[I] [locc::detail::Executor/00] counted 52 files in 2ms [20:57:48] [executor.cpp:101]
+-------------------------------------------------------+
| Category   | Files | Lines |  Code | Comments | Empty |
---------------------------------------------------------
| C++ Header |    24 |   639 |   458 |       74 |   134 |
| C++ Source |    16 |   940 |   787 |       24 |   153 |
| CMake      |     4 |    90 |    65 |        0 |    25 |
| JSON       |     5 |   444 |     0 |        0 |     1 |
| Markdown   |     1 |   118 |    90 |        0 |    28 |
| Shell      |     2 |    36 |    22 |        2 |    12 |
| Total      |    52 | 2,267 | 1,422 |      100 |   353 |
+-------------------------------------------------------+
```

### Options

`locc` supports a few options, as of v0.5.2:

```
$ ./locc --help
lines of code counter
Usage:
  locc [OPTION...] [path(=)] 
  locc <--help|--usage|--version>

OPTIONS
  -t, --threads                desired number of threads
  -h, --heuristic              counting Heuristic (performance/precision)
  -s, --spec-json              path to custom Specification JSON
  -l, --log-level              logging level (error/warn/info/debug)
      --sort-by                sort by (header name)
  -d, --max-recurse-depth      max depth for recursive directory iteration
      --no-code-families       no default Code Families
      --no-text-categories     no default text Categories
      --no-exclude-suffixes    no default exclude suffixes
      --help                   display this help and exit
      --usage                  print usage and exit
      --version                print version text and exit
```

### Custom Specification

The schema for a custom specification JSON is provided [here](./schema.json).

## Building

Use CMake and a preset / generator of your choice.

### Requirements

- Desktop OS (`int main()` entrypoint)
- CMake 3.24+
- C++23 compiler (and stdlib)

## Contributing

Pull requests are welcome.

## External Dependencies

* [klib](https://github.com/karnkaul/klib)
* [djson](https://github.com/karnkaul/djson)

[**Original Repository**](https://github.com/karnkaul/locc)
