#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <sstream>

#include "caisseepargnestatementreader.h"

using namespace testing;


TEST(CaisseEpargneStatementReader, readStatement_shouldFailIfStartDateIsAfterEndDate)
{

}

TEST(CaisseEpargneStatementReader, readStatement_shouldFailIfBalancesDoesNotMatch)
{

}

TEST(CaisseEpargneStatementReader, readStatement_shouldReadAllInfo)
{
    std::stringstream ss;
    CaisseEpargneStatementReader reader;
    Statement statement = reader.readStatement(ss);
}
