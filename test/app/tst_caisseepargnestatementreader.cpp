#include <Transaction.h>
#include <Currency.h>
#include <CaisseEpargneTransactionReader.h>
#include <MalformedTransactionListException.h>
#include <UnexpectedCurrencyException.h>

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <sstream>
#include <memory>

using namespace testing;


void checkTransaction(
    const TransactionData& s,
    const std::string& expectedDate,
    double expectedAmount,
    const std::string& expectedLabel,
    const std::string& expectedDetails
) {
    std::tm date = s.date;
    char buffer[255]; std::strftime(buffer, sizeof(buffer), "%Y/%m/%d", &date);
    EXPECT_EQ(expectedDate, buffer);
    EXPECT_DOUBLE_EQ(expectedAmount, s.amount);
    EXPECT_EQ(expectedLabel, s.label);
    EXPECT_EQ(expectedDetails, s.details);
}

class CaisseEpargneStatementReaderTest : public testing::Test
{
public:
    CaisseEpargneStatementReaderTest() : 
        testCurrency_(std::make_shared<Currency>("EUR", "Euro", "978", "�")),
        reader(testCurrency_)
    {}

private:
    std::shared_ptr<Currency> testCurrency_;

public:
    CaisseEpargneTransactionReader reader;
};

TEST_F(CaisseEpargneStatementReaderTest, readTransaction_shouldFailWithMissingMetadata)
{
    std::stringstream ss1(
        "Code de l'agence : 11111;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Num�ro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de p�riode;;;; 10\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "Solde en d�but de p�riode;;;; 10\n"
    );
    ASSERT_THROW(reader.readTransactionList(ss1), MalformedTransactionListException);

    std::stringstream ss2(
        "Code de la banque : 00000;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Num�ro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de p�riode;;;; 10\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "Solde en d�but de p�riode;;;; 10\n"
    );
    ASSERT_THROW(reader.readTransactionList(ss2), MalformedTransactionListException);

    std::stringstream ss3(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de p�riode;;;; 10\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "Solde en d�but de p�riode;;;; 10\n"
    );
    ASSERT_THROW(reader.readTransactionList(ss3), MalformedTransactionListException);

    std::stringstream ss4(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Num�ro de compte : 01234567890; Nom du compte : XXX;\n"
        "\n"
        "Solde en fin de p�riode;;;; 10\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "Solde en d�but de p�riode;;;; 10\n"
    );
    ASSERT_THROW(reader.readTransactionList(ss4), MalformedTransactionListException);

    std::stringstream ss5(
        "Code de la banque : 00000;Code de l'agence : 11111;\n"
        "Num�ro de compte : 01234567890; Devise: EUR;\n"
        "\n"
        "Solde en fin de p�riode;;;; 10\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "Solde en d�but de p�riode;;;; 10\n"
    );
    ASSERT_NO_THROW(reader.readTransactionList(ss5));
}

TEST_F(CaisseEpargneStatementReaderTest, readTransaction_shouldFailWithMalformedMetadata)
{
    std::stringstream ss(
        "Code de la banque = 00000;Code de l'agence : 11111;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Num�ro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de p�riode;;;; 10\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "Solde en d�but de p�riode;;;; 10\n"
    );
    ASSERT_THROW(reader.readTransactionList(ss), MalformedTransactionListException);
}

TEST_F(CaisseEpargneStatementReaderTest, readTransaction_shouldFailWithMalformedBalanceLine)
{
    std::stringstream ss1(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Num�ro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de p�riode;;; 10\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "Solde en d�but de p�riode;;;; 10\n"
    );
    ASSERT_THROW(reader.readTransactionList(ss1), MalformedTransactionListException);

    std::stringstream ss2(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Num�ro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de p�riode;;;; notANumber\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "Solde en d�but de p�riode;;;; 10\n"
    );
    ASSERT_THROW(reader.readTransactionList(ss2), MalformedTransactionListException);
}


TEST_F(CaisseEpargneStatementReaderTest, readTransaction_shouldFailWithMalformedTransactionDate)
{
    std::stringstream ss1(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Num�ro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de p�riode;;;; +10\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "31/12; XXX ; Libell� d�bit; -50,50;; D�tails d�bit;\n"
        "Solde en d�but de p�riode;;;; +10\n"
    );
    ASSERT_THROW(reader.readTransactionList(ss1), MalformedTransactionListException);

    std::stringstream ss2(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Num�ro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de p�riode;;;; 10\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "31/12/2019; XXX ; Libell� d�bit; -50,50;; D�tails d�bit;\n"
        "Solde en d�but de p�riode;;;; 10\n"
    );
    ASSERT_THROW(reader.readTransactionList(ss2), MalformedTransactionListException);

    std::stringstream ss3(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Num�ro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de p�riode;;;; 10\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "30/02/19; XXX ; Libell� d�bit; -50,50;; D�tails d�bit;\n"
        "Solde en d�but de p�riode;;;; 10\n"
    );
    ASSERT_THROW(reader.readTransactionList(ss3), MalformedTransactionListException);
}

TEST_F(CaisseEpargneStatementReaderTest, readTransaction_shouldFailWithMalformedTransactionAmount)
{
    std::stringstream ss1(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Num�ro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de p�riode;;;; +10\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "31/12/19; XXX ; Wrong statement; +50,50;; Cannot have positive debit;\n"
        "Solde en d�but de p�riode;;;; +10\n"
    );
    ASSERT_THROW(reader.readTransactionList(ss1), MalformedTransactionListException);

    std::stringstream ss2(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Num�ro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de p�riode;;;; 10\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "31/12/19; XXX ; Wrong statement;; -50,50; Cannot have negative credit;\n"
        "Solde en d�but de p�riode;;;; 10\n"
    );
    ASSERT_THROW(reader.readTransactionList(ss2), MalformedTransactionListException);

    std::stringstream ss3(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Num�ro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de p�riode;;;; 10\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "31/12/19; XXX ; Wrong statement; -50,50; 50,50; Cannot have both debit and credit;\n"
        "Solde en d�but de p�riode;;;; 10\n"
    );
    ASSERT_THROW(reader.readTransactionList(ss3), MalformedTransactionListException);

    std::stringstream ss4(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Num�ro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de p�riode;;;; 10\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "31/12/19; XXX ; Wrong statement;;; Must have at least one of debit/credit filled with an amount;\n"
        "Solde en d�but de p�riode;;;; 10\n"
    );
    ASSERT_THROW(reader.readTransactionList(ss4), MalformedTransactionListException);

    std::stringstream ss5(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Num�ro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de p�riode;;;; 10\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "31/12/19; XXX ; Wrong statement; notADouble;; Debit must be a double;\n"
        "Solde en d�but de p�riode;;;; 10\n"
    );
    ASSERT_THROW(reader.readTransactionList(ss5), MalformedTransactionListException);

    std::stringstream ss6(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Num�ro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de p�riode;;;; 10\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "31/12/19; XXX ; Wrong statement;; notADouble; Credit must be a double;\n"
        "Solde en d�but de p�riode;;;; 10\n"
    );
    ASSERT_THROW(reader.readTransactionList(ss6), MalformedTransactionListException);
}

TEST_F(CaisseEpargneStatementReaderTest, readTransaction_shouldFailWithInvalidBalanceDelta)
{
    std::stringstream ss(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Num�ro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de p�riode;;;; 60\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "31/12/19; XXX ; Libell� d�bit; -50,50;; D�tails d�bit;\n"
        "03/12/19; XXX ; Libell� cr�dit;; 100,00; D�tails cr�dit;\n"
        "Solde en d�but de p�riode;;;; -10\n"
    );
    ASSERT_THROW(reader.readTransactionList(ss), MalformedTransactionListException);
}

TEST_F(CaisseEpargneStatementReaderTest, readTransaction_shouldSucceedWithValidData)
{
    std::stringstream ss(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Num�ro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de p�riode;;;; 39,50\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "31/12/19; XXX ; Libell� d�bit; -50,50;; D�tails d�bit;\n"
        "03/12/19; XXX ; Libell� cr�dit;; 100,00; D�tails cr�dit;\n"
        "Solde en d�but de p�riode;;;; -10\n"
    );
    TransactionList statements = reader.readTransactionList(ss);

    EXPECT_EQ("00000", statements.bankId);
    EXPECT_EQ("11111", statements.agencyId);
    EXPECT_EQ("01234567890", statements.accountNumber);
    EXPECT_DOUBLE_EQ(-10.0, statements.startBalance);
    EXPECT_DOUBLE_EQ(39.5, statements.endBalance);
    EXPECT_EQ(2, statements.items.size());
    checkTransaction(statements.items[0], "2019/12/03", 100, "Libell� cr�dit", "D�tails cr�dit");
    checkTransaction(statements.items[1], "2019/12/31", -50.5, "Libell� d�bit", "D�tails d�bit");
}

TEST_F(CaisseEpargneStatementReaderTest, readTransaction_shouldFailWithUnexpectedCurrency)
{
    std::stringstream ss(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Num�ro de compte : 01234567890; Nom du compte : XXX; Devise: CAD;\n"
        "\n"
        "Solde en fin de p�riode;;;; 10\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "Solde en d�but de p�riode;;;; 10\n"
    );
    ASSERT_THROW(reader.readTransactionList(ss), UnexpectedCurrencyException);
}

TEST_F(CaisseEpargneStatementReaderTest, readStatement_shouldSucceedWithEmptyStatements)
{
    std::stringstream ss(
        "Code de la banque : 00000;Code de l'agence : 11111;Date de d�but de t�l�chargement : 01/12/2019;Date de fin de t�l�chargement : 31/12/2019;\n"
        "Num�ro de compte : 01234567890; Nom du compte : XXX; Devise: EUR;\n"
        "\n"
        "Solde en fin de p�riode;;;; 10\n"
        "Date; Num�ro d'op�ration;Libell�;D�bit;Cr�dit;D�tail;\n"
        "Solde en d�but de p�riode;;;; 10\n"
    );
    TransactionList statements = reader.readTransactionList(ss);

    EXPECT_EQ("00000", statements.bankId);
    EXPECT_EQ("11111", statements.agencyId);
    EXPECT_EQ("01234567890", statements.accountNumber);
    EXPECT_DOUBLE_EQ(10.0, statements.startBalance);
    EXPECT_DOUBLE_EQ(10.0, statements.endBalance);
    EXPECT_EQ(0, statements.items.size());
}
