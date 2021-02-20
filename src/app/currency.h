#pragma once

#include <map>
#include <string>
#include <stdexcept>

class Currency
{
public:
	std::string code() const;
	std::string name() const;
	std::string number() const;
	std::string symbol() const;

	Currency(const std::string& code, const std::string& name, const std::string& number, const std::string& symbol);
	Currency(const Currency&) = delete;
	Currency& operator=(const Currency&) = delete;

private:
	std::string code_;
	std::string name_;
	std::string number_;
	std::string symbol_;
};
