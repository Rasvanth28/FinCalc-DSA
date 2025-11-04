#pragma once
#include <string>
#include <vector>

struct Deposit
{
    std::string name;
    double amount;
    double rate;
    int months;

    // for sorting (insertionSort)
    bool operator<(const Deposit &other) const
    {
        return amount < other.amount; // sort by amount
    }

    // for binarySearch
    bool operator==(const Deposit &other) const
    {
        return name == other.name; // search by name
    }
};

namespace Deposits
{
    void showMenu();
    double calcSimple(const Deposit &d);
    double calcMaturity(const Deposit &d);
    void displayAll(const std::vector<Deposit> &list);
}