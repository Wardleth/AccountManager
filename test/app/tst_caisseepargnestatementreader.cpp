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
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; +40\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "31 / 12 / 19; XXX ; Libellé débit; -50, 00;; Détails débit;\n"
        "03 / 12 / 19; XXX ; Libellé crédit;; +100, 00; Détails crédit;\n"
        "Solde en début de période;;;; -10\n"
    );
    CaisseEpargneStatementReader reader;
    StatementList statements = reader.readStatementList(ss);

    EXPECT_EQ("00000", statements.bankId);
    EXPECT_EQ("11111", statements.counterId);
    EXPECT_EQ("01234567890", statements.accountNumber);
    EXPECT_EQ(2, statements.items.size());
    checkStatement(statements.items[0], "2019/12/03", 100, "Libellé crédit", "Détails crédit", Currency::Euro);
    checkStatement(statements.items[1], "2019/12/31", -50, "Libellé débit", "Détails débit", Currency::Euro);

}
