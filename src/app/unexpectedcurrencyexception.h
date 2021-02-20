#pragma once

#include <stdexcept>

class UnexpectedCurrencyException : std::runtime_error
{
public:
	UnexpectedCurrencyException(const std::string& currencyCode);
};
