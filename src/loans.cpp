#include "utilities.hpp"
#include "loans.hpp"
#include <iostream>
#include <cmath>

namespace Loans
{

    double calcSimple(const Loan &loan)
    {
        return loan.principal * loan.rate * loan.years / 100.0;
    }

    double calcCompound(const Loan &loan)
    {
        return loan.principal * (std::pow(1 + loan.rate / 100.0, loan.years) - 1);
    }

    double calcEMI(const Loan &loan)
    {
        double monthlyRate = loan.rate / (12 * 100);
        int months = loan.years * 12;
        return (loan.principal * monthlyRate * std::pow(1 + monthlyRate, months)) /
               (std::pow(1 + monthlyRate, months) - 1);
    }

    void displayAll(const std::vector<Loan> &loans)
    {
        std::cout << "\n--- Loans ---\n";
        for (const auto &l : loans)
        {
            std::cout << l.name
                      << " | Principal=" << l.principal
                      << " | Rate=" << l.rate
                      << " | Years=" << l.years
                      << "\n";
        }
    }

    void showMenu()
    {
        std::vector<Loan> loans;

        while (true)
        {
            std::cout << "\n[Loans]\n"
                      << "1) Add loan\n"
                      << "2) View loans\n"
                      << "3) Sort loans\n"
                      << "4) Search loan\n"
                      << "5) Back\n"
                      << "Choice: ";

            int ch;
            if (!(std::cin >> ch))
            {
                Utilities::clearStdin();
                continue;
            }

            if (ch == 1)
            {
                Loan l;
                std::cout << "Name: ";
                std::cin >> l.name;
                std::cout << "Principal: ";
                std::cin >> l.principal;
                std::cout << "Rate (%): ";
                std::cin >> l.rate;
                std::cout << "Years: ";
                std::cin >> l.years;

                loans.push_back(l);

                std::cout << "Simple Interest: " << calcSimple(l) << "\n";
                std::cout << "Compound Interest: " << calcCompound(l) << "\n";
                std::cout << "EMI (monthly): " << calcEMI(l) << "\n";
            }
            else if (ch == 2)
            {
                displayAll(loans);
            }
            else if (ch == 3)
            {
                std::cout << "\nSort by:\n"
                          << "1) Name\n"
                          << "2) Principal\n"
                          << "3) Rate\n"
                          << "4) Years\n"
                          << "Choice: ";
                int s;
                std::cin >> s;

                switch (s)
                {
                case 1:
                    Utilities::bubbleSort(loans, LoanBy::byName);
                    std::cout << "Sorted by name.\n";
                    break;
                case 2:
                    Utilities::bubbleSort(loans, LoanBy::byPrincipal);
                    std::cout << "Sorted by principal.\n";
                    break;
                case 3:
                    Utilities::bubbleSort(loans, LoanBy::byRate);
                    std::cout << "Sorted by rate.\n";
                    break;
                case 4:
                    Utilities::bubbleSort(loans, LoanBy::byYears);
                    std::cout << "Sorted by years.\n";
                    break;
                default:
                    std::cout << "Invalid sort option.\n";
                    break;
                }
            }
            else if (ch == 4)
            {
                if (loans.empty())
                {
                    std::cout << "No loans to search.\n";
                    continue;
                }

                std::cout << "\nSearch by:\n"
                          << "1) Name\n"
                          << "2) Principal\n"
                          << "3) Rate\n"
                          << "4) Years\n"
                          << "Choice: ";
                int s;
                std::cin >> s;

                if (s == 1)
                {
                    std::cout << "Enter name: ";
                    std::string key;
                    std::cin >> key;
                    int idx = Utilities::linearSearch(loans, Loan{key, 0, 0, 0}, LoanBy::byName);
                    if (idx != -1)
                    {
                        const auto &l = loans[idx];
                        std::cout << "Found: " << l.name << " | Principal=" << l.principal
                                  << " | Rate=" << l.rate << " | Years=" << l.years << "\n";
                    }
                    else
                        std::cout << "Loan not found.\n";
                }
                else if (s == 2)
                {
                    std::cout << "Enter principal: ";
                    double p;
                    std::cin >> p;

                    int idx = Utilities::linearSearch(loans, Loan{"", p, 0, 0}, LoanBy::byPrincipal);
                    if (idx != -1)
                    {
                        const auto &l = loans[idx];
                        std::cout << "Found: " << l.name << " | Principal=" << l.principal
                                  << " | Rate=" << l.rate << " | Years=" << l.years << "\n";
                    }
                    else
                        std::cout << "Loan not found.\n";
                }
                else if (s == 3)
                {
                    std::cout << "Enter rate: ";
                    double r;
                    std::cin >> r;

                    int idx = Utilities::linearSearch(loans, Loan{"", 0, r, 0}, LoanBy::byRate);
                    if (idx != -1)
                    {
                        const auto &l = loans[idx];
                        std::cout << "Found: " << l.name << " | Principal=" << l.principal
                                  << " | Rate=" << l.rate << " | Years=" << l.years << "\n";
                    }
                    else
                        std::cout << "Loan not found.\n";
                }
                else if (s == 4)
                {
                    std::cout << "Enter years: ";
                    int y;
                    std::cin >> y;

                    int idx = Utilities::linearSearch(loans, Loan{"", 0, 0, y}, LoanBy::byYears);
                    if (idx != -1)
                    {
                        const auto &l = loans[idx];
                        std::cout << "Found: " << l.name << " | Principal=" << l.principal
                                  << " | Rate=" << l.rate << " | Years=" << l.years << "\n";
                    }
                    else
                        std::cout << "Loan not found.\n";
                }
                else
                {
                    std::cout << "Invalid search option.\n";
                }
            }
            else
            {
                break;
            }
        }
    }
}
