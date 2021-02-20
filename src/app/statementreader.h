#pragma once

#include <istream>

#include "statement.h"
#include "statementlist.h"

class StatementReader
{
public:
    StatementReader() = default;
    StatementReader(const StatementReader& other) = delete;
    StatementReader& operator=(const StatementReader& other) = delete;
    virtual ~StatementReader() = default;
    virtual Statement readStatement(std::istream& is) = 0;
    virtual StatementList readStatementList(std::istream& is) = 0;
};
