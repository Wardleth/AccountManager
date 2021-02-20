#pragma once

#include <string>

class Bank
{
public:
	std::string bic() const;
	std::string name() const;

	Bank(const std::string& bic, const std::string& name);
	Bank(const Bank&) = delete;
	Bank& operator=(const Bank& other) = delete;

private:
	std::string bic_;
	std::string name_;
};
