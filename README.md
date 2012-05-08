NoSweatConfigFileParser
=======================

Simple configuration file parser for C++11 with a purely file based setup, implemented as a header only library and under [MIT license](http://www.opensource.org/licenses/MIT).

### Why another configuration file parser
Sometimes I just want to be able to configure some values in the early stages of a project without having to care about external dependencies or, like many config file parsers do, having to specify all key/value pairs and default values in the code.

The NoSweatConfigFileParser can only deal with four different data types and does not have any advanced features, so at some point in a project you might want to move on to a more elaborate library like [Boost.Program_options](http://www.boost.org/doc/libs/1_49_0/doc/html/program_options.html).

### Features
The NoSweatConfigFileParser is configured completely via two files. The first file, which from now on will be called the **default config file**, specifies the type, the key name and the default value of every configuration option in one line as follows:

```
{int/float/string/bool} key_name {= or :} default_value
```

Everything that does fit this syntax will be silently ignored. This enables comments/grouping/...

```
[some options]
int connection_per_user = 1
float max_bandwidth_per_user = 10.0
string default_uri = www.google.de
bool is_admin = false
```

Everything after the type specifier and before the assignment operator (= or :) will be considered part of the key name, and everything after the operator part of the value. See the example default configuration file tests/default_config.cfg for some line it will and others it will not parse.


### Installation
No need to compile anything, just put the NoSweatConfigFileParser.hpp file in one of your project's include paths or point the compiler to the directory containing the file. Most compilers currently also need to be told to compile with C++11 support.

It has no dependencies, but needs a compiler supporting C++11. Currently only tested with gcc 4.7.

```
g++-4.7 -std=c++11 -Ipath/to/nosweatconfigfileparser ...
```

### Usage

```c++
#include <iostream>
#include "NoSweatConfigFileParser.hpp"

using namespace NoSweat;

int main() {
    // Init with two file paths, one to the default configuration file and one
    // to the user configuration file.
    NoSweatConfigFileParser config_parser = NoSweatConfigFileParser("default_config.cfg", "config.cfg");
    // Everything is already parsed and ready to be used.
    std::cout << "From config file: " << config_parser.get_int("value_name") << std::endl;
}
```
