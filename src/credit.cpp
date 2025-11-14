#include "credit.hpp"
#include "utilities.hpp"
#include "storage.hpp"
#include "ds_hash.hpp"
#include "ds_linked_list.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

namespace Credits
{
    // display using view order
    void displayAll(const std::vector<CreditRecord> &list,
                    const std::vector<std::size_t> &order)
    {
        std::cout << "\n--- Credit / Debt Records ---\n";
        for (std::size_t idx : order)
        {
            const auto &c = list[idx];
            double total = c.amount + (c.amount * c.interest * c.months / 100.0);
            double monthly = (c.months > 0) ? total / c.months : total;

            std::cout << std::fixed << std::setprecision(2);
            std::cout << c.name
                      << " | Amt=" << c.amount
                      << " | Interest=" << c.interest << "%"
                      << " | Months=" << c.months
                      << " | Monthly=" << monthly
                      << "\n";
        }
    }

    // rebuild non-unique indexes
    void rebuildValueIndexes(
        const std::vector<CreditRecord> &credits,
        HashMultiMap<double> &amountIndex,
        HashMultiMap<double> &interestIndex,
        HashMultiMap<int> &monthsIndex)
    {
        amountIndex.clear();
        interestIndex.clear();
        monthsIndex.clear();
        for (std::size_t i = 0; i < credits.size(); ++i)
        {
            amountIndex.put(credits[i].amount, i);
            interestIndex.put(credits[i].interest, i);
            monthsIndex.put(credits[i].months, i);
        }
    }

    void rebuildAll(std::vector<CreditRecord> &credits,
                    std::vector<std::size_t> &viewOrder,
                    StringIntMap &nameIndex,
                    HashMultiMap<double> &amountIndex,
                    HashMultiMap<double> &interestIndex,
                    HashMultiMap<int> &monthsIndex)
    {
        Utilities::quickSort(credits, CreditSort::byName);

        viewOrder.clear();
        viewOrder.reserve(credits.size());
        for (std::size_t i = 0; i < credits.size(); ++i)
            viewOrder.push_back(i);
        Utilities::quickSortIndices(viewOrder, credits, CreditSort::byName);

        nameIndex = StringIntMap(211);
        for (std::size_t i = 0; i < credits.size(); ++i)
            nameIndex.put(credits[i].name, i);

        rebuildValueIndexes(credits, amountIndex, interestIndex, monthsIndex);
    }

    void showMenu()
    {
        // load
        std::vector<CreditRecord> credits;
        Storage::loadCredits(credits, "data/credits.txt");

        // build
        std::vector<std::size_t> viewOrder;
        StringIntMap nameIndex(211);
        HashMultiMap<double> amountIndex(211);
        HashMultiMap<double> interestIndex(211);
        HashMultiMap<int> monthsIndex(211);
        rebuildAll(credits, viewOrder, nameIndex, amountIndex, interestIndex, monthsIndex);

        DSLinkedList<std::string> recent;

        while (true)
        {
            std::cout << "\n[Credits / Debts]\n"
                      << "1) Add record\n"
                      << "2) View all records\n"
                      << "3) Sort records\n"
                      << "4) Search record\n"
                      << "5) Update record\n"
                      << "6) Delete record\n"
                      << "7) Reset data\n"
                      << "8) Load sample data\n"
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
                CreditRecord c;
                std::cout << "Name: ";
                std::getline(std::cin >> std::ws, c.name); // ⭐ full name with spaces

                std::cout << "Amount: ";
                std::cin >> c.amount;
                std::cout << "Interest (%): ";
                std::cin >> c.interest;
                std::cout << "Months: ";
                std::cin >> c.months;

                credits.push_back(c);
                std::size_t idx = credits.size() - 1;

                // incremental index updates
                nameIndex.put(c.name, idx);
                amountIndex.put(c.amount, idx);
                interestIndex.put(c.interest, idx);
                monthsIndex.put(c.months, idx);

                viewOrder.push_back(idx);

                recent.pushFront("Added credit: " + c.name);
                std::cout << "Record added.\n";
            }
            else if (ch == 2)
            {
                displayAll(credits, viewOrder);
            }
            else if (ch == 3) // sort view
            {
                std::cout << "\nSort by:\n"
                          << "1) Name\n"
                          << "2) Amount\n"
                          << "3) Interest\n"
                          << "4) Months\n"
                          << "Choice: ";
                int s;
                std::cin >> s;
                switch (s)
                {
                case 1:
                    Utilities::quickSortIndices(viewOrder, credits, CreditSort::byName);
                    std::cout << "Sorted by name.\n";
                    break;
                case 2:
                    Utilities::quickSortIndices(viewOrder, credits, CreditSort::byAmount);
                    std::cout << "Sorted by amount.\n";
                    break;
                case 3:
                    Utilities::quickSortIndices(viewOrder, credits, CreditSort::byInterest);
                    std::cout << "Sorted by interest.\n";
                    break;
                case 4:
                    Utilities::quickSortIndices(viewOrder, credits, CreditSort::byMonths);
                    std::cout << "Sorted by months.\n";
                    break;
                default:
                    std::cout << "Invalid sort option.\n";
                    break;
                }
                recent.pushFront("Sorted credits");
            }
            else if (ch == 4) // search
            {
                if (credits.empty())
                {
                    std::cout << "No records to search.\n";
                    continue;
                }

                std::cout << "\nSearch by:\n"
                          << "1) Name\n"
                          << "2) Amount\n"
                          << "3) Interest\n"
                          << "4) Months\n"
                          << "Choice: ";
                int s;
                std::cin >> s;

                if (s == 1)
                {
                    std::cout << "Enter name: ";
                    std::string key;
                    std::getline(std::cin >> std::ws, key); // ⭐ full name with spaces

                    std::size_t idx;
                    if (nameIndex.get(key, idx))
                    {
                        const auto &c = credits[idx];
                        std::cout << "Found: " << c.name
                                  << " | Amount=" << c.amount
                                  << " | Interest=" << c.interest
                                  << " | Months=" << c.months << "\n";
                    }
                    else
                        std::cout << "Record not found.\n";
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
                            const auto &c = credits[idx];
                            std::cout << "Found: " << c.name
                                      << " | Amount=" << c.amount
                                      << " | Interest=" << c.interest
                                      << " | Months=" << c.months << "\n";
                        }
                    }
                    else
                        std::cout << "No records with that amount.\n";
                }
                else if (s == 3)
                {
                    std::cout << "Enter interest (%): ";
                    double r;
                    std::cin >> r;
                    std::vector<std::size_t> matches;
                    if (interestIndex.get(r, matches))
                    {
                        for (auto idx : matches)
                        {
                            const auto &c = credits[idx];
                            std::cout << "Found: " << c.name
                                      << " | Amount=" << c.amount
                                      << " | Interest=" << c.interest
                                      << " | Months=" << c.months << "\n";
                        }
                    }
                    else
                        std::cout << "No records with that interest.\n";
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
                            const auto &c = credits[idx];
                            std::cout << "Found: " << c.name
                                      << " | Amount=" << c.amount
                                      << " | Interest=" << c.interest
                                      << " | Months=" << c.months << "\n";
                        }
                    }
                    else
                        std::cout << "No records with that tenure.\n";
                }
                else
                    std::cout << "Invalid search option.\n";
            }
            else if (ch == 5) // update → rebuild
            {
                std::cout << "Enter name to update: ";
                std::string key;
                std::getline(std::cin >> std::ws, key); // ⭐ full name with spaces

                std::size_t idx;
                if (!nameIndex.get(key, idx))
                {
                    std::cout << "Not found.\n";
                }
                else
                {
                    auto &c = credits[idx];
                    std::cout << "New amount (" << c.amount << "): ";
                    std::cin >> c.amount;
                    std::cout << "New interest (" << c.interest << "): ";
                    std::cin >> c.interest;
                    std::cout << "New months (" << c.months << "): ";
                    std::cin >> c.months;

                    rebuildAll(credits, viewOrder, nameIndex, amountIndex, interestIndex, monthsIndex);
                    recent.pushFront("Updated credit: " + key);
                    std::cout << "Updated.\n";
                }
            }
            else if (ch == 6) // delete → rebuild
            {
                std::cout << "Enter name to delete: ";
                std::string key;
                std::getline(std::cin >> std::ws, key); // ⭐ full name with spaces

                std::size_t idx;
                if (!nameIndex.get(key, idx))
                {
                    std::cout << "Not found.\n";
                }
                else
                {
                    credits.erase(credits.begin() + idx);
                    rebuildAll(credits, viewOrder, nameIndex, amountIndex, interestIndex, monthsIndex);
                    recent.pushFront("Deleted credit: " + key);
                    std::cout << "Deleted.\n";
                }
            }
            else if (ch == 7) // reset
            {
                credits.clear();
                viewOrder.clear();
                nameIndex = StringIntMap(211);
                amountIndex.clear();
                interestIndex.clear();
                monthsIndex.clear();

                std::vector<CreditRecord> empty;
                Storage::saveCredits(empty, "data/credits.txt");

                recent.pushFront("Reset credit data");
                std::cout << "All credit/debt data reset.\n";
            }
            else if (ch == 8) // load sample
            {
                std::vector<CreditRecord> sample;
                Storage::loadCredits(sample, "data/credits_sample.txt");
                credits = sample;
                rebuildAll(credits, viewOrder, nameIndex, amountIndex, interestIndex, monthsIndex);
                recent.pushFront("Loaded sample credits");
                std::cout << "Sample credit/debt data loaded.\n";
            }
            else
            {
                break;
            }
        }

        Storage::saveCredits(credits, "data/credits.txt");
    }

} // namespace Credits
