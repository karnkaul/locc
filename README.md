# Lines of Code Counter

[![Build status](https://github.com/karnkaul/locc/actions/workflows/ci.yml/badge.svg)](https://github.com/karnkaul/locc/actions/workflows/ci.yml)

`locc` prints a small report summarising the counts of lines of code, given a path.

## Usage

Syntax:

```
locc [OPTIONS...] <path>
```

## Output

Examples of output:

Linux kernel (36 million lines counted)

```
$ time ./locc-cli linux/

[====================] Completed (72352/72352)

---------------------------------------------------------------------------
| File Type    |  Files |      Lines |       Code |  Comments |     Empty |
---------------------------------------------------------------------------
| C            | 34,679 | 24,641,580 | 18,335,424 | 2,752,545 | 3,553,611 |
| C/C++ header | 26,396 | 10,216,011 |  7,944,696 | 1,508,356 |   762,959 |
| JSON         |    867 |    537,555 |    537,553 |         0 |         2 |
| YAML         |  4,436 |    497,589 |    395,624 |    20,318 |    81,647 |
| Shell script |    923 |    181,514 |    126,030 |    22,945 |    32,539 |
| Plain Text   |  1,503 |    136,506 |          0 |         0 |    24,586 |
| Makefile     |  3,156 |     84,147 |     59,038 |    12,933 |    12,176 |
| Python       |    264 |     79,032 |     62,136 |     4,910 |    11,986 |
| XML          |     24 |     22,163 |     19,282 |     1,663 |     1,218 |
| Rust         |     82 |     19,716 |      9,246 |     8,760 |     1,710 |
| C++          |      4 |      2,305 |      1,870 |       100 |       335 |
| CSV          |     11 |      1,373 |          0 |         0 |       106 |
| Markdown     |      1 |        270 |        197 |         0 |        73 |
| C++ header   |      2 |        125 |         59 |        47 |        19 |
| HTML         |      2 |         33 |         22 |         8 |         3 |
| INI          |      2 |         13 |          6 |         5 |         2 |
| Total        | 72,352 | 36,419,932 | 27,491,183 | 4,332,590 | 4,482,972 |
---------------------------------------------------------------------------


real    0m0.550s
user    0m4.313s
sys     0m1.076s
```

This repo (v0.1)

```
$ out/ubsan/cli/Debug/locc-cli -vpe=ext --sort-column=Comments --sort-ascend
params:
  sort by       : Comments
  exclude       : ext
  grammars      : 
  threads       : 16
  sort ascend   : true
  no progress   : true
  verbose       : true
  path          : .


-----------------------------------------------------------
| File Type    | Files | Lines |  Code | Comments | Empty |
-----------------------------------------------------------
| CMake script |     4 |   192 |   160 |        0 |    32 |
| Markdown     |     1 |   126 |    93 |        0 |    33 |
| JSON         |     3 |   634 |   631 |        3 |     0 |
| C++ header   |    19 |   448 |   345 |        5 |    98 |
| C++          |    17 | 1,279 | 1,083 |       16 |   180 |
| Total        |    44 | 2,679 | 2,312 |       24 |   343 |
-----------------------------------------------------------

```

## Building

Use CMake and a preset / generator of your choice. All provided presets use Ninja Multi-Config and the default compiler except those named `*clang*`.

### Requirements

- Desktop OS (`int main()` entry point)
- CMake 3.23+
- C++23 compiler (and stdlib)

## Contributing

Pull requests are welcome.

## Architecture

* `locc-lib` is a library that can be used by CLI / GUI executables
* `locc-cli` is the built-in CLI app that links to `locc-lib` and whose outputs are in this document; CMake option: `LOCC_BUILD_CLI`
* `locc-gui` is a built-in GUI app that links to `locc-lib`; CMake option: `LOCC_BUILD_GUI`

### Dependencies

```
   locc-gui   locc-cli
      |        |
  ----------   -
  |        |   |
 gvdi     locc-lib
             |
          -------
          |     |
        klib  djson
```

* [klib](https://github.com/karnkaul/klib)
* [gvdi](https://github.com/karnkaul/gvdi)
* [djson](https://github.com/karnkaul/djson)

[**Original Repository**](https://github.com/karnkaul/locc)
