#include "utils.h"

#include <algorithm>
#include <sstream>

namespace StringUtils
{
    std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    std::string trim(const std::string& s) {
        auto front = std::find_if_not(s.begin(), s.end(), std::isspace);
        auto back = std::find_if_not(s.rbegin(), s.rend(), std::isspace).base();
        return (back < front) ? std::string() : std::string(front, back);
    }

    bool startsWith(const std::string& prefix, const std::string& s) {
        return (s.rfind(prefix, 0) == 0);
    }
}