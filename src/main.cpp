#include <iostream>
#include <string>
#include "utilities.hpp"
#include "loans.hpp"

static void printMainMenu()
{
    std::cout << "=== FinCalc-DSA ===\n"
              << "1) Loans (ROI calc)\n"
              << "2) Deposits (FD/RD)\n"
              << "3) Credit/Debt metrics\n"
              << "4) Exit\n"
              << "Choice: ";
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    while (true)
    {
        printMainMenu();
        int choice;
        if (!(std::cin >> choice))
        {
            Utilities::clearStdin();
            std::cout << "Invalid input. Try again.\n";
            continue;
        }

        switch (choice)
        {
        case 1:
            Loans::showMenu();
            break;
        case 2:
            Deposits::showMenu();
            break;
        case 3:
            std::cout << "Credit/Debit module placeholder.\n";
            // Need to implement
            break;
        case 4:
            std::cout << "Bye!\n";
            return 0;
        default:
            std::cout << "Invalid choice. Try again.\n";
        }
    }
}