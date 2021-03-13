#include "User.h"

std::string User::firstname() const {
	return firstname_;
}

std::string User::lastname() const {
	return lastname_;
}

std::string User::email() const {
	return email_;
}

User::User(const std::string& identifier, const std::string& firstname, const std::string& lastname, const std::string& email) :
	identifier_(identifier),
	firstname_(firstname),
	lastname_(lastname),
	email_(email)
{}
