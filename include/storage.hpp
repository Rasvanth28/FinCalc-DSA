#pragma once
#include <vector>
#include <string>
#include "deposits.hpp"
#include "credit.hpp"
#include "loans.hpp"

namespace Storage
{
    bool saveDeposits(const std::vector<Deposit> &deps, const std::string &path);
    bool loadDeposits(std::vector<Deposit> &deps, const std::string &path);

    bool saveCredits(const std::vector<CreditRecord> &creds, const std::string &path);
    bool loadCredits(std::vector<CreditRecord> &creds, const std::string &path);

    bool saveLoans(const std::vector<Loan> &loans, const std::string &path);
    bool loadLoans(std::vector<Loan> &loans, const std::string &path);
}
