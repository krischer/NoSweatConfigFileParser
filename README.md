NoSweatConfigFileParser
=======================

Simple configuration file parser for C++11 with a purely file based setup, implemented as a header only library and under [MIT license](http://www.opensource.org/licenses/MIT).

## Why another configuration file parser
Sometimes I just want to be able to configure some values in the early stages of a project without having to care about external dependencies or, like many configuration file parsers do, having to specify all key/value pairs and default values in the code.

The NoSweatConfigFileParser can only deal with four different data types and does not have any advanced features, so at some point in a project you might want to move on to a more elaborate library like [Boost.Program_options](http://www.boost.org/doc/libs/1_49_0/doc/html/program_options.html).

## Configuration
The NoSweatConfigFileParser is configured completely via two files. The **default configuration file** and the **user configuration file**.

### Default configuration file
The default configuration file specifies the type, the key name and the default value of every configuration option. It is intended for the developer to always assure that some values are set and have sensible default values. The syntax is

```
{int/float/string/bool} key_name {= or :} default_value
```

Everything that does not fit this syntax or cannot be handled by the parser will be silently ignored. This enables comments/grouping/...

**Example:**

```
[some options]
int connection_per_user = 1
float max_bandwidth_per_user = 10.0
# Comment about the next line...
string default_uri = www.google.de
bool is_admin = false
```

Everything after the type specifier and before the assignment operator (= or :) will be considered part of the key name, and everything after the operator part of the value. See the example [default configuration file](https://github.com/Kurli/NoSweatConfigFileParser/blob/master/tests/default_config.cfg) for some lines it will and others it will not parse.

### User configuration file
The user configuration file can overwrite values set in the default configuration file. It is intended to provide a simple configuration file for the user and has the same syntax as the default configuration file, except that the type specifier is optional. Any value not specified in the default configuration file, or specified with another type, will be ignored. The syntax is also compatible with standard [INI files](http://en.wikipedia.org/wiki/INI_file), although sections will be ignored and a flat hierarchy enforced.

**Example:**

```
[user options]
; Allow more connections per user than the default value.
connections_per_user = 2
```
See the example [user configuration file](https://github.com/Kurli/NoSweatConfigFileParser/blob/master/tests/config.cfg) for some more information.

## Installation
No need to compile anything, just put the *NoSweatConfigFileParser.hpp* file in one of your project's include paths or point the compiler to the directory containing the file. Most compilers currently also need to be told to compile with C++11 support.

It has no dependencies, but needs a compiler supporting C++11. Currently only tested with gcc 4.7.

```
g++-4.7 -std=c++11 -Ipath/to/nosweatconfigfileparser ...
```

## Example Usage
Assuming two files,

**default_config.cfg**
```
int number_of_connections=1
float maximum bandwidth=123.45
```

and

**config.cfg**
```
number_of_connections=2
```

it can be used as follows:

```c++
#include <iostream>
#include "NoSweatConfigFileParser.hpp"

using namespace NoSweat;

int main() {
    // Init with two file paths, one to the default configuration file and one
    // to the user configuration file.
    NoSweatConfigFileParser config_parser{"default_config.cfg", "config.cfg"};
    // Everything is already parsed and ready to be used.
    std::cout << "Number of connections: " << config_parser.get_int("number_of_connections") << std::endl;
    std::cout << "Maximum bandwidth: " << config_parser.get_float("maximum bandwidth") << std::endl;
}
```

**Output:**

```
Number of connections: 2
Maximum bandwidth: 123.45
```

## Short reference

### Constructors and general methods.
**NoSweat::NoSweatConfigFileParser::NoSweatConfigFileParser(std::string default_config_file_path)**
Constructor that just parses the default config file.

NoSweat::NoSweatConfigFileParser::NoSweatConfigFileParser(std::string default_config_file_path, std::string config_file_path)
:    Constructor that also parses the user config file.

void NoSweat::NoSweatConfigFileParser::read_config_file(std::string config_file_path)
:    Read the config file. Useful if it has not been read or has changed.

void NoSweat::NoSweatConfigFileParser::print_configuration()
:    Print the current state of the configuration to stdout. Useful for debugging.

### Retrieving values
The type has to be specified. If no value exists for the given key name and implicitly given type, a default value (int:0, float:0.0, string:"", bool:false) will returned. No exception will ever be raised.


int NoSweat::NoSweatConfigFileParser::get_int(std::string key_name)
:    Get an integer config value.
float NoSweat::NoSweatConfigFileParser::get_float(std::string key_name)
:    Get a float config value.
std::string NoSweat::NoSweatConfigFileParser::get_string(std::string key_name)
:    Get a string config value.
bool NoSweat::NoSweatConfigFileParser::get_bool(std::string key_name)
:    Get a bool config value.

