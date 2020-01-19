#include "caisseepargnestatementreader.h"

#include "invalidstatementlistexception.h"

#include "utils.h"

#include <list>
#include <sstream>
#include <iomanip>

Statement CaisseEpargneStatementReader::readStatement(std::istream &is, Currency currency) {
    std::string line;
    std::getline(is, line);
    auto statementData = StringUtils::split(line, ';');
    if (statementData.size() < 6) {
        throw InvalidStatementListException();
    }
    std::istringstream iss(statementData[0]);
    std::tm date; iss >> std::get_time(&date, "%d / %m / %y");
    if (!statementData[3].empty() && !statementData[4].empty()) {
        throw InvalidStatementListException();
    }
    double amount = 0.0;
    if (!statementData[3].empty()) {
        amount = std::strtod(statementData[3].c_str(), nullptr);
    }
    else if (!statementData[4].empty()) {
        amount = std::strtod(statementData[4].c_str(), nullptr);
    }
    else {
        throw InvalidStatementListException();
    }
    return Statement(date, amount, StringUtils::trim(statementData[2]), StringUtils::trim(statementData[5]), currency);
}

StatementList CaisseEpargneStatementReader::readStatementList(std::istream& is) {
    auto metadata = readMetadata(is);
    
    auto bankCodeData = metadata.find("Code de la banque");
    if (bankCodeData == metadata.end() || bankCodeData->second.size() != 5) {
        throw InvalidStatementListException();
    }
    auto counterCodeData = metadata.find("Code de l'agence");
    if (counterCodeData == metadata.end() || counterCodeData->second.size() != 5) {
        throw InvalidStatementListException();
    }
    auto accountNumberData = metadata.find("Numéro de compte");
    if (accountNumberData == metadata.end() || accountNumberData->second.size() != 11) {
        throw InvalidStatementListException();
    }
    auto currencyData = metadata.find("Devise");
    if (currencyData == metadata.end()) {
        throw InvalidStatementListException();
    }
    Currency currency = stringToCurrency(currencyData->second);

    std::string tmp;
    while (std::getline(is, tmp) && !StringUtils::startsWith("Solde en fin de période", tmp)) {}
    double endBalance = extractBalance(tmp);

    std::getline(is, tmp); // Skip headers
    // TODO: check headers have expected columns count (6)

    std::list<Statement> statements;
    while (std::getline(is, tmp) && !StringUtils::startsWith("Solde en début de période", tmp)) {
        std::istringstream iis(tmp);
        statements.push_front(readStatement(iis, currency));
    }
    double startBalance = extractBalance(tmp);

    // TOTO: check balance delta against statements

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
            throw InvalidStatementListException();
        }
        metadata.insert(std::make_pair(StringUtils::trim(data[0]), StringUtils::trim(data[1])));
    }

    return metadata;
}

double CaisseEpargneStatementReader::extractBalance(const std::string& line) {
    auto endBalanceData = StringUtils::split(line, ';');
    if (endBalanceData.size() < 2) {
        throw InvalidStatementListException();
    }
    return std::strtod(endBalanceData.back().c_str(), nullptr);
}

