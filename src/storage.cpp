#include "storage.hpp"
#include <fstream>
#include <iostream>

namespace Storage
{
    bool saveDeposits(const std::vector<Deposit> &deps, const std::string &path)
    {
        std::ofstream out(path);
        if (!out)
            return false;
        for (const auto &d : deps)
        {
            out << d.name << " "
                << d.amount << " "
                << d.rate << " "
                << d.months << "\n";
        }
        return true;
    }

    bool loadDeposits(std::vector<Deposit> &deps, const std::string &path)
    {
        std::ifstream in(path);
        if (!in)
            return false;
        deps.clear();
        Deposit d;
        while (in >> d.name >> d.amount >> d.rate >> d.months)
        {
            deps.push_back(d);
        }
        return true;
    }

    bool saveCredits(const std::vector<CreditRecord> &creds, const std::string &path)
    {
        std::ofstream out(path);
        if (!out)
            return false;
        for (const auto &c : creds)
        {
            out << c.name << " "
                << c.amount << " "
                << c.interest << " "
                << c.months << "\n";
        }
        return true;
    }

    bool loadCredits(std::vector<CreditRecord> &creds, const std::string &path)
    {
        std::ifstream in(path);
        if (!in)
            return false;
        creds.clear();
        CreditRecord c;
        while (in >> c.name >> c.amount >> c.interest >> c.months)
        {
            creds.push_back(c);
        }
        return true;
    }

    bool saveLoans(const std::vector<Loan> &loans, const std::string &path)
    {
        std::ofstream out(path);
        if (!out)
            return false;
        for (const auto &l : loans)
        {
            out << l.name << " "
                << l.principal << " "
                << l.rate << " "
                << l.years << "\n";
        }
        return true;
    }

    bool loadLoans(std::vector<Loan> &loans, const std::string &path)
    {
        std::ifstream in(path);
        if (!in)
            return false;
        loans.clear();
        Loan l;
        while (in >> l.name >> l.principal >> l.rate >> l.years)
        {
            loans.push_back(l);
        }
        return true;
    }
}
