#include "loans.hpp"
#include <iostream>
#include <cmath>

namespace Loans {

    double calcSimple(const Loan& loan) {
        return loan.principal * loan.rate * loan.years / 100.0;
    }

    double calcCompound(const Loan& loan) {
        return loan.principal * (pow(1 + loan.rate / 100.0, loan.years) - 1);
    }

    double calcEMI(const Loan& loan) {
        double monthlyRate = loan.rate / (12 * 100);
        int months = loan.years * 12;
        return (loan.principal * monthlyRate * pow(1 + monthlyRate, months)) /
               (pow(1 + monthlyRate, months) - 1);
    }

    void displayAll(const std::vector<Loan>& loans) {
        std::cout << "\n--- Loan Details ---\n";
        for (const auto& l : loans)
            std::cout << l.name << ": Principal=" << l.principal
                      << ", Rate=" << l.rate
                      << ", Years=" << l.years << '\n';
    }

    void showMenu() {
        std::vector<Loan> loans;
        while (true) {
            std::cout << "\n1) Add Loan\n2) View Loans\n3) Back\nChoice: ";
            int ch;
            std::cin >> ch;
            if (ch == 1) {
                Loan l;
                std::cout << "Name: "; std::cin >> l.name;
                std::cout << "Principal: "; std::cin >> l.principal;
                std::cout << "Rate: "; std::cin >> l.rate;
                std::cout << "Years: "; std::cin >> l.years;
                loans.push_back(l);

                std::cout << "Simple Interest: " << calcSimple(l) << '\n';
                std::cout << "Compound Interest: " << calcCompound(l) << '\n';
                std::cout << "EMI (Monthly): " << calcEMI(l) << "\n";
            } else if (ch == 2) {
                displayAll(loans);
            } else break;
        }
    }
}
