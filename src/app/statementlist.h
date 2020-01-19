#pragma once

#include "statement.h"

#include <vector>

struct StatementList
{
	std::string bankId;
	std::string counterId;
	std::string accountNumber;
	std::vector<Statement> items;
};
