#include "currency.h"

std::string Currency::code() const {
	return code_;
}

std::string Currency::name() const {
	return name_;
}

std::string Currency::number() const {
	return number_;
}

std::string Currency::symbol() const {
	return symbol_;
}

Currency::Currency(const std::string& code, const std::string& name, const std::string& number, const std::string& symbol) :
	code_(code),
	name_(name),
	number_(number),
	symbol_(symbol)
{}
