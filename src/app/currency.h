#pragma once

#include <map>
#include <string>
#include <stdexcept>

enum class Currency
{
	Euro
};

const static std::map<std::string, Currency> CurrencyMap{
	{ "EUR", Currency::Euro }
};

inline Currency stringToCurrency(const std::string& currencyString) {
	auto currency = CurrencyMap.find(currencyString);
	if (currency == CurrencyMap.end()) {
		throw std::runtime_error("Unkown currency");
	}
	return currency->second;
}
