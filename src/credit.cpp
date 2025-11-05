#include "utilities.hpp"
#include <iostream>
#include <queue>
#include <stack>
#include <iomanip>

namespace Credits
{
    void displayAll(const std::vector<CreditRecord> &list)
    {
        std::cout << "\n--- Credit/Debit Record ---\n";
        for (const auto &c : list)
        {
            double total = c.amount + (c.amount * c.interest * c.months / 100.0);
            double monthly = total / c.months;
            std::cout << std::fixed << std::setprecision(2);
            std::cout << c.name << " | Amt=" << c.amount
                      << "% | Months=" << c.months
                      << " | Monthly=" << monthly << "\n";
        }
    }

    void simulateFIFO(const std::vector<CreditRecord> &list)
    {
        std::queue<CreditRecord> q;
        for (auto &c : list)
            q.push(c);
        std::cout << "\n[FIFO Simulation - pay oldest first]\n";
        while (!q.empty())
        {
            CreditRecord cur = q.front();
            q.pop();
            std::cout << "Paying: " << cur.name
                      << " | Amt=" << cur.amount
                      << " | Interesrt=" << cur.interest << "%\n";
        }
    }

    void simulateLIFO(const std::vector<CreditRecord> &list)
    {
        std::stack<CreditRecord> s;
        for (auto &c : list)
            s.push(c);

        std::cout << "\v[LIFO Simulation - pay newest first]\n";
        while (!s.empty())
        {
            CreditRecord cur = s.top();
            s.pop();
            std::cout << "Paying: " << cur.name
                      << " | Amt=" << cur.amount
                      << " | Interest=" << cur.interest << "%\n";
        }
    }

    void showMenu()
    {
        std::vector<CreditRecord> credits;

        while (true)
        {
            std::cout << "\n[Credits / Debts]\n"
                      << "1) Add record\n"
                      << "2) View all records\n"
                      << "3) FIFO schedule\n"
                      << "4) LIFO schedule\n"
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
                CreditRecord c;
                std::cout << "Name: ";
                std::cin >> c.name;
                std::cout << "Amount: ";
                std::cin >> c.amount;
                std::cout << "Interest (% per month): ";
                std::cin >> c.interest;
                std::cout << "Months: ";
                std::cin >> c.months;
                credits.push_back(c);
            }
            else if (ch == 2)
                displayAll(credits);
            else if (ch == 3)
                simulateFIFO(credits);
            else if (ch == 4)
                simulateLIFO(credits);
            else
                break;
        }
    }
}