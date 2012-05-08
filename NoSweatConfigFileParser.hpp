/*****************************************************************************
Copyright (c) 2012 Lion Krischer (krischer@geophysik.uni-muenchen.de)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

/// @file NoSweatConfigFileParser.hpp

#ifndef __NOSWEAT_CONFIGFILE_PARSER__
#define __NOSWEAT_CONFIGFILE_PARSER__


#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace NoSweat {
    class NoSweatConfigFileParser {
        public:
            inline NoSweatConfigFileParser(std::string default_config_file);
            inline NoSweatConfigFileParser(std::string default_config_file, std::string config_file);
            inline ~NoSweatConfigFileParser();
            inline void print_configuration();
            inline int get_int(std::string key);
            inline float get_float(std::string key);
            inline std::string get_string(std::string key);
            inline bool get_bool(std::string key);
            inline void read_config_file(std::string config_file);

        private:
            inline NoSweatConfigFileParser();
            inline void parse_default_config_file();
            inline bool is_key_available(std::string key);

            // Convert the string value to the corresponding type and add to the default key/value maps.
            inline void add_default_integer_value(const std::string key, const std::string value);
            inline void add_default_float_value(const std::string key, const std::string value);
            inline void add_default_string_value(const std::string key, std::string value);
            inline void add_default_bool_value(const std::string key, std::string value);

            // Set an already existing key to a value. If the key does not exist, nothing will happen.
            inline void set_integer_value(const std::string key, const std::string value);
            inline void set_float_value(const std::string key, const std::string value);
            inline void set_string_value(const std::string key, std::string value);
            inline void set_bool_value(const std::string key, std::string value);
            // Set a given key to value. Will search all maps and determine the key automatically.
            inline void set_value(const std::string key, std::string value);

            // Accepted types of configuration variables. The additional space
            // is important because otherwise it could as well be a key name.
            const std::vector<std::string> accepted_value_types_{"int ", "float ", "string ", "bool "};
            const std::string accepted_assignment_operators_{":="};
            // Case-insensitive accepted names for the boolean values.
            const std::vector<std::string> accepted_boolean_true_values_{"true", "yes", "y", "on", "1", "right"};
            const std::vector<std::string> accepted_boolean_false_values_{"false", "no", "n", "off", "0", "wrong"};
            // Path of the default configuration file.
            std::string default_config_file_;
            // Path of the configuration file.
            std::string config_file_;
            // Store all the configuration values in there. The first value in
            // the pair is always the default value and the second value is the
            // user set value. If no user set value was given at any point the
            // second value will also be the default value.
            std::map<std::string, std::pair<int, int>> integer_config_values_;
            std::map<std::string, std::pair<float, float>> float_config_values_;
            std::map<std::string, std::pair<std::string, std::string>> string_config_values_;
            std::map<std::string, std::pair<bool, bool>> bool_config_values_;
    };

    inline void trim(std::string& str);
    inline bool starts_with(const std::string str, const std::string substr);
}


/// The constructor takes the paths of the default configuration file and of the
/// normal configuration file and parses them both upon construction.
NoSweat::NoSweatConfigFileParser::NoSweatConfigFileParser(std::string default_config_file, std::string config_file)
: default_config_file_(default_config_file) {
    // Parse both config files.
    parse_default_config_file();
    read_config_file(config_file);
}

NoSweat::NoSweatConfigFileParser::NoSweatConfigFileParser(std::string default_config_file)
: default_config_file_(default_config_file) {
    parse_default_config_file();
}


NoSweat::NoSweatConfigFileParser::NoSweatConfigFileParser() {};
NoSweat::NoSweatConfigFileParser::~NoSweatConfigFileParser() {};


/// Print the current state of the configuration parser. Intended for debugging.
void NoSweat::NoSweatConfigFileParser::print_configuration() {
    std::cout << "NoSweatConfigFileParser object: default_config_file='" << default_config_file_ <<
        "', config_file='" << config_file_ << "'" << std::endl;
    if (integer_config_values_.size()) {
        std::cout << "\tInteger values:" << std::endl;
        for (auto i: integer_config_values_) {
            std::cout << "\t\t" << i.first << ": " << i.second.second << " (default value: " <<
                i.second.first << ")" << std::endl;
        }
    }
    if (float_config_values_.size()) {
        std::cout << "\tFloat values:" << std::endl;
        for (auto i: float_config_values_) {
            std::cout << "\t\t" << i.first << ": " << i.second.second << " (default value: " <<
                i.second.first << ")" << std::endl;
        }
    }
    if (string_config_values_.size()) {
        std::cout << "\tString values:" << std::endl;
        for (auto i: string_config_values_) {
            std::cout << "\t\t" << i.first << ": " << i.second.second << " (default value: " <<
                i.second.first << ")" << std::endl;
        }
    if (bool_config_values_.size()) {
        std::cout << "\tBoolean values:" << std::endl;
        std::string pretty_bool_1;
        std::string pretty_bool_2;
        for (auto i: bool_config_values_) {
            if (i.second.first)
                pretty_bool_1 = "true";
            else
                pretty_bool_1 = "false";
            if (i.second.second)
                pretty_bool_2 = "true";
            else
                pretty_bool_2 = "false";
            std::cout << "\t\t" << i.first << ": " << pretty_bool_2 << " (default value: " <<
                pretty_bool_1 << ")" << std::endl;
        }
    }
    }

}


void NoSweat::NoSweatConfigFileParser::parse_default_config_file() {
    std::ifstream file_stream{default_config_file_};
    std::string current_line;
    if (file_stream.is_open()) {
        // Loop over all lines.
        while (file_stream.good()) {
            std::getline(file_stream, current_line);
            trim(current_line);

            // Check if the trimmed line starts with a value keyword.
            bool starts_with_value_type{false};
            for (auto type: accepted_value_types_) {
                if (starts_with(current_line, type)) {
                    starts_with_value_type = true;
                    break;
                }
            }
            if (!starts_with_value_type)
                continue;

            // Split at the first equals sign, or colon, if it has none, skip the line.
            std::string::size_type index = current_line.find_first_of(accepted_assignment_operators_);
            // If it either has none of these symbol or the first occurence is at the end, skip the line.
            if (index == std::string::npos || index == (current_line.size() - 1))
                continue;
            // Otherwise split it in two parts.
            std::string type_and_key{current_line.substr(0, index)};
            std::string value{current_line.substr(index + 1)};
            trim(type_and_key);
            trim(value);

            // These should never be able to have 0 length, but just check
            // anyway because the library is not performance critical.
            if (type_and_key.size() == 0 || value.size() == 0)
                continue;

            index = type_and_key.find(" ");
            std::string type{type_and_key.substr(0, index)};
            std::string key{type_and_key.substr(index + 1)};
            trim(type);
            trim(key);

            if (type == "int")
                add_default_integer_value(key, value);
            else if (type == "float")
                add_default_float_value(key, value);
            else if (type == "string")
                add_default_string_value(key, value);
            else if (type == "bool")
                add_default_bool_value(key, value);
            else
                continue;
        }
        file_stream.close();
    }
    else {
        std::cout << "WARNING: Could not find the default configuration file " <<
            default_config_file_ << "." << std::endl;
    }
}


void NoSweat::NoSweatConfigFileParser::read_config_file(std::string config_file) {
    config_file_ = config_file;
    std::ifstream file_stream{config_file_};
    std::string current_line;
    if (file_stream.is_open()) {
        // Loop over all lines.
        while (file_stream.good()) {
            std::getline(file_stream, current_line);
            trim(current_line);

            std::string::size_type index{current_line.find_first_of("=:")};
            if (index == std::string::npos || index == 0 || index == (current_line.size() - 1))
                continue;

            std::string key{current_line.substr(0, index)};
            std::string value{current_line.substr(index + 1)};
            trim(key);
            trim(value);

            // If the type is given in the user configuration file it will be
            // enforced, e.g. it will not be accepted as a value for a key with
            // the same name but a different type.
            std::string enforce_type{""};
            bool starts_with_value_type{false};
            for (auto type: accepted_value_types_) {
                if (starts_with(current_line, type)) {
                    starts_with_value_type = true;
                    break;
                }
            }
            if (starts_with_value_type) {
                index = key.find_first_of(" \t");
                enforce_type=key.substr(0, index);
                key.erase(0, index);
                trim(key);
            }

            if (!enforce_type.empty()) {
                if (enforce_type == "int")
                    set_integer_value(key, value);
                else if (enforce_type == "float")
                    set_float_value(key, value);
                else if (enforce_type == "string")
                    set_string_value(key, value);
                else if (enforce_type == "bool")
                    set_bool_value(key, value);
                continue;
            }
            set_value(key, value);
        }
        file_stream.close();
    }
    else {
        std::cout << "WARNING: Could not find the configuration file " <<
            config_file_ << "." << std::endl;
    }
}


void NoSweat::NoSweatConfigFileParser::add_default_integer_value(const std::string key, const std::string value) {
    if (!is_key_available(key))
        return;
    int int_value;
    // Catch the possible exceptions.
    try {
        int_value = std::stoi(value);
    }
    catch (const std::invalid_argument& e) {
        return;
    }
    catch (const std::out_of_range& e) {
        return;
    }
    integer_config_values_[key] = std::make_pair(int_value, int_value);
}


void NoSweat::NoSweatConfigFileParser::add_default_float_value(const std::string key, const std::string value) {
    if (!is_key_available(key))
        return;
    float float_value;
    // Catch the possible exceptions.
    try {
        float_value = std::stof(value);
    }
    catch (const std::invalid_argument& e) {
        return;
    }
    catch (const std::out_of_range& e) {
        return;
    }
    float_config_values_[key] = std::make_pair(float_value, float_value);
}


void NoSweat::NoSweatConfigFileParser::add_default_string_value(const std::string key, std::string value) {
    if (!is_key_available(key))
        return;
    string_config_values_[key] = std::make_pair(value, value);
}


void NoSweat::NoSweatConfigFileParser::add_default_bool_value(const std::string key, std::string value) {
    if (!is_key_available(key))
        return;
    // Transform to lower case.
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    bool bool_value;
    // Map to corresponding boolean value.
    if (std::find(accepted_boolean_false_values_.begin(), accepted_boolean_false_values_.end(), value) !=
        accepted_boolean_false_values_.end())
        bool_value = false;
    else if (std::find(accepted_boolean_true_values_.begin(), accepted_boolean_true_values_.end(), value) != 
        accepted_boolean_true_values_.end())
        bool_value = true;
    else
        return;
    bool_config_values_[key] = std::make_pair(bool_value, bool_value);
}


/// Returns true if the key has not been taken yet. Checks key/value maps.
/// It would be faster to keep a list of used keys around but I prefer simpler
/// data structures as long as performance is not critical.
bool NoSweat::NoSweatConfigFileParser::is_key_available(std::string key) {
    if(integer_config_values_.find(key) != integer_config_values_.end())
        return false;
    else if(float_config_values_.find(key) != float_config_values_.end())
        return false;
    else if(string_config_values_.find(key) != string_config_values_.end())
        return false;
    else if(bool_config_values_.find(key) != bool_config_values_.end())
        return false;
    return true;
}


void NoSweat::NoSweatConfigFileParser::set_integer_value(const std::string key, const std::string value){
    if (integer_config_values_.find(key) == integer_config_values_.end())
        return;
    int int_value;
    // Catch the possible exceptions.
    try {
        int_value = std::stoi(value);
    }
    catch (const std::invalid_argument& e) {
        return;
    }
    catch (const std::out_of_range& e) {
        return;
    }
    integer_config_values_[key].second = int_value;
}


void NoSweat::NoSweatConfigFileParser::set_float_value(const std::string key, const std::string value) {
    if (float_config_values_.find(key) == float_config_values_.end())
        return;
    float float_value;
    // Catch the possible exceptions.
    try {
        float_value = std::stof(value);
    }
    catch (const std::invalid_argument& e) {
        return;
    }
    catch (const std::out_of_range& e) {
        return;
    }
    float_config_values_[key].second = float_value;
}


void NoSweat::NoSweatConfigFileParser::set_string_value(const std::string key, std::string value) {
    if (string_config_values_.find(key) == string_config_values_.end())
        return;
    string_config_values_[key].second = value;
}


void NoSweat::NoSweatConfigFileParser::set_bool_value(const std::string key, std::string value) {
    if (bool_config_values_.find(key) == bool_config_values_.end())
        return;
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    // Map to corresponding boolean value.
    bool bool_value;
    if (std::find(accepted_boolean_false_values_.begin(), accepted_boolean_false_values_.end(), value) !=
        accepted_boolean_false_values_.end())
        bool_value = false;
    else if (std::find(accepted_boolean_true_values_.begin(), accepted_boolean_true_values_.end(), value) != 
        accepted_boolean_true_values_.end())
        bool_value = true;
    else
        return;
    bool_config_values_[key].second = bool_value;
}


void NoSweat::NoSweatConfigFileParser::set_value(const std::string key, std::string value) {
    if (integer_config_values_.find(key) != integer_config_values_.end())
        set_integer_value(key, value);
    else if (float_config_values_.find(key) != float_config_values_.end())
        set_float_value(key, value);
    else if (string_config_values_.find(key) != string_config_values_.end())
        set_string_value(key, value);
    else if (bool_config_values_.find(key) != bool_config_values_.end())
        set_bool_value(key, value);
}


/// Two-sided in-place whitespace trimming.
inline void NoSweat::trim(std::string& str) {
    // Define all needed whitespaces: space, horizontal tab, new line, carriage return, vertical tab and line feed.
    std::string whitespace_chars{" \t\n\r\v\f"};
    // Remove leading spaces.
    str.erase(0, str.find_first_not_of(whitespace_chars));
    // Remove trailing spaces.
    str.erase(str.find_last_not_of(whitespace_chars) + 1);
}


/// Return true, if str starts with substr, false otherwise.
inline bool NoSweat::starts_with(const std::string str, const std::string substr) {
    std::string::size_type length = substr.size();
    if (length == 0) {
        // Obviously always true.
        return true;
    }
    if (str.substr(0, length) == substr)
        return true;
    return false;
}


int NoSweat::NoSweatConfigFileParser::get_int(std::string key) {
    if (integer_config_values_.find(key) == integer_config_values_.end())
        return 0;
    return integer_config_values_[key].second;
}


float NoSweat::NoSweatConfigFileParser::get_float(std::string key) {
    if (float_config_values_.find(key) == float_config_values_.end())
        return 0.0;
    return float_config_values_[key].second;
}


std::string NoSweat::NoSweatConfigFileParser::get_string(std::string key) {
    if (string_config_values_.find(key) == string_config_values_.end())
        return "";
    return string_config_values_[key].second;
}


bool NoSweat::NoSweatConfigFileParser::get_bool(std::string key) {
    if (bool_config_values_.find(key) == bool_config_values_.end())
        return false;
    return bool_config_values_[key].second;
}

#endif
