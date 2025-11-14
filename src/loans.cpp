#include "utilities.hpp"
#include "loans.hpp"
#include "storage.hpp"
#include "ds_hash.hpp"
#include "ds_linked_list.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <string>

namespace Loans
{
    // ----- calc helpers -----
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

    // ----- display -----
    void displayAll(const std::vector<Loan> &loans,
                    const std::vector<std::size_t> &order)
    {
        std::cout << "\n--- Loans ---\n";
        for (std::size_t idx : order)
        {
            const auto &l = loans[idx];
            std::cout << l.name
                      << " | Principal=" << l.principal
                      << " | Rate=" << l.rate
                      << " | Years=" << l.years << "\n";
        }
    }

    // ----- rebuild value indexes -----
    void rebuildValueIndexes(
        const std::vector<Loan> &loans,
        HashMultiMap<double> &principalIndex,
        HashMultiMap<double> &rateIndex,
        HashMultiMap<int> &yearsIndex)
    {
        principalIndex.clear();
        rateIndex.clear();
        yearsIndex.clear();
        for (std::size_t i = 0; i < loans.size(); ++i)
        {
            principalIndex.put(loans[i].principal, i);
            rateIndex.put(loans[i].rate, i);
            yearsIndex.put(loans[i].years, i);
        }
    }

    // ----- full rebuild -----
    void rebuildAll(std::vector<Loan> &loans,
                    std::vector<std::size_t> &viewOrder,
                    StringIntMap &nameIndex,
                    HashMultiMap<double> &principalIndex,
                    HashMultiMap<double> &rateIndex,
                    HashMultiMap<int> &yearsIndex)
    {
        Utilities::quickSort(loans, LoanBy::byName);

        viewOrder.clear();
        viewOrder.reserve(loans.size());
        for (std::size_t i = 0; i < loans.size(); ++i)
            viewOrder.push_back(i);
        Utilities::quickSortIndices(viewOrder, loans, LoanBy::byName);

        nameIndex = StringIntMap(101);
        for (std::size_t i = 0; i < loans.size(); ++i)
            nameIndex.put(loans[i].name, i);

        rebuildValueIndexes(loans, principalIndex, rateIndex, yearsIndex);
    }

    void showMenu()
    {
        // load
        std::vector<Loan> loans;
        Storage::loadLoans(loans, "data/loans.txt");

        // build
        std::vector<std::size_t> viewOrder;
        StringIntMap nameIndex(101);
        HashMultiMap<double> principalIndex(101);
        HashMultiMap<double> rateIndex(101);
        HashMultiMap<int> yearsIndex(101);
        rebuildAll(loans, viewOrder, nameIndex, principalIndex, rateIndex, yearsIndex);

        DSLinkedList<std::string> recent;

        while (true)
        {
            std::cout << "\n[Loans]\n"
                      << "1) Add loan\n"
                      << "2) View loans\n"
                      << "3) Sort loans\n"
                      << "4) Search loan\n"
                      << "5) Update loan\n"
                      << "6) Delete loan\n"
                      << "7) Load sample data\n"
                      << "8) Reset data\n"
                      << "9) Back\n"
                      << "Choice: ";

            int ch;
            if (!(std::cin >> ch))
            {
                Utilities::clearStdin();
                continue;
            }

            if (ch == 1) // add (cheap)
            {
                Loan l;

                std::cout << "Name: ";
                std::getline(std::cin >> std::ws, l.name); // ⭐ FIXED: full name with spaces

                std::cout << "Principal: ";
                std::cin >> l.principal;
                std::cout << "Rate (%): ";
                std::cin >> l.rate;
                std::cout << "Years: ";
                std::cin >> l.years;

                loans.push_back(l);
                std::size_t idx = loans.size() - 1;

                nameIndex.put(l.name, idx);
                principalIndex.put(l.principal, idx);
                rateIndex.put(l.rate, idx);
                yearsIndex.put(l.years, idx);

                viewOrder.push_back(idx);

                std::cout << "Simple Interest: " << calcSimple(l) << "\n";
                std::cout << "Compound Interest: " << calcCompound(l) << "\n";
                std::cout << "EMI (monthly): " << calcEMI(l) << "\n";

                recent.pushFront("Added loan: " + l.name);
            }
            else if (ch == 2)
            {
                displayAll(loans, viewOrder);
            }
            else if (ch == 3) // sort view
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
                    Utilities::quickSortIndices(viewOrder, loans, LoanBy::byName);
                    std::cout << "Sorted by name.\n";
                    break;
                case 2:
                    Utilities::quickSortIndices(viewOrder, loans, LoanBy::byPrincipal);
                    std::cout << "Sorted by principal.\n";
                    break;
                case 3:
                    Utilities::quickSortIndices(viewOrder, loans, LoanBy::byRate);
                    std::cout << "Sorted by rate.\n";
                    break;
                case 4:
                    Utilities::quickSortIndices(viewOrder, loans, LoanBy::byYears);
                    std::cout << "Sorted by years.\n";
                    break;
                default:
                    std::cout << "Invalid sort option.\n";
                    break;
                }

                recent.pushFront("Sorted loans");
            }
            else if (ch == 4) // search
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
                    std::getline(std::cin >> std::ws, key); // ⭐ FIXED

                    std::size_t idx;
                    if (nameIndex.get(key, idx))
                    {
                        const auto &l = loans[idx];
                        std::cout << "Found: " << l.name
                                  << " | Principal=" << l.principal
                                  << " | Rate=" << l.rate
                                  << " | Years=" << l.years << "\n";
                    }
                    else
                        std::cout << "Loan not found.\n";
                }
                else if (s == 2)
                {
                    std::cout << "Enter principal: ";
                    double p;
                    std::cin >> p;
                    std::vector<std::size_t> matches;
                    if (principalIndex.get(p, matches))
                    {
                        for (auto idx : matches)
                        {
                            const auto &l = loans[idx];
                            std::cout << "Found: " << l.name
                                      << " | Principal=" << l.principal
                                      << " | Rate=" << l.rate
                                      << " | Years=" << l.years << "\n";
                        }
                    }
                    else
                        std::cout << "No loans with that principal.\n";
                }
                else if (s == 3)
                {
                    std::cout << "Enter rate: ";
                    double r;
                    std::cin >> r;
                    std::vector<std::size_t> matches;
                    if (rateIndex.get(r, matches))
                    {
                        for (auto idx : matches)
                        {
                            const auto &l = loans[idx];
                            std::cout << "Found: " << l.name
                                      << " | Principal=" << l.principal
                                      << " | Rate=" << l.rate
                                      << " | Years=" << l.years << "\n";
                        }
                    }
                    else
                        std::cout << "No loans with that rate.\n";
                }
                else if (s == 4)
                {
                    std::cout << "Enter years: ";
                    int y;
                    std::cin >> y;
                    std::vector<std::size_t> matches;
                    if (yearsIndex.get(y, matches))
                    {
                        for (auto idx : matches)
                        {
                            const auto &l = loans[idx];
                            std::cout << "Found: " << l.name
                                      << " | Principal=" << l.principal
                                      << " | Rate=" << l.rate
                                      << " | Years=" << l.years << "\n";
                        }
                    }
                    else
                        std::cout << "No loans with that tenure.\n";
                }
                else
                    std::cout << "Invalid search option.\n";
            }
            else if (ch == 5) // update → rebuild
            {
                std::cout << "Enter name to update: ";
                std::string key;
                std::getline(std::cin >> std::ws, key); // ⭐ FIXED

                std::size_t idx;
                if (!nameIndex.get(key, idx))
                {
                    std::cout << "Not found.\n";
                }
                else
                {
                    auto &l = loans[idx];
                    std::cout << "New principal (" << l.principal << "): ";
                    std::cin >> l.principal;
                    std::cout << "New rate (" << l.rate << "): ";
                    std::cin >> l.rate;
                    std::cout << "New years (" << l.years << "): ";
                    std::cin >> l.years;

                    rebuildAll(loans, viewOrder, nameIndex, principalIndex, rateIndex, yearsIndex);
                    recent.pushFront("Updated loan: " + key);
                    std::cout << "Updated.\n";
                }
            }
            else if (ch == 6) // delete → rebuild
            {
                std::cout << "Enter name to delete: ";
                std::string key;
                std::getline(std::cin >> std::ws, key); // ⭐ FIXED

                std::size_t idx;
                if (!nameIndex.get(key, idx))
                {
                    std::cout << "Not found.\n";
                }
                else
                {
                    loans.erase(loans.begin() + idx);
                    rebuildAll(loans, viewOrder, nameIndex, principalIndex, rateIndex, yearsIndex);
                    recent.pushFront("Deleted loan: " + key);
                    std::cout << "Deleted.\n";
                }
            }
            else if (ch == 7) // load sample
            {
                std::vector<Loan> sample;
                Storage::loadLoans(sample, "data/loans_sample.txt");
                loans = sample;
                rebuildAll(loans, viewOrder, nameIndex, principalIndex, rateIndex, yearsIndex);
                recent.pushFront("Loaded sample loans");
                std::cout << "Sample loans loaded.\n";
            }
            else if (ch == 8) // reset
            {
                loans.clear();
                viewOrder.clear();
                nameIndex = StringIntMap(101);
                principalIndex.clear();
                rateIndex.clear();
                yearsIndex.clear();

                std::vector<Loan> empty;
                Storage::saveLoans(empty, "data/loans.txt");

                recent.pushFront("Reset loan data");
                std::cout << "All loan data reset.\n";
            }
            else
            {
                break;
            }
        }

        Storage::saveLoans(loans, "data/loans.txt");
    }

} // namespace Loans
