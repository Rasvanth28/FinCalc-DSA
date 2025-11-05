#include "utilities.hpp"
#include "ds_linked_list.hpp"
#include <iostream>

namespace Deposits
{

    double calcSimple(const Deposit &d)
    {
        // simple interest for months
        double years = d.months / 12.0;
        return d.amount * d.rate * years / 100.0;
    }

    double calcMaturity(const Deposit &d)
    {
        // for now same as simple; you can change to compounding later
        return d.amount + calcSimple(d);
    }

    void displayAll(const std::vector<Deposit> &list)
    {
        std::cout << "\n--- Deposits ---\n";
        for (const auto &d : list)
        {
            std::cout << d.name
                      << " | Amt=" << d.amount
                      << " | Rate=" << d.rate
                      << " | Tenure=" << d.months << " months\n";
        }
    }

    void showMenu()
    {
        std::vector<Deposit> deposits;
        // linked-list for recent actions
        DSLinkedList<std::string> recent;
        int currentSortMode = 0;
        while (true)
        {
            std::cout << "\n[Deposits]\n"
                      << "1) Add deposit\n"
                      << "2) View deposits\n"
                      << "3) Sort deposits\n"
                      << "4) Search deposit\n"
                      << "5) Show recent actions\n"
                      << "6) Back\n"
                      << "Choices: ";

            int ch;
            if (!(std::cin >> ch))
            {
                Utilities::clearStdin();
                continue;
            }
            if (ch == 1)
            {
                Deposit d;
                std::cout << "Name: ";
                std::cin >> d.name;
                std::cout << "Amount: ";
                std::cin >> d.amount;
                std::cout << "Rate (%) ";
                std::cin >> d.rate;
                std::cout << "Tenure (months): ";
                std::cin >> d.months;
                deposits.push_back(d);

                currentSortMode = 0;

                double mat = calcMaturity(d);
                std::cout << "Maturity value: " << mat << "\n";

                recent.pushFront("Added deposit: " + d.name);
            }
            else if (ch == 2)
            {
                displayAll(deposits);
            }
            else if (ch == 3)
            {
                std::cout << "\nSort by:\n"
                          << "1) Name\n"
                          << "2) Amount\n"
                          << "3) Rate\n"
                          << "4) Months\n"
                          << "Choice: ";
                int s;
                std::cin >> s;

                switch (s)
                {
                case 1:
                    Utilities::insertionSort(deposits, DepositSort::byName);
                    std::cout << "Sorted by name.\n";
                    break;
                case 2:
                    Utilities::insertionSort(deposits, DepositSort::byAmount);
                    std::cout << "Sorted by amount.\n";
                    break;
                case 3:
                    Utilities::insertionSort(deposits, DepositSort::byRate);
                    std::cout << "Sorted by rate.\n";
                    break;
                case 4:
                    Utilities::insertionSort(deposits, DepositSort::byMonths);
                    std::cout << "Sorted by months.\n";
                    break;
                default:
                    std::cout << "Invalid sort options.\n";
                    break;
                }

                currentSortMode = s;
                recent.pushFront("Sorted deposits");
            }
            else if (ch == 4)
            {
                if (deposits.empty())
                {
                    std::cout << "No deposits to search.\n";
                    continue;
                }

                std::cout << "\nSearch by:\n"
                          << "1) Name\n"
                          << "2) Amount\n"
                          << "3) Rate\n"
                          << "4) Months\n"
                          << "Choice: ";
                int s;
                std::cin >> s;

                if (s == 1)
                {
                    std::cout << "Enter name: ";
                    std::string key;
                    std::cin >> key;

                    if (currentSortMode != 1)
                    {
                        Utilities::insertionSort(deposits, DepositSort::byName);
                        currentSortMode = 1;
                    }

                    Deposit temp{key, 0, 0, 0};
                    int index = Utilities::binarySearch(deposits, temp, DepositSort::byName);
                    if (index != -1)
                    {
                        const auto &d = deposits[index];
                        std::cout << "Found: " << d.name << " | Amt=" << d.amount
                                  << " | Rate=" << d.rate << " | Months=" << d.months << "\n";
                    }
                    else
                        std::cout << "Deposit not found.\n";
                }
                else if (s == 2)
                {
                    std::cout << "Enter amount: ";
                    double amt;
                    std::cin >> amt;
                    if (currentSortMode != 2)
                    {
                        Utilities::insertionSort(deposits, DepositSort::byAmount);
                        currentSortMode = 2;
                    }

                    Deposit temp{"", amt, 0, 0};
                    int index = Utilities::binarySearch(deposits, temp, DepositSort::byAmount);
                    if (index != -1)
                    {
                        const auto &d = deposits[index];
                        std::cout << "Found: " << d.name << " | Amt=" << d.amount
                                  << " | Rate=" << d.rate << " | Months=" << d.months << "\n";
                    }
                    else
                        std::cout << "Deposit not found.\n";
                }
                else if (s == 3)
                {
                    std::cout << "Enter rate: ";
                    double r;
                    std::cin >> r;

                    if (currentSortMode != 3)
                    {
                        Utilities::insertionSort(deposits, DepositSort::byRate);
                        currentSortMode = 3;
                    }

                    Deposit temp{"", 0, r, 0};
                    int index = Utilities::binarySearch(deposits, temp, DepositSort::byRate);
                    if (index != -1)
                    {
                        const auto &d = deposits[index];
                        std::cout << "Found: " << d.name << " | Amt=" << d.amount
                                  << " | Rate=" << d.rate << " | Months=" << d.months << "\n";
                    }
                    else
                        std::cout << "Deposit not found.\n";
                }
                else if (s == 4)
                {
                    std::cout << "Enter months: ";
                    int m;
                    std::cin >> m;

                    if (currentSortMode != 4)
                    {
                        Utilities::insertionSort(deposits, DepositSort::byMonths);
                        currentSortMode = 4;
                    }

                    Deposit temp{"", 0, 0, m};
                    int index = Utilities::binarySearch(deposits, temp, DepositSort::byMonths);
                    if (index != -1)
                    {
                        const auto &d = deposits[index];
                        std::cout << "Found: " << d.name << " | Amt=" << d.amount
                                  << " | Rate=" << d.rate << " | Months=" << d.months << "\n";
                    }
                    else
                        std::cout << "Deposit not found.\n";
                }
                else
                {
                    std::cout << "Invalid search option.\n";
                }

                recent.pushFront("Searched deposits");
            }
            else if (ch == 5)
            {
                std::cout << "\nRecent actions:\n";
                std::string action;
                while (recent.popFront(action))
                    std::cout << action << "\n";
            }
            else
                break;
        }
    }

}