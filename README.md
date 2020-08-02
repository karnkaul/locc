## Lines of Code Counter

[![Build status](https://ci.appveyor.com/api/projects/status/5xlc0f3xsdypp0xn?svg=true)](https://ci.appveyor.com/project/karnkaul/loc)

This is a hobby application designed to count lines of code given a list of files or a directory.

### Usage

Syntax:

```
loc [options] <file0> [file1 file2...] (1)
loc [options] <directory> (2)

(1) provide an explicit list of files to count
(2) provide a root directory to recursively traverse (at the risk of counting unwanted files)
```

#### Options:

- \*`[-i|--ignore]=[line|block-start,block-end]`: Replace ignore delimiters (`//` and `/\*,\*/` by default)
- \*`--skip-substr=[...]`: Add path substring pattern to ignore
- \*`--skip-ext=[...]`: Add specific extensions to ignore
- `[-o|--one-thread]`: Run everything on the main thread
- `[-b|--blanks]`: Include blank lines as lines of code
- `[-q|--quiet]`: Print nothing but total lines of code
- `[-v|--verbose]`: Lots of output
- `[-d|--debug]`: Random debug output
- `[-h|--help]`: Display help

*\*Supports multiple instances of same argument type*

### Building

Use CMake and a generator of your choice.
Requirements:

- C++17 compiler (and stdlib)

### Contributing

WIP
