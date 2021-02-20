#include "statement.h"

Statement::Statement(const std::tm& date, double amount, const std::string& label, const std::string& details):
	date_(date),
	amount_(amount),
	label_(label),
	details_(details)
{}

bool Statement::isCredit() const {
	return amount_ >= 0;
}

bool Statement::isDebit() const {
	return amount_ <= 0;
}

std::tm Statement::date() const {
	return date_;
}

double Statement::amount() const {
	return amount_;
}

std::string Statement::label() const {
	return label_;
}

std::string Statement::details() const {
	return details_;
}

std::weak_ptr<const StatementGroup> Statement::group() const {
	return group_;
}
