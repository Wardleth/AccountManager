#include "CaisseEpargneTransactionReader.h"

#include "MalformedTransactionListException.h"
#include "UnexpectedCurrencyException.h"

#include "Utils.h"

#include <iomanip>
#include <list>
#include <regex>
#include <sstream>

static constexpr int STATEMENT_COLUMNS_COUNT = 6;
static constexpr int BALANCE_COLUMNS_COUNT = 5;

TransactionData CaisseEpargneTransactionReader::readTransaction(std::istream& is) {
    std::string line;
    std::getline(is, line);
    auto statementData = StringUtils::split(line, ';');
    if (statementData.size() < STATEMENT_COLUMNS_COUNT) {
        throw MalformedTransactionListException();
    }

    std::regex date_time_regex("([0-9]{2})\\s*/\\s*([0-9]{2})\\s*/\\s*([0-9]{2})");
    std::smatch result;
    if (!std::regex_search(statementData[0], result, date_time_regex)) {
        throw MalformedTransactionListException();
    }
    std::istringstream iss(statementData[0]);
    std::tm date = {};
    iss >> std::get_time(&date, "%d/%m/%y");
    date.tm_mday = std::stoi(result[1]);
    date.tm_mon = std::stoi(result[2]) - 1; // tm_mon is 0-based
    date.tm_year = std::stoi(result[3]) + 100; // result[3] is year since 2000, but tm_year expect year since 1900

    if (!statementData[3].empty() && !statementData[4].empty()) {
        throw MalformedTransactionListException();
    }
    double amount = 0.0;
    if (!statementData[3].empty()) {
        amount = extractDouble(statementData[3]);
        if (amount > 0.0) {
            throw MalformedTransactionListException();
        }
    }
    else if (!statementData[4].empty()) {
        amount = extractDouble(statementData[4]);
        if (amount < 0.0) {
            throw MalformedTransactionListException();
        }
    }
    else {
        throw MalformedTransactionListException();
    }

    return TransactionData(date, amount, StringUtils::trim(statementData[2]), StringUtils::trim(statementData[5]));
}

TransactionList CaisseEpargneTransactionReader::readTransactionList(std::istream& is) {
    auto metadata = readMetadata(is);
    
    auto bankCodeData = metadata.find("Code de la banque");
    if (bankCodeData == metadata.end() || bankCodeData->second.size() != 5) {
        throw MalformedTransactionListException();
    }
    auto agencyCodeData = metadata.find("Code de l'agence");
    if (agencyCodeData == metadata.end() || agencyCodeData->second.size() != 5) {
        throw MalformedTransactionListException();
    }
    auto accountNumberData = metadata.find("Numéro de compte");
    if (accountNumberData == metadata.end() || accountNumberData->second.size() != 11) {
        throw MalformedTransactionListException();
    }
    auto currencyData = metadata.find("Devise");
    if (currencyData == metadata.end()) {
        throw MalformedTransactionListException();
    }

    if (auto currency = expectedCurrency_.lock()) {
        if (currencyData->second != currency->code()) {
            throw UnexpectedCurrencyException(currencyData->second);
        }
    }

    std::string tmp;
    while (std::getline(is, tmp) && !StringUtils::startsWith("Solde en fin de période", tmp)) {}
    double endBalance = extractBalance(tmp);

    std::getline(is, tmp); // Skip headers

    std::list<TransactionData> statements;
    while (std::getline(is, tmp) && !StringUtils::startsWith("Solde en début de période", tmp)) {
        std::istringstream iis(tmp);
        statements.push_front(readTransaction(iis));
    }
    double startBalance = extractBalance(tmp);

    ensureValidity(endBalance - startBalance, statements);

    return {
        bankCodeData->second,
        agencyCodeData->second,
        accountNumberData->second,
        startBalance,
        endBalance,
        std::vector<TransactionData>(statements.begin(), statements.end())
    };
}

std::map<std::string, std::string> CaisseEpargneTransactionReader::readMetadata(std::istream& is) {
    std::map<std::string, std::string> metadata;

    std::string firstLine, secondLine;
    std::getline(is, firstLine);
    std::getline(is, secondLine);

    for (auto& token : StringUtils::split(firstLine + secondLine, ';')) {
        auto data = StringUtils::split(token, ':');
        if (data.size() < 2) {
            throw MalformedTransactionListException();
        }
        metadata.insert(std::make_pair(StringUtils::trim(data[0]), StringUtils::trim(data[1])));
    }

    return metadata;
}

double CaisseEpargneTransactionReader::extractDouble(const std::string& valueStr) const {
    std::istringstream iss(valueStr);
    iss.imbue(std::locale("fr_FR.UTF8"));
    double value;
    iss >> value;
    if (iss.fail()) {
        throw MalformedTransactionListException();
    }

    return value;
}

double CaisseEpargneTransactionReader::extractBalance(const std::string& line) const {
    auto endBalanceData = StringUtils::split(line, ';');
    if (endBalanceData.size() < BALANCE_COLUMNS_COUNT) {
        throw MalformedTransactionListException();
    }

    return extractDouble(endBalanceData.back());
}

void CaisseEpargneTransactionReader::ensureValidity(double balanceDelta, const std::list<TransactionData>& statements) const {
    double sum = 0.0;
    for (auto& s : statements) {
        sum += s.amount;
    }

    if (sum != balanceDelta) {
        throw MalformedTransactionListException();
    }
}

CaisseEpargneTransactionReader::CaisseEpargneTransactionReader(std::weak_ptr<Currency> expectedCurrency) :
    expectedCurrency_(expectedCurrency)
{}
