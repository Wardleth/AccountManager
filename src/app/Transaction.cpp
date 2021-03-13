#include "Transaction.h"

TransactionData::TransactionData(const std::tm& date, double amount, const std::string& label, const std::string& details):
	date(date),
	amount(amount),
	label(label),
	details(details)
{}

bool TransactionData::isCredit() const {
	return amount >= 0;
}

bool TransactionData::isDebit() const {
	return amount <= 0;
}
