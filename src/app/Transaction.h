#pragma once

#include "TransactionGroup.h"
#include "currency.h"

#include <string>
#include <chrono>
#include <memory>

struct TransactionData
{
    bool isCredit() const;
    bool isDebit() const;

    TransactionData(const std::tm& date, double amount, const std::string& label, const std::string& details);
    TransactionData(const TransactionData&) = default;
    TransactionData& operator=(const TransactionData&) = default;

    std::tm date;
    double amount;
    std::string label;
    std::string details;
};
