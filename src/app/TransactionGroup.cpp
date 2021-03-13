#include "TransactionGroup.h"

std::string TransactionGroup::name() const {
	return name_;
}

TransactionGroup::TransactionGroup(long id, const std::string& name) :
	id_(id),
	name_(name)
{}
