#pragma once
#include <vector>
#include <string>

struct Loan {
    std::string name;
    double principal;
    double rate;
    int years;
};

namespace Loans {
    void showMenu();
    double calcSimple(const Loan& loan);
    double calcCompound(const Loan& loan);
    double calcEMI(const Loan& loan);
    void displayAll(const std::vector<Loan>& loans);
    void sortLoansByPrincipal(std::vector<Loan>& loans);
    void searchLoanByName(const std::vector<Loan>& loans);
}
