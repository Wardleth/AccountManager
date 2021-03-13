#include "Account.h"

bool Account::AddTransaction(const TransactionData& transaction) {
	(void)transaction;
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
