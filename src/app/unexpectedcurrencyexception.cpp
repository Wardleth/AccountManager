#include "unexpectedcurrencyexception.h"

UnexpectedCurrencyException::UnexpectedCurrencyException(const std::string& currencyCode) :
	std::runtime_error(currencyCode)
{}
