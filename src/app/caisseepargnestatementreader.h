#pragma once

#include "statementreader.h"

#include <map>

class CaisseEpargneStatementReader : public StatementReader
{
public:
    virtual Statement readStatement(std::istream &is) override;
    virtual StatementList readStatementList(std::istream& is) override;

private:
    std::map<std::string, std::string> readMetadata(std::istream& is);
    double extractBalance(const std::string& line);

private:
    Currency currency_;
};
