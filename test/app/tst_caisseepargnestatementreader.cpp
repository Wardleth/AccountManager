#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <sstream>

#include <caisseepargnestatementreader.h>
#include <malformedstatementlistexception.h>

using namespace testing;


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

TEST(CaisseEpargneStatementReader, readStatement_shouldFailWithMissingMetadata)
{
    CaisseEpargneStatementReader reader;

    std::stringstream ss1(
        "Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; +10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "Solde en début de période;;;; +10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss1), MalformedStatementListException);

    std::stringstream ss2(
        "Code de la banque : 00000;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; +10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "Solde en début de période;;;; +10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss2), MalformedStatementListException);

    std::stringstream ss3(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; +10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "Solde en début de période;;;; +10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss3), MalformedStatementListException);

    std::stringstream ss4(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX;\n"
        "\n"
        "Solde en fin de période;;;; +10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "Solde en début de période;;;; +10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss4), MalformedStatementListException);

    std::stringstream ss5(
        "Code de la banque : 00000;Code de l'agence : 11111;\n"
        "Numéro de compte : 01234567890; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; +10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "Solde en début de période;;;; +10\n"
    );
    ASSERT_NO_THROW(reader.readStatementList(ss5));
}

TEST(CaisseEpargneStatementReader, readStatement_shouldFailWithMalformedMetadata)
{
    CaisseEpargneStatementReader reader;

    std::stringstream ss(
        "Code de la banque = 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; +10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "Solde en début de période;;;; +10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss), MalformedStatementListException);
}

TEST(CaisseEpargneStatementReader, readStatement_shouldFailWithMalformedBalanceLine)
{
    CaisseEpargneStatementReader reader;

    std::stringstream ss1(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;; +10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "Solde en début de période;;;; +10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss1), MalformedStatementListException);

    std::stringstream ss2(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; notANumber\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "Solde en début de période;;;; +10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss2), MalformedStatementListException);
}


TEST(CaisseEpargneStatementReader, readStatement_shouldFailWithMalformedStatementDate)
{
    CaisseEpargneStatementReader reader;

    std::stringstream ss1(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; +10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "31 / 12; XXX ; Libellé débit; -50, 00;; Détails débit;\n"
        "Solde en début de période;;;; +10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss1), MalformedStatementListException);

    std::stringstream ss2(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; +10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "31 / 12 / 2019; XXX ; Libellé débit; -50, 00;; Détails débit;\n"
        "Solde en début de période;;;; +10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss2), MalformedStatementListException);

    std::stringstream ss3(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; +10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "30 / 02 / 19; XXX ; Libellé débit; -50, 00;; Détails débit;\n"
        "Solde en début de période;;;; +10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss3), MalformedStatementListException);
}

TEST(CaisseEpargneStatementReader, readStatement_shouldFailWithMalformedStatementAmount)
{
    CaisseEpargneStatementReader reader;

    std::stringstream ss1(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; +10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "31 / 12 / 19; XXX ; Wrong statement; +50, 00;; Cannot have positive debit;\n"
        "Solde en début de période;;;; +10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss1), MalformedStatementListException);

    std::stringstream ss2(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; +10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "31 / 12 / 19; XXX ; Wrong statement;; -50, 00; Cannot have negative credit;\n"
        "Solde en début de période;;;; +10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss2), MalformedStatementListException);

    std::stringstream ss3(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; +10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "31 / 12 / 19; XXX ; Wrong statement; -50, 00; +50,00; Cannot have both debit and credit;\n"
        "Solde en début de période;;;; +10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss3), MalformedStatementListException);

    std::stringstream ss4(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; +10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "31 / 12 / 19; XXX ; Wrong statement;;; Must have at least one of debit/credit filled with an amount;\n"
        "Solde en début de période;;;; +10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss4), MalformedStatementListException);

    //std::stringstream ss5(
    //    "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
    //    "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
    //    "\n"
    //    "Solde en fin de période;;;; +10\n"
    //    "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
    //    "31 / 12 / 19; XXX ; Wrong statement; notADouble;; Debit must be a double;\n"
    //    "Solde en début de période;;;; +10\n"
    //);
    //ASSERT_THROW(reader.readStatementList(ss5), MalformedStatementListException);

    //std::stringstream ss6(
    //    "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
    //    "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
    //    "\n"
    //    "Solde en fin de période;;;; +10\n"
    //    "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
    //    "31 / 12 / 19; XXX ; Wrong statement;; notADouble; Debit must be a double;\n"
    //    "Solde en début de période;;;; +10\n"
    //);
    //ASSERT_THROW(reader.readStatementList(ss6), MalformedStatementListException);
}

TEST(CaisseEpargneStatementReader, readStatement_shouldFailWithInvalidBalanceDelta)
{
    CaisseEpargneStatementReader reader;
    std::stringstream ss(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; +60\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "31 / 12 / 19; XXX ; Libellé débit; -50, 00;; Détails débit;\n"
        "03 / 12 / 19; XXX ; Libellé crédit;; +100, 00; Détails crédit;\n"
        "Solde en début de période;;;; -10\n"
    );
    ASSERT_THROW(reader.readStatementList(ss), MalformedStatementListException);
}

TEST(CaisseEpargneStatementReader, readStatement_shouldWorkWithValidData)
{
    CaisseEpargneStatementReader reader;
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
    StatementList statements = reader.readStatementList(ss);

    EXPECT_EQ("00000", statements.bankId);
    EXPECT_EQ("11111", statements.counterId);
    EXPECT_EQ("01234567890", statements.accountNumber);
    EXPECT_EQ(2, statements.items.size());
    checkStatement(statements.items[0], "2019/12/03", 100, "Libellé crédit", "Détails crédit", Currency::Euro);
    checkStatement(statements.items[1], "2019/12/31", -50, "Libellé débit", "Détails débit", Currency::Euro);
}


TEST(CaisseEpargneStatementReader, readStatement_shouldWorkWithEmptyStatements)
{
    CaisseEpargneStatementReader reader;
    std::stringstream ss(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de début de téléchargement : 01/12/2019;Date de fin de téléchargement : 31/12/2019;\n"
        "Numéro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de période;;;; +10\n"
        "Date; Numéro d'opération;Libellé;Débit;Crédit;Détail;\n"
        "Solde en début de période;;;; +10\n"
    );
    StatementList statements = reader.readStatementList(ss);

    EXPECT_EQ("00000", statements.bankId);
    EXPECT_EQ("11111", statements.counterId);
    EXPECT_EQ("01234567890", statements.accountNumber);
    EXPECT_EQ(0, statements.items.size());
}
