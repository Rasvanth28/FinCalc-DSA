#include "utilities.hpp"
#include "ds_linked_list.hpp"
#include "ds_hash.hpp"
#include "storage.hpp"
#include "deposits.hpp"
#include <iostream>
#include <vector>
#include <string>

namespace Deposits
{
    // ----- interest helpers -----
    double calcSimple(const Deposit &d)
    {
        double years = d.months / 12.0;
        return d.amount * d.rate * years / 100.0;
    }

    double calcMaturity(const Deposit &d)
    {
        return d.amount + calcSimple(d);
    }

    // ----- display using viewOrder -----
    void displayAll(const std::vector<Deposit> &list,
                    const std::vector<std::size_t> &order)
    {
        std::cout << "\n--- Deposits ---\n";
        for (std::size_t idx : order)
        {
            const auto &d = list[idx];
            std::cout << d.name
                      << " | Amt=" << d.amount
                      << " | Rate=" << d.rate
                      << " | Tenure=" << d.months << " months\n";
        }
    }

    // ----- rebuild non-unique indexes -----
    void rebuildValueIndexes(
        const std::vector<Deposit> &deposits,
        HashMultiMap<double> &amountIndex,
        HashMultiMap<double> &rateIndex,
        HashMultiMap<int> &monthsIndex)
    {
        amountIndex.clear();
        rateIndex.clear();
        monthsIndex.clear();
        for (std::size_t i = 0; i < deposits.size(); ++i)
        {
            amountIndex.put(deposits[i].amount, i);
            rateIndex.put(deposits[i].rate, i);
            monthsIndex.put(deposits[i].months, i);
        }
    }

    // ----- full rebuild (expensive) -----
    void rebuildAll(std::vector<Deposit> &deposits,
                    std::vector<std::size_t> &viewOrder,
                    StringIntMap &nameIndex,
                    HashMultiMap<double> &amountIndex,
                    HashMultiMap<double> &rateIndex,
                    HashMultiMap<int> &monthsIndex)
    {
        // main vector sorted by name only during full rebuilds
        Utilities::quickSort(deposits, DepositSort::byName);

        // rebuild view
        viewOrder.clear();
        viewOrder.reserve(deposits.size());
        for (std::size_t i = 0; i < deposits.size(); ++i)
            viewOrder.push_back(i);
        Utilities::quickSortIndices(viewOrder, deposits, DepositSort::byName);

        // rebuild name hash
        nameIndex = StringIntMap(211);
        for (std::size_t i = 0; i < deposits.size(); ++i)
            nameIndex.put(deposits[i].name, i);

        // rebuild value hashes
        rebuildValueIndexes(deposits, amountIndex, rateIndex, monthsIndex);
    }

    void showMenu()
    {
        // 1) load from file
        std::vector<Deposit> deposits;
        Storage::loadDeposits(deposits, "data/deposits.txt");

        // 2) build all structures once
        std::vector<std::size_t> viewOrder;
        StringIntMap nameIndex(211);
        HashMultiMap<double> amountIndex(211);
        HashMultiMap<double> rateIndex(211);
        HashMultiMap<int> monthsIndex(211);
        rebuildAll(deposits, viewOrder, nameIndex, amountIndex, rateIndex, monthsIndex);

        // recent actions
        DSLinkedList<std::string> recent;

        while (true)
        {
            std::cout << "\n[Deposits]\n"
                      << "1) Add deposit\n"
                      << "2) View deposits\n"
                      << "3) Sort deposits\n"
                      << "4) Search deposit\n"
                      << "5) Show recent actions\n"
                      << "6) Update deposit\n"
                      << "7) Delete deposit\n"
                      << "8) Load sample data\n"
                      << "9) Reset data\n"
                      << "10) Back\n"
                      << "Choices: ";

            int ch;
            if (!(std::cin >> ch))
            {
                Utilities::clearStdin();
                continue;
            }

            if (ch == 1) // CREATE (cheap)
            {
                Deposit d;
                std::cout << "Name: ";
                std::getline(std::cin >> std::ws, d.name); // ⭐ supports spaces

                std::cout << "Amount: ";
                std::cin >> d.amount;
                std::cout << "Rate (%): ";
                std::cin >> d.rate;
                std::cout << "Tenure (months): ";
                std::cin >> d.months;

                deposits.push_back(d);
                std::size_t idx = deposits.size() - 1;

                // incremental index updates (O(1))
                nameIndex.put(d.name, idx);
                amountIndex.put(d.amount, idx);
                rateIndex.put(d.rate, idx);
                monthsIndex.put(d.months, idx);

                // view just appends — we sort only when user asks
                viewOrder.push_back(idx);

                double mat = calcMaturity(d);
                std::cout << "Maturity value: " << mat << "\n";

                recent.pushFront("Added deposit: " + d.name);
            }
            else if (ch == 2) // READ
            {
                displayAll(deposits, viewOrder);
            }
            else if (ch == 3) // SORT (expensive, but user-triggered)
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
                    Utilities::quickSortIndices(viewOrder, deposits, DepositSort::byName);
                    std::cout << "Sorted by name.\n";
                    break;
                case 2:
                    Utilities::quickSortIndices(viewOrder, deposits, DepositSort::byAmount);
                    std::cout << "Sorted by amount.\n";
                    break;
                case 3:
                    Utilities::quickSortIndices(viewOrder, deposits, DepositSort::byRate);
                    std::cout << "Sorted by rate.\n";
                    break;
                case 4:
                    Utilities::quickSortIndices(viewOrder, deposits, DepositSort::byMonths);
                    std::cout << "Sorted by months.\n";
                    break;
                default:
                    std::cout << "Invalid sort options.\n";
                    break;
                }

                recent.pushFront("Sorted deposits");
            }
            else if (ch == 4) // SEARCH
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
                    std::getline(std::cin >> std::ws, key); // ⭐ supports spaces

                    std::size_t idx;
                    if (nameIndex.get(key, idx))
                    {
                        const auto &d = deposits[idx];
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
                    std::vector<std::size_t> matches;
                    if (amountIndex.get(amt, matches))
                    {
                        for (auto idx : matches)
                        {
                            const auto &d = deposits[idx];
                            std::cout << "Found: " << d.name << " | Amt=" << d.amount
                                      << " | Rate=" << d.rate << " | Months=" << d.months << "\n";
                        }
                    }
                    else
                        std::cout << "No deposits with that amount.\n";
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
                            const auto &d = deposits[idx];
                            std::cout << "Found: " << d.name << " | Amt=" << d.amount
                                      << " | Rate=" << d.rate << " | Months=" << d.months << "\n";
                        }
                    }
                    else
                        std::cout << "No deposits with that rate.\n";
                }
                else if (s == 4)
                {
                    std::cout << "Enter months: ";
                    int m;
                    std::cin >> m;
                    std::vector<std::size_t> matches;
                    if (monthsIndex.get(m, matches))
                    {
                        for (auto idx : matches)
                        {
                            const auto &d = deposits[idx];
                            std::cout << "Found: " << d.name << " | Amt=" << d.amount
                                      << " | Rate=" << d.rate << " | Months=" << d.months << "\n";
                        }
                    }
                    else
                        std::cout << "No deposits with that tenure.\n";
                }
                else
                {
                    std::cout << "Invalid search option.\n";
                }

                recent.pushFront("Searched deposits");
            }
            else if (ch == 5) // recent
            {
                std::cout << "\nRecent actions:\n";
                std::string action;
                while (recent.popFront(action))
                    std::cout << action << "\n";
            }
            else if (ch == 6) // UPDATE (rare → rebuild)
            {
                std::cout << "Enter name to update: ";
                std::string key;
                std::getline(std::cin >> std::ws, key); // ⭐ supports spaces

                std::size_t idx;
                if (!nameIndex.get(key, idx))
                {
                    std::cout << "Not found.\n";
                }
                else
                {
                    auto &d = deposits[idx];
                    std::cout << "New amount (" << d.amount << "): ";
                    std::cin >> d.amount;
                    std::cout << "New rate (" << d.rate << "): ";
                    std::cin >> d.rate;
                    std::cout << "New months (" << d.months << "): ";
                    std::cin >> d.months;

                    // full rebuild so all indexes stay correct
                    rebuildAll(deposits, viewOrder, nameIndex, amountIndex, rateIndex, monthsIndex);

                    recent.pushFront("Updated deposit: " + key);
                }
            }
            else if (ch == 7) // DELETE (rare → rebuild)
            {
                std::cout << "Enter name to delete: ";
                std::string key;
                std::getline(std::cin >> std::ws, key); // ⭐ supports spaces

                std::size_t idx;
                if (!nameIndex.get(key, idx))
                {
                    std::cout << "Not found.\n";
                }
                else
                {
                    deposits.erase(deposits.begin() + idx);
                    // rebuild everything to fix shifted indices
                    rebuildAll(deposits, viewOrder, nameIndex, amountIndex, rateIndex, monthsIndex);
                    recent.pushFront("Deleted deposit: " + key);
                    std::cout << "Deleted.\n";
                }
            }
            else if (ch == 8) // load sample → rebuild
            {
                std::vector<Deposit> sample;
                Storage::loadDeposits(sample, "data/deposits_sample.txt");
                deposits = sample;
                rebuildAll(deposits, viewOrder, nameIndex, amountIndex, rateIndex, monthsIndex);
                recent.pushFront("Loaded sample deposits");
                std::cout << "Sample deposits loaded.\n";
            }
            else if (ch == 9) // reset
            {
                deposits.clear();
                viewOrder.clear();
                nameIndex = StringIntMap(211);
                amountIndex.clear();
                rateIndex.clear();
                monthsIndex.clear();
                std::vector<Deposit> empty;
                Storage::saveDeposits(empty, "data/deposits.txt");
                recent.pushFront("Reset deposit data");
                std::cout << "All deposit data reset.\n";
            }
            else
            {
                break;
            }
        }

        // save on exit
        Storage::saveDeposits(deposits, "data/deposits.txt");
    }

} // namespace Deposits
