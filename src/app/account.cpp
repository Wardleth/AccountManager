#include "account.h"

bool Account::AddStatement(const Statement& statement) {
	(void)statement;
	return false;
}

std::string Account::iban() const {
	return iban_;
}

std::weak_ptr<const Currency> Account::currency() const {
	return currency_;
}

Account::Account()
{}
