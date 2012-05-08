/// @file test_nosweatconfigparser.cpp
///
/// Some "test cases".
///
/// To keep it simple this is done without a unit testing framework.

#include "NoSweatConfigFileParser.hpp"

using namespace NoSweat;


// Global test counters.
static int tests_passed = 0;
static int total_tests = 0;


// Simple helper test function. Testing for floats is usually dangerous but in
// this case it should be ok, because the same compiler with the same settings
// is used (header only lib).
template<typename T>
void assert_value(std::string value_name, T value_is, T value_should_be) {
    total_tests += 1;
    if (value_is == value_should_be) {
        tests_passed += 1;
        std::cout << ".";
        return;
    }
    std::cout << "FAILURE: The value for " << value_name << " is " << value_is <<
        ". Should be: " << value_should_be << "." << std::endl;
}


int main() {
    //////////
    // Test the default value setting and getting.
    //////////
    NoSweatConfigFileParser config_parser = NoSweatConfigFileParser{"default_config.cfg"};

    // Test the basic values.
    assert_value<int>("max_number_of_users", config_parser.get_int("max_number_of_users"), 1);
    assert_value<float>("movement_speed", config_parser.get_float("movement_speed"), 12.34);
    assert_value<std::string>("username", config_parser.get_string("username"), "some_user");
    assert_value<bool>("use_accelerator", config_parser.get_bool("use_accelerator"), true);

    // Test all the ways to set booleans.
    assert_value<bool>("is_true", config_parser.get_bool("is_true"), true);
    assert_value<bool>("is_false", config_parser.get_bool("is_false"), false);
    assert_value<bool>("is_also_false", config_parser.get_bool("is_also_false"), false);
    assert_value<bool>("this_one_is_true", config_parser.get_bool("this_one_is_true"), true);
    assert_value<bool>("also_true", config_parser.get_bool("also_true"), true);
    assert_value<bool>("value_is_false", config_parser.get_bool("value_is_false"), false);

    // Test the "specialities".
    assert_value<std::string>("key names can have spaces", config_parser.get_string("key names can have spaces"),
        "everything after the assignment operator will be the value");
    assert_value<float>("speed", config_parser.get_float("speed"), 1.0);
    assert_value<int>("$uper awesome names are also quite poss!!ble",
            config_parser.get_int("$uper awesome names are also quite poss!!ble"), 1);
    assert_value<float>("high_prec_interval", config_parser.get_float("high_prec_interval"), 0.00002);

    // Test that other values have not been set.
    assert_value<bool>("boolean", config_parser.get_bool("boolean"), false);
    assert_value<int>("asdf", config_parser.get_int("asdf"), 0);
    assert_value<std::string>("this is not very valid my dear.",
            config_parser.get_string("this is not very valid my dear."), "");
    // This will have the previously set value
    assert_value<float>("speed", config_parser.get_float("speed"), 1.0);

    // Trying to access non-existing variables will just return the default value for the corresponding type.
    assert_value<int>("random stuff", config_parser.get_int("random stuff"), 0);
    assert_value<float>("random stuff", config_parser.get_float("random stuff"), 0.0);
    assert_value<std::string>("random stuff", config_parser.get_string("random stuff"), "");
    assert_value<bool>("random stuff", config_parser.get_bool("random stuff"), false);

    // The same is true when trying to access variables with the wrong type.
    // Then just the default value of the variable type will be returned.
    assert_value<float>("max_number_of_users", config_parser.get_float("max_number_of_users"), 0.0);


    //////////
    // Now load an actual config file and test that overwriting the default values works.
    //////////
    config_parser.read_config_file("config.cfg");

    assert_value<int>("max_number_of_users", config_parser.get_int("max_number_of_users"), 22);
    assert_value<float>("movement_speed", config_parser.get_float("movement_speed"), 123.4);
    assert_value<std::string>("username", config_parser.get_string("username"), "some_other_user");
    assert_value<bool>("use_accelerator", config_parser.get_bool("use_accelerator"), false);
    assert_value<std::string>("key names can have spaces", config_parser.get_string("key names can have spaces"),
        "new value");

    // Values that are not specified in the default value files will not be parsed.
    assert_value<int>("random stuff", config_parser.get_int("random stuff"), 0);
    assert_value<float>("random stuff", config_parser.get_float("random stuff"), 0.0);
    assert_value<std::string>("random stuff", config_parser.get_string("random stuff"), "");
    assert_value<bool>("random stuff", config_parser.get_bool("random stuff"), false);


    //////////
    // Loading both the default config file and the normal config file upon construction has the same effect.
    //////////
    NoSweatConfigFileParser config_parser_2 = NoSweatConfigFileParser{"default_config.cfg", "config.cfg"};

    assert_value<int>("max_number_of_users", config_parser_2.get_int("max_number_of_users"), 22);
    assert_value<float>("movement_speed", config_parser_2.get_float("movement_speed"), 123.4);
    assert_value<std::string>("username", config_parser_2.get_string("username"), "some_other_user");
    assert_value<bool>("use_accelerator", config_parser_2.get_bool("use_accelerator"), false);
    assert_value<std::string>("key names can have spaces", config_parser_2.get_string("key names can have spaces"),
        "new value");

    // Values that are not specified in the default value files will not be parsed.
    assert_value<int>("random stuff", config_parser_2.get_int("random stuff"), 0);
    assert_value<float>("random stuff", config_parser_2.get_float("random stuff"), 0.0);
    assert_value<std::string>("random stuff", config_parser_2.get_string("random stuff"), "");
    assert_value<bool>("random stuff", config_parser_2.get_bool("random stuff"), false);

    assert_value<float>("speed", config_parser_2.get_float("speed"), 1.0);
    assert_value<int>("$uper awesome names are also quite poss!!ble",
            config_parser_2.get_int("$uper awesome names are also quite poss!!ble"), 1);
    assert_value<float>("high_prec_interval", config_parser_2.get_float("high_prec_interval"), 0.00002);


    // Print some kind of "error report".
    std::cout << std::endl;
    std::cout << "Passed " << tests_passed << " of " << total_tests << " \"tests\" (asserts)." << std::endl;
}
