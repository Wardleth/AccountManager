#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <vector>

#include "statement.h"

class Account
{
public:
    Account();
    Account(const Account& other) = delete;
    Account& operator=(const Account& other) = delete;

    bool AddStatement(const Statement& statement);

private:
    std::string bankCode_;
    std::string agencyCode_;
    std::string identifier_;
    std::vector<Statement> statements_;
};

#endif // ACCOUNT_H
