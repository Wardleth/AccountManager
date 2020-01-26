#include "caisseepargnestatementreader.h"

#include "malformedstatementlistexception.h"

#include "utils.h"

#include <iomanip>
#include <list>
#include <regex>
#include <sstream>

static constexpr int STATEMENT_COLUMNS_COUNT = 6;
static constexpr int BALANCE_COLUMNS_COUNT = 5;

Statement CaisseEpargneStatementReader::readStatement(std::istream& is, Currency currency) {
    std::string line;
    std::getline(is, line);
    auto statementData = StringUtils::split(line, ';');
    if (statementData.size() < STATEMENT_COLUMNS_COUNT) {
        throw MalformedStatementListException();
    }

    std::regex date_time_regex("([0-9]{2}) / ([0-9]{2}) / ([0-9]{2})");
    std::smatch result;
    if (!std::regex_match(statementData[0], date_time_regex)) {
        throw MalformedStatementListException();
    }
    std::istringstream iss(statementData[0]);
    std::tm date = {};
    iss >> std::get_time(&date, "%d / %m / %y");
    if (iss.fail()) {
        throw MalformedStatementListException();
    }

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

    return Statement(date, amount, StringUtils::trim(statementData[2]), StringUtils::trim(statementData[5]), currency);
}

StatementList CaisseEpargneStatementReader::readStatementList(std::istream& is) {
    auto metadata = readMetadata(is);
    
    auto bankCodeData = metadata.find("Code de la banque");
    if (bankCodeData == metadata.end() || bankCodeData->second.size() != 5) {
        throw MalformedStatementListException();
    }
    auto counterCodeData = metadata.find("Code de l'agence");
    if (counterCodeData == metadata.end() || counterCodeData->second.size() != 5) {
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
    Currency currency = stringToCurrency(currencyData->second);

    std::string tmp;
    while (std::getline(is, tmp) && !StringUtils::startsWith("Solde en fin de période", tmp)) {}
    double endBalance = extractBalance(tmp);

    std::getline(is, tmp); // Skip headers

    std::list<Statement> statements;
    while (std::getline(is, tmp) && !StringUtils::startsWith("Solde en début de période", tmp)) {
        std::istringstream iis(tmp);
        statements.push_front(readStatement(iis, currency));
    }
    double startBalance = extractBalance(tmp);

    ensureValidity(endBalance - startBalance, statements);

    return {
        bankCodeData->second,
        counterCodeData->second,
        accountNumberData->second,
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
    char* end = nullptr;
    double value = std::strtod(valueStr.c_str(), &end);
    if (end == nullptr || strcmp(valueStr.c_str(), end) == 0) {
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

