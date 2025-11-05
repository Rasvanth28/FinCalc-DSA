#pragma once
#include <vector>
#include <string>

struct Loan
{
    std::string name;
    double principal;
    double rate;
    int years;

    // default compare by name (like Deposit)
    bool operator==(const Loan &other) const
    {
        return name == other.name;
    }
    bool operator<(const Loan &other) const
    {
        return name < other.name;
    }
};

namespace LoanBy
{
    inline bool byName(const Loan &a, const Loan &b) { return a.name < b.name; }
    inline bool byPrincipal(const Loan &a, const Loan &b) { return a.principal < b.principal; }
    inline bool byRate(const Loan &a, const Loan &b) { return a.rate < b.rate; }
    inline bool byYears(const Loan &a, const Loan &b) { return a.years < b.years; }
}

namespace Loans
{
    void showMenu();
    double calcSimple(const Loan &loan);
    double calcCompound(const Loan &loan);
    double calcEMI(const Loan &loan);
    void displayAll(const std::vector<Loan> &loans);
}
