#ifndef STATEMENTREADER_H
#define STATEMENTREADER_H

#include <istream>

#include "statement.h"

class StatementReader
{
public:
    StatementReader() = default;
    StatementReader(const StatementReader& other) = delete;
    StatementReader& operator=(const StatementReader& other) = delete;
    virtual ~StatementReader() = default;
    virtual Statement readStatement(std::istream& is) = 0;
};

#endif // STATEMENTREADER_H
