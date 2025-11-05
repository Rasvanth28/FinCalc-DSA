#pragma once
#include <string>
#include <vector>

struct CreditRecord
{
    std::string name;
    double amount;
    double interest;
    int months;
};

namespace Credits
{
    void showMenu();
    void displayAll(const std::vector<CreditRecord> &list);
    void simulateFIFO(const std::vector<CreditRecord> &list);
    void simulateLIFO(const std::vector<CreditRecord> &list);
}