#pragma once

#include "Transaction.h"

#include <vector>

struct TransactionList
{
	std::string bankId;
	std::string agencyId;
	std::string accountNumber;
	double startBalance;
	double endBalance;
	std::vector<TransactionData> items;
};
