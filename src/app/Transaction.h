#pragma once

#include "TransactionData.h"
#include "TransactionGroup.h"

#include <memory>

class Transaction
{
public:
    std::tm date() const;
    double amount() const;
    std::string label() const;
    std::string details() const;
    std::weak_ptr<const TransactionGroup> group() const;

    bool isCredit() const;
    bool isDebit() const;

    Transaction(const std::tm& date, double amount, const std::string& label, const std::string& details);
    Transaction(const Transaction&) = delete;
    Transaction& operator=(const Transaction&) = delete;

private:
    long id_;
    TransactionData data_;
    std::weak_ptr<const TransactionGroup> group_;
};
