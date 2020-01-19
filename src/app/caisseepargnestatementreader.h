#ifndef CAISSEEPARGNESTATEMENTREADER_H
#define CAISSEEPARGNESTATEMENTREADER_H

#include "statementreader.h"

class CaisseEpargneStatementReader : public StatementReader
{
public:
    virtual Statement readStatement(std::istream &is) override;
};

#endif // CAISSEEPARGNESTATEMENTREADER_H
