#include "statementgroup.h"

std::string StatementGroup::name() const {
	return name_;
}

StatementGroup::StatementGroup(long id, const std::string& name) :
	id_(id),
	name_(name)
{}
