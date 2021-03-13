#pragma once

#include <string>
#include <chrono>

struct TransactionData
{
    std::tm date;
    double amount;
    std::string label;
    std::string details;
};
