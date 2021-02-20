#pragma once

#include "statement.h"

#include <vector>

struct StatementList
{
	std::string bankId;
	std::string agencyId;
	std::string accountNumber;
	double startBalance;
	double endBalance;
	std::vector<Statement> items;
};
