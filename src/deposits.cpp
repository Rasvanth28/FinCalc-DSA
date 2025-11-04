#include "utilities.hpp"
#include "ds_linked_list.hpp"
#include <iostream>
#include <cmath>

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
        while (true)
        {
            std::cout << "\n[Deposits]\n"
                      << "1) Add deposit\n"
                      << "2) View deposits\n"
                      << "3) Sort deposits (by amount)\n"
                      << "4) Search deposit (by name)\n"
                      << "5) Show recent actions (LL)\n"
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
                std::cout << "Amoount: ";
                std::cin >> d.amount;
                std::cout << "Rate (%) ";
                std::cin >> d.rate;
                std::cout << "Tenure (monhts): ";
                std::cin >> d.months;
                deposits.push_back(d);

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
                Utilities::insertionSort(deposits);
                std::cout << "Sorted by amount (ascending).\n";
            }
            else if (ch == 4)
            {
                std::cout << "Enter name to search: ";
                std::string key;
                std::cin >> key;
                Deposit temp{key, 0, 0, 0};
                int index = Utilities::binarySearch(deposits, temp);
                if (index != -1)
                {
                    std::cout << "Found: " << deposits[index].name
                              << " | Amt=" << deposits[index].amount
                              << " | Rate=" << deposits[index].rate
                              << " | Months=" << deposits[index].months << "\n";
                }
                else
                {
                    std::cout << "Deposit not found.\n";
                }
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