#pragma once

#include <vector>
#include <string>

namespace StringUtils
{
	std::vector<std::string> split(const std::string& s, char delimiter);
	std::string trim(const std::string& s);
	bool startsWith(const std::string& prefix, const std::string& s);
}
