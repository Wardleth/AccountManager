#include "bank.h"

std::string Bank::bic() const {
	return bic_;
}

std::string Bank::name() const {
	return name_;
}

Bank::Bank(const std::string& bic, const std::string& name) :
	bic_(bic),
	name_(name)
{}
