#pragma once

#include "statementreader.h"

#include <map>
#include <list>

class CaisseEpargneStatementReader : public StatementReader
{
public:
    virtual Statement readStatement(std::istream &is, Currency currency) override;
    virtual StatementList readStatementList(std::istream& is) override;

private:
    std::map<std::string, std::string> readMetadata(std::istream& is);
    double extractDouble(const std::string& value) const;
    double extractBalance(const std::string& line) const;
    void ensureValidity(double balanceDelta, const std::list<Statement>& statements) const;
};
