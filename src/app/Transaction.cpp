#include "Transaction.h"

std::tm Transaction::date() const {
	return data_.date;
}

double Transaction::amount() const {
	return data_.amount;
}

std::string Transaction::label() const {
	return data_.label;
}

std::string Transaction::details() const {
	return data_.details;
}

std::weak_ptr<const TransactionGroup> Transaction::group() const {
	return group_;
}

bool Transaction::isCredit() const {
	return data_.amount >= 0;
}

bool Transaction::isDebit() const {
	return data_.amount <= 0;
}

Transaction::Transaction(const std::tm& date, double amount, const std::string& label, const std::string& details) :
	id_{ 0 },
	data_{ date, amount, label, details }
{}
