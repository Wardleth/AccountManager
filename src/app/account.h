#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "Transaction.h"
#include "Bank.h"

#include <vector>
#include <string>
#include <memory>

class Account
{
public:
    bool AddTransaction(const TransactionData& transaction);

    std::string iban() const;
    std::weak_ptr<const Currency> currency() const;

    Account();
    Account(const Account& other) = delete;
    Account& operator=(const Account& other) = delete;

private:
    std::string iban_;
    std::vector<TransactionData> transactions_;
    std::weak_ptr<const Currency> currency_;
};

#endif // ACCOUNT_H
