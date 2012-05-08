NoSweatConfigFileParser
=======================

Simple C++11 based config file parser with a purely file based setup, implemented as a header only library and under [MIT license](http://www.opensource.org/licenses/MIT).

## Installation.
No need to compile anything, just put the NoSweatConfigFileParser.hpp file in one of your project's include paths or point the compiler to the directory containing the file, e.g.

```
gcc -Ipath/to/nosweatconfigfileparser ...
```

## Usage

```c++
include "NoSweatConfigFileParser.hpp"
using namespace NoSweat

int main() {
    // Init with two file paths, one to the default configuration file and one
    // to the user configuration file.
    config_parser = NoSweatConfigFileParser("default_config.cfg", "config.cfg");
    // Everything is already parsed and ready to be used.
    std::cout << "From config file: " << config_parser.get_int("value_name") << std::endl;
}
```
