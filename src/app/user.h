#pragma once

#include "Account.h"

#include <map>

class User
{
public:
	std::string firstname() const;
	std::string lastname() const;
	std::string email() const;

	User(const std::string& identifier, const std::string& firstname, const std::string& lastname, const std::string& email);
	User(const User&) = delete;
	User& operator=(const User&) = delete;

private:
	std::string identifier_; // TODO : which form ?
	std::string firstname_;
	std::string lastname_;
	std::string email_;
	std::vector<std::string> ownedAccounts_;
};
