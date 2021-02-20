#pragma once

#include <string>

class StatementGroup
{
public:
    std::string name() const;

    StatementGroup(long id, const std::string& name);
    StatementGroup(const StatementGroup&) = default;
    StatementGroup& operator=(const StatementGroup&) = default;

private:
    long id_;
    std::string name_;
};
