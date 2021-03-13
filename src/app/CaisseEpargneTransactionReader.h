#pragma once

#include "TransactionReader.h"

#include <map>
#include <list>

class CaisseEpargneTransactionReader : public TransactionReader
{
public:
    virtual TransactionData readTransaction(std::istream &is) override;
    virtual TransactionList readTransactionList(std::istream& is) override;

    CaisseEpargneTransactionReader(std::weak_ptr<Currency> expectedCurrency);

private:
    std::map<std::string, std::string> readMetadata(std::istream& is);
    double extractDouble(const std::string& value) const;
    double extractBalance(const std::string& line) const;
    void ensureValidity(double balanceDelta, const std::list<TransactionData>& statements) const;

private:
    std::weak_ptr<Currency> expectedCurrency_;
};
