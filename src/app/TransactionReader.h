#pragma once

#include <istream>

#include "Transaction.h"
#include "TransactionList.h"

class TransactionReader
{
public:
    TransactionReader() = default;
    TransactionReader(const TransactionReader& other) = delete;
    TransactionReader& operator=(const TransactionReader& other) = delete;
    virtual ~TransactionReader() = default;
    virtual TransactionData readTransaction(std::istream& is) = 0;
    virtual TransactionList readTransactionList(std::istream& is) = 0;
};
