#pragma once
#include <string>
#include <vector>

struct CreditRecord
{
    std::string name;
    double amount;   // principal / outstanding
    double interest; // percentage
    int months;      // tenure
};

namespace CreditSort
{
    inline bool byName(const CreditRecord &a, const CreditRecord &b) { return a.name < b.name; }
    inline bool byAmount(const CreditRecord &a, const CreditRecord &b) { return a.amount < b.amount; }
    inline bool byInterest(const CreditRecord &a, const CreditRecord &b) { return a.interest < b.interest; }
    inline bool byMonths(const CreditRecord &a, const CreditRecord &b) { return a.months < b.months; }
}

namespace Credits
{
    void showMenu();
}
