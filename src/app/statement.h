#ifndef STATEMENT_H
#define STATEMENT_H

#include <chrono>

#include "statementkind.h"

struct Statement
{
    double amount;
    //std::time_t date;
    StatementKind type;
    std::string details;
};

#endif // STATEMENT_H
