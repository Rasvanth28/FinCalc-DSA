#pragma once
#include <string>
#include <vector>

struct Deposit
{
    std::string name;
    double amount;
    double rate;
    int months;

    bool operator==(const Deposit &other) const
    {
        return name == other.name;
    }
    bool operator<(const Deposit &other) const
    {
        return name < other.name;
    }
};

namespace DepositSort
{
    inline bool byName(const Deposit &a, const Deposit &b) { return a.name < b.name; }
    inline bool byAmount(const Deposit &a, const Deposit &b) { return a.amount < b.amount; }
    inline bool byRate(const Deposit &a, const Deposit &b) { return a.rate < b.rate; }
    inline bool byMonths(const Deposit &a, const Deposit &b) { return a.months < b.months; }
}

namespace Deposits
{
    void showMenu();
    double calcSimple(const Deposit &d);
    double calcMaturity(const Deposit &d);
    void displayAll(const std::vector<Deposit> &list);

}