#include <parse_config.h>
#include <string>
#include <sstream>

String findConfig(String &config, String key)
{
    std::stringstream ss(config.c_str());
    std::string _key;
    while (ss >> _key)
    {
        if (_key.compare(key.c_str()) == 0)
        {
            char eq;
            ss >> eq;
            std::string value;
            ss >> value;
            return String(value.c_str());
        }
    }
}