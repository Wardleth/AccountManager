#include "caisseepargnestatementreader.h"

#include "malformedstatementlistexception.h"
#include "unexpectedcurrencyexception.h"

#include "utils.h"

#include <iomanip>
#include <list>
#include <regex>
#include <sstream>

static constexpr int STATEMENT_COLUMNS_COUNT = 6;
static constexpr int BALANCE_COLUMNS_COUNT = 5;

Statement CaisseEpargneStatementReader::readStatement(std::istream& is) {
    std::string line;
    std::getline(is, line);
    auto statementData = StringUtils::split(line, ';');
    if (statementData.size() < STATEMENT_COLUMNS_COUNT) {
        throw MalformedStatementListException();
    }

    std::regex date_time_regex("([0-9]{2})\\s*/\\s*([0-9]{2})\\s*/\\s*([0-9]{2})");
    std::smatch result;
    if (!std::regex_search(statementData[0], result, date_time_regex)) {
        throw MalformedStatementListException();
    }
    std::istringstream iss(statementData[0]);
    std::tm date = {};
    iss >> std::get_time(&date, "%d/%m/%y");
    date.tm_mday = std::stoi(result[1]);
    date.tm_mon = std::stoi(result[2]) - 1; // tm_mon is 0-based
    date.tm_year = std::stoi(result[3]) + 100; // result[3] is year since 2000, but tm_year expect year since 1900

    if (!statementData[3].empty() && !statementData[4].empty()) {
        throw MalformedStatementListException();
    }
    double amount = 0.0;
    if (!statementData[3].empty()) {
        amount = extractDouble(statementData[3]);
        if (amount > 0.0) {
            throw MalformedStatementListException();
        }
    }
    else if (!statementData[4].empty()) {
        amount = extractDouble(statementData[4]);
        if (amount < 0.0) {
            throw MalformedStatementListException();
        }
    }
    else {
        throw MalformedStatementListException();
    }

    return Statement(date, amount, StringUtils::trim(statementData[2]), StringUtils::trim(statementData[5]));
}

StatementList CaisseEpargneStatementReader::readStatementList(std::istream& is) {
    auto metadata = readMetadata(is);
    
    auto bankCodeData = metadata.find("Code de la banque");
    if (bankCodeData == metadata.end() || bankCodeData->second.size() != 5) {
        throw MalformedStatementListException();
    }
    auto agencyCodeData = metadata.find("Code de l'agence");
    if (agencyCodeData == metadata.end() || agencyCodeData->second.size() != 5) {
        throw MalformedStatementListException();
    }
    auto accountNumberData = metadata.find("Numéro de compte");
    if (accountNumberData == metadata.end() || accountNumberData->second.size() != 11) {
        throw MalformedStatementListException();
    }
    auto currencyData = metadata.find("Devise");
    if (currencyData == metadata.end()) {
        throw MalformedStatementListException();
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

    std::list<Statement> statements;
    while (std::getline(is, tmp) && !StringUtils::startsWith("Solde en début de période", tmp)) {
        std::istringstream iis(tmp);
        statements.push_front(readStatement(iis));
    }
    double startBalance = extractBalance(tmp);

    ensureValidity(endBalance - startBalance, statements);

    return {
        bankCodeData->second,
        agencyCodeData->second,
        accountNumberData->second,
        startBalance,
        endBalance,
        std::vector<Statement>(statements.begin(), statements.end())
    };
}

std::map<std::string, std::string> CaisseEpargneStatementReader::readMetadata(std::istream& is) {
    std::map<std::string, std::string> metadata;

    std::string firstLine, secondLine;
    std::getline(is, firstLine);
    std::getline(is, secondLine);

    for (auto& token : StringUtils::split(firstLine + secondLine, ';')) {
        auto data = StringUtils::split(token, ':');
        if (data.size() < 2) {
            throw MalformedStatementListException();
        }
        metadata.insert(std::make_pair(StringUtils::trim(data[0]), StringUtils::trim(data[1])));
    }

    return metadata;
}

double CaisseEpargneStatementReader::extractDouble(const std::string& valueStr) const {
    std::istringstream iss(valueStr);
    iss.imbue(std::locale("fr_FR.UTF8"));
    double value;
    iss >> value;
    if (iss.fail()) {
        throw MalformedStatementListException();
    }

    return value;
}

double CaisseEpargneStatementReader::extractBalance(const std::string& line) const {
    auto endBalanceData = StringUtils::split(line, ';');
    if (endBalanceData.size() < BALANCE_COLUMNS_COUNT) {
        throw MalformedStatementListException();
    }

    return extractDouble(endBalanceData.back());
}

void CaisseEpargneStatementReader::ensureValidity(double balanceDelta, const std::list<Statement>& statements) const {
    double sum = 0.0;
    for (auto& s : statements) {
        sum += s.amount();
    }

    if (sum != balanceDelta) {
        throw MalformedStatementListException();
    }
}

CaisseEpargneStatementReader::CaisseEpargneStatementReader(std::weak_ptr<Currency> expectedCurrency) :
    expectedCurrency_(expectedCurrency)
{}
