#pragma once

#include "currency.h"

#include <string>
#include <chrono>

class Statement
{
public:
    Statement(const std::tm& date, double amount, const std::string& label, const std::string& details, Currency currency);

    bool isCredit() const;
    bool isDebit() const;

    std::tm date() const;
    double amount() const;
    std::string label() const;
    std::string details() const;
    Currency currency() const;

private:
    std::tm date_;
    double amount_;
    std::string label_;
    std::string details_;
    Currency currency_;
};
