#pragma once

#include "statementgroup.h"
#include "currency.h"

#include <string>
#include <chrono>
#include <memory>

class Statement
{
public:
    std::tm date() const;
    double amount() const;
    std::string label() const;
    std::string details() const;
    std::weak_ptr<const StatementGroup> group() const;

    bool isCredit() const;
    bool isDebit() const;

    Statement(const std::tm& date, double amount, const std::string& label, const std::string& details);
    Statement(const Statement&) = default;
    Statement& operator=(const Statement&) = default;

private:
    long id_;
    std::tm date_;
    double amount_;
    std::string label_;
    std::string details_;

    std::weak_ptr<const StatementGroup> group_;
};
