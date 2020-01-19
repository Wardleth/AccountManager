#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <sstream>

#include <caisseepargnestatementreader.h>

using namespace testing;


TEST(CaisseEpargneStatementReader, readStatement_shouldFailIfStartDateIsAfterEndDate)
{

}

TEST(CaisseEpargneStatementReader, readStatement_shouldFailIfBalancesDoesNotMatch)
{

}

void checkStatement(
    const Statement& s,
    const std::string& expectedDate,
    double expectedAmount,
    const std::string& expectedLabel,
    const std::string& expectedDetails,
    Currency expectedCurrency
) {
    std::tm date = s.date();
    char buffer[255]; std::strftime(buffer, sizeof(buffer), "%Y/%m/%d", &date);
    EXPECT_EQ(expectedDate, buffer);
    EXPECT_EQ(expectedAmount, s.amount());
    EXPECT_EQ(expectedLabel, s.label());
    EXPECT_EQ(expectedDetails, s.details());
    EXPECT_EQ(expectedCurrency, s.currency());
}

TEST(CaisseEpargneStatementReader, readStatement_shouldReadAllInfo)
{
    std::stringstream ss(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Num�ro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de p�riode;;;; +40\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "31 / 12 / 19; XXX ; Libell� d�bit; -50, 00;; D�tails d�bit;\n"
        "03 / 12 / 19; XXX ; Libell� cr�dit;; +100, 00; D�tails cr�dit;\n"
        "Solde en d�but de p�riode;;;; -10\n"
    );
    CaisseEpargneStatementReader reader;
    StatementList statements = reader.readStatementList(ss);

    EXPECT_EQ("00000", statements.bankId);
    EXPECT_EQ("11111", statements.counterId);
    EXPECT_EQ("01234567890", statements.accountNumber);
    EXPECT_EQ(2, statements.items.size());
    checkStatement(statements.items[0], "2019/12/03", 100, "Libell� cr�dit", "D�tails cr�dit", Currency::Euro);
    checkStatement(statements.items[1], "2019/12/31", -50, "Libell� d�bit", "D�tails d�bit", Currency::Euro);

}
