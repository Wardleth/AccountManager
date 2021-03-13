#pragma once

#include <string>

class TransactionGroup
{
public:
    std::string name() const;

    TransactionGroup(long id, const std::string& name);
    TransactionGroup(const TransactionGroup&) = default;
    TransactionGroup& operator=(const TransactionGroup&) = default;

private:
    long id_;
    std::string name_;
};
