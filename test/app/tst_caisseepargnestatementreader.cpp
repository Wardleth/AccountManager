#include <currency.h>
#include <caisseepargnestatementreader.h>
#include <malformedstatementlistexception.h>
#include <unexpectedcurrencyexception.h>

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <sstream>
#include <memory>

using namespace testing;


void checkStatement(
    const Statement& s,
    const std::string& expectedDate,
    double expectedAmount,
    const std::string& expectedLabel,
    const std::string& expectedDetails
) {
    std::tm date = s.date();
    char buffer[255]; std::strftime(buffer, sizeof(buffer), "%Y/%m/%d", &date);
    EXPECT_EQ(expectedDate, buffer);
    EXPECT_DOUBLE_EQ(expectedAmount, s.amount());
    EXPECT_EQ(expectedLabel, s.label());
    EXPECT_EQ(expectedDetails, s.details());
}

class CaisseEpargneStatementReaderTest : public testing::Test
{
public:
    CaisseEpargneStatementReaderTest() : 
        testCurrency_(std::make_shared<Currency>("EUR", "Euro", "978", "€")),
        reader(testCurrency_)
    {}

private:
    std::shared_ptr<Currency> testCurrency_;

public:
    CaisseEpargneStatementReader reader;
};

TEST_F(CaisseEpargneStatementReaderTest, readStatement_shouldFailWithMissingMetadata)
{
    std::stringstream ss1(
        "Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; 10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "Solde en début de période;;;; 10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss1), MalformedStatementListException);

    std::stringstream ss2(
        "Code de la banque : 00000;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; 10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "Solde en début de période;;;; 10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss2), MalformedStatementListException);

    std::stringstream ss3(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; 10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "Solde en début de période;;;; 10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss3), MalformedStatementListException);

    std::stringstream ss4(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX;\n"
        "\n"
        "Solde en fin de période;;;; 10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "Solde en début de période;;;; 10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss4), MalformedStatementListException);

    std::stringstream ss5(
        "Code de la banque : 00000;Code de l'agence : 11111;\n"
        "Numéro de compte : 01234567890; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; 10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "Solde en début de période;;;; 10\n"
    );
    ASSERT_NO_THROW(reader.readStatementList(ss5));
}

TEST_F(CaisseEpargneStatementReaderTest, readStatement_shouldFailWithMalformedMetadata)
{
    std::stringstream ss(
        "Code de la banque = 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; 10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "Solde en début de période;;;; 10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss), MalformedStatementListException);
}

TEST_F(CaisseEpargneStatementReaderTest, readStatement_shouldFailWithMalformedBalanceLine)
{
    std::stringstream ss1(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;; 10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "Solde en début de période;;;; 10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss1), MalformedStatementListException);

    std::stringstream ss2(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; notANumber\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "Solde en début de période;;;; 10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss2), MalformedStatementListException);
}


TEST_F(CaisseEpargneStatementReaderTest, readStatement_shouldFailWithMalformedStatementDate)
{
    std::stringstream ss1(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; +10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "31/12; XXX ; Libellé débit; -50,50;; Détails débit;\n"
        "Solde en début de période;;;; +10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss1), MalformedStatementListException);

    std::stringstream ss2(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; 10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "31/12/2019; XXX ; Libellé débit; -50,50;; Détails débit;\n"
        "Solde en début de période;;;; 10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss2), MalformedStatementListException);

    std::stringstream ss3(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; 10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "30/02/19; XXX ; Libellé débit; -50,50;; Détails débit;\n"
        "Solde en début de période;;;; 10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss3), MalformedStatementListException);
}

TEST_F(CaisseEpargneStatementReaderTest, readStatement_shouldFailWithMalformedStatementAmount)
{
    std::stringstream ss1(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; +10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "31/12/19; XXX ; Wrong statement; +50,50;; Cannot have positive debit;\n"
        "Solde en début de période;;;; +10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss1), MalformedStatementListException);

    std::stringstream ss2(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; 10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "31/12/19; XXX ; Wrong statement;; -50,50; Cannot have negative credit;\n"
        "Solde en début de période;;;; 10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss2), MalformedStatementListException);

    std::stringstream ss3(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; 10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "31/12/19; XXX ; Wrong statement; -50,50; 50,50; Cannot have both debit and credit;\n"
        "Solde en début de période;;;; 10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss3), MalformedStatementListException);

    std::stringstream ss4(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; 10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "31/12/19; XXX ; Wrong statement;;; Must have at least one of debit/credit filled with an amount;\n"
        "Solde en début de période;;;; 10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss4), MalformedStatementListException);

    std::stringstream ss5(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; 10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "31/12/19; XXX ; Wrong statement; notADouble;; Debit must be a double;\n"
        "Solde en début de période;;;; 10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss5), MalformedStatementListException);

    std::stringstream ss6(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; 10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "31/12/19; XXX ; Wrong statement;; notADouble; Credit must be a double;\n"
        "Solde en début de période;;;; 10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss6), MalformedStatementListException);
}

TEST_F(CaisseEpargneStatementReaderTest, readStatement_shouldFailWithInvalidBalanceDelta)
{
    std::stringstream ss(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; 60\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "31/12/19; XXX ; Libellé débit; -50,50;; Détails débit;\n"
        "03/12/19; XXX ; Libellé crédit;; 100,00; Détails crédit;\n"
        "Solde en début de période;;;; -10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss), MalformedStatementListException);
}

TEST_F(CaisseEpargneStatementReaderTest, readStatement_shouldSucceedWithValidData)
{
    std::stringstream ss(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; 39,50\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "31/12/19; XXX ; Libellé débit; -50,50;; Détails débit;\n"
        "03/12/19; XXX ; Libellé crédit;; 100,00; Détails crédit;\n"
        "Solde en début de période;;;; -10\n"
    );
    StatementList statements = reader.readStatementList(ss);

    EXPECT_EQ("00000", statements.bankId);
    EXPECT_EQ("11111", statements.agencyId);
    EXPECT_EQ("01234567890", statements.accountNumber);
    EXPECT_DOUBLE_EQ(-10.0, statements.startBalance);
    EXPECT_DOUBLE_EQ(39.5, statements.endBalance);
    EXPECT_EQ(2, statements.items.size());
    checkStatement(statements.items[0], "2019/12/03", 100, "Libellé crédit", "Détails crédit");
    checkStatement(statements.items[1], "2019/12/31", -50.5, "Libellé débit", "Détails débit");
}

TEST_F(CaisseEpargneStatementReaderTest, readStatement_shouldFailWithUnexpectedCurrency)
{
    std::stringstream ss(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: CAD;\n"
        "\n"
        "Solde en fin de période;;;; 10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "Solde en début de période;;;; 10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss), UnexpectedCurrencyException);
}

TEST_F(CaisseEpargneStatementReaderTest, readStatement_shouldSucceedWithEmptyStatements)
{
    std::stringstream ss(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; 10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "Solde en début de période;;;; 10\n"
    );
    StatementList statements = reader.readStatementList(ss);

    EXPECT_EQ("00000", statements.bankId);
    EXPECT_EQ("11111", statements.agencyId);
    EXPECT_EQ("01234567890", statements.accountNumber);
    EXPECT_DOUBLE_EQ(10.0, statements.startBalance);
    EXPECT_DOUBLE_EQ(10.0, statements.endBalance);
    EXPECT_EQ(0, statements.items.size());
}
