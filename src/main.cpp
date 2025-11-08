#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>

// your headers
#include "utilities.hpp"
#include "loans.hpp"
#include "deposits.hpp"
#include "credit.hpp"
#include "ds_hash.hpp"
#include "storage.hpp"

// ---------------------------------------------------------
// tiny helpers to make fake records for benchmarking
// ---------------------------------------------------------
static Deposit makeDeposit(int i)
{
    Deposit d;
    d.name = "Deposit_User_" + std::to_string(i);
    d.amount = 1000 + (i % 5000);
    d.rate = 5.0 + (i % 10);
    d.months = 6 + (i % 36);
    return d;
}

static Loan makeLoan(int i)
{
    Loan l;
    l.name = "Loan_User_" + std::to_string(i);
    l.principal = 5000 + (i % 10000);
    l.rate = 7.0 + (i % 5);
    l.years = 1 + (i % 10);
    return l;
}

static CreditRecord makeCredit(int i)
{
    CreditRecord c;
    c.name = "Credit_User_" + std::to_string(i);
    c.amount = 2000 + (i % 4000);
    c.interest = 2.0 + (i % 5); // monthly %
    c.months = 3 + (i % 24);
    return c;
}

// ---------------------------------------------------------
// benchmark: DEPOSITS
// generates N records, builds indexes like your real code,
// times: build, view-sort, insert+rebuild
// writes to data/bench_deposits.csv (append/overwrite)
// ---------------------------------------------------------
static void runDepositBenchmark(const std::vector<int> &sizes)
{
    std::ofstream out("data/bench_deposits.csv");
    if (!out)
    {
        std::cout << "Could not open data/bench_deposits.csv for writing\n";
        return;
    }

    out << "N,build_ms,view_sort_ms,insert_rebuild_ms,space_units\n";

    for (int N : sizes)
    {
        std::vector<Deposit> deposits;
        deposits.reserve(N);
        for (int i = 0; i < N; ++i)
            deposits.push_back(makeDeposit(i));

        StringIntMap nameIndex(211);
        HashMultiMap<double> amountIndex(211);
        HashMultiMap<double> rateIndex(211);
        HashMultiMap<int> monthsIndex(211);

        // ---- build phase ----
        auto t1 = std::chrono::high_resolution_clock::now();

        Utilities::quickSort(deposits, DepositSort::byName);

        for (std::size_t i = 0; i < deposits.size(); ++i)
            nameIndex.put(deposits[i].name, i);

        for (std::size_t i = 0; i < deposits.size(); ++i)
        {
            amountIndex.put(deposits[i].amount, i);
            rateIndex.put(deposits[i].rate, i);
            monthsIndex.put(deposits[i].months, i);
        }

        auto t2 = std::chrono::high_resolution_clock::now();

        // ---- view sort by amount ----
        std::vector<std::size_t> viewOrder(deposits.size());
        for (std::size_t i = 0; i < deposits.size(); ++i)
            viewOrder[i] = i;

        auto t3 = std::chrono::high_resolution_clock::now();
        Utilities::quickSortIndices(viewOrder, deposits, DepositSort::byAmount);
        auto t4 = std::chrono::high_resolution_clock::now();

        // ---- insert + rebuild (simulate add) ----
        auto t5 = std::chrono::high_resolution_clock::now();

        deposits.push_back(makeDeposit(N + 1));
        Utilities::quickSort(deposits, DepositSort::byName);

        nameIndex = StringIntMap(211);
        amountIndex.clear();
        rateIndex.clear();
        monthsIndex.clear();
        for (std::size_t i = 0; i < deposits.size(); ++i)
        {
            nameIndex.put(deposits[i].name, i);
            amountIndex.put(deposits[i].amount, i);
            rateIndex.put(deposits[i].rate, i);
            monthsIndex.put(deposits[i].months, i);
        }

        auto t6 = std::chrono::high_resolution_clock::now();

        auto build_ms = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() / 1000.0;
        auto view_ms = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count() / 1000.0;
        auto insert_ms = std::chrono::duration_cast<std::chrono::microseconds>(t6 - t5).count() / 1000.0;

        std::size_t space_approx =
            deposits.size() +
            nameIndex.getBucketCount() +
            amountIndex.getBucketCount() +
            rateIndex.getBucketCount() +
            monthsIndex.getBucketCount();

        out << N << ","
            << build_ms << ","
            << view_ms << ","
            << insert_ms << ","
            << space_approx << "\n";

        std::cout << "[bench deposits] N=" << N
                  << " build=" << build_ms << "ms"
                  << " view=" << view_ms << "ms"
                  << " insert=" << insert_ms << "ms\n";
    }

    std::cout << "✅ deposits benchmark -> data/bench_deposits.csv\n";
}

// ---------------------------------------------------------
// benchmark: LOANS
// ---------------------------------------------------------
static void runLoanBenchmark(const std::vector<int> &sizes)
{
    std::ofstream out("data/bench_loans.csv");
    if (!out)
    {
        std::cout << "Could not open data/bench_loans.csv for writing\n";
        return;
    }

    out << "N,build_ms,view_sort_ms,insert_rebuild_ms,space_units\n";

    for (int N : sizes)
    {
        std::vector<Loan> loans;
        loans.reserve(N);
        for (int i = 0; i < N; ++i)
            loans.push_back(makeLoan(i));

        StringIntMap nameIndex(101);
        HashMultiMap<double> principalIndex(101);
        HashMultiMap<double> rateIndex(101);
        HashMultiMap<int> yearsIndex(101);

        auto t1 = std::chrono::high_resolution_clock::now();

        Utilities::quickSort(loans, LoanBy::byName);

        for (std::size_t i = 0; i < loans.size(); ++i)
            nameIndex.put(loans[i].name, i);

        for (std::size_t i = 0; i < loans.size(); ++i)
        {
            principalIndex.put(loans[i].principal, i);
            rateIndex.put(loans[i].rate, i);
            yearsIndex.put(loans[i].years, i);
        }

        auto t2 = std::chrono::high_resolution_clock::now();

        std::vector<std::size_t> viewOrder(loans.size());
        for (std::size_t i = 0; i < loans.size(); ++i)
            viewOrder[i] = i;

        auto t3 = std::chrono::high_resolution_clock::now();
        Utilities::quickSortIndices(viewOrder, loans, LoanBy::byPrincipal);
        auto t4 = std::chrono::high_resolution_clock::now();

        auto t5 = std::chrono::high_resolution_clock::now();
        loans.push_back(makeLoan(N + 1));
        Utilities::quickSort(loans, LoanBy::byName);

        nameIndex = StringIntMap(101);
        principalIndex.clear();
        rateIndex.clear();
        yearsIndex.clear();
        for (std::size_t i = 0; i < loans.size(); ++i)
        {
            nameIndex.put(loans[i].name, i);
            principalIndex.put(loans[i].principal, i);
            rateIndex.put(loans[i].rate, i);
            yearsIndex.put(loans[i].years, i);
        }
        auto t6 = std::chrono::high_resolution_clock::now();

        auto build_ms = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() / 1000.0;
        auto view_ms = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count() / 1000.0;
        auto insert_ms = std::chrono::duration_cast<std::chrono::microseconds>(t6 - t5).count() / 1000.0;

        std::size_t space_approx =
            loans.size() +
            nameIndex.getBucketCount() +
            principalIndex.getBucketCount() +
            rateIndex.getBucketCount() +
            yearsIndex.getBucketCount();

        out << N << ","
            << build_ms << ","
            << view_ms << ","
            << insert_ms << ","
            << space_approx << "\n";

        std::cout << "[bench loans] N=" << N
                  << " build=" << build_ms << "ms"
                  << " view=" << view_ms << "ms"
                  << " insert=" << insert_ms << "ms\n";
    }

    std::cout << "✅ loans benchmark -> data/bench_loans.csv\n";
}

// ---------------------------------------------------------
// benchmark: CREDITS
// ---------------------------------------------------------
static void runCreditBenchmark(const std::vector<int> &sizes)
{
    std::ofstream out("data/bench_credits.csv");
    if (!out)
    {
        std::cout << "Could not open data/bench_credits.csv for writing\n";
        return;
    }

    out << "N,build_ms,view_sort_ms,insert_rebuild_ms,space_units\n";

    for (int N : sizes)
    {
        std::vector<CreditRecord> credits;
        credits.reserve(N);
        for (int i = 0; i < N; ++i)
            credits.push_back(makeCredit(i));

        StringIntMap nameIndex(211);
        HashMultiMap<double> amountIndex(211);
        HashMultiMap<double> interestIndex(211);
        HashMultiMap<int> monthsIndex(211);

        auto t1 = std::chrono::high_resolution_clock::now();

        Utilities::quickSort(credits, CreditSort::byName);

        for (std::size_t i = 0; i < credits.size(); ++i)
            nameIndex.put(credits[i].name, i);

        for (std::size_t i = 0; i < credits.size(); ++i)
        {
            amountIndex.put(credits[i].amount, i);
            interestIndex.put(credits[i].interest, i);
            monthsIndex.put(credits[i].months, i);
        }

        auto t2 = std::chrono::high_resolution_clock::now();

        std::vector<std::size_t> viewOrder(credits.size());
        for (std::size_t i = 0; i < credits.size(); ++i)
            viewOrder[i] = i;

        auto t3 = std::chrono::high_resolution_clock::now();
        Utilities::quickSortIndices(viewOrder, credits, CreditSort::byAmount);
        auto t4 = std::chrono::high_resolution_clock::now();

        auto t5 = std::chrono::high_resolution_clock::now();
        credits.push_back(makeCredit(N + 1));
        Utilities::quickSort(credits, CreditSort::byName);

        nameIndex = StringIntMap(211);
        amountIndex.clear();
        interestIndex.clear();
        monthsIndex.clear();
        for (std::size_t i = 0; i < credits.size(); ++i)
        {
            nameIndex.put(credits[i].name, i);
            amountIndex.put(credits[i].amount, i);
            interestIndex.put(credits[i].interest, i);
            monthsIndex.put(credits[i].months, i);
        }
        auto t6 = std::chrono::high_resolution_clock::now();

        auto build_ms = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() / 1000.0;
        auto view_ms = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count() / 1000.0;
        auto insert_ms = std::chrono::duration_cast<std::chrono::microseconds>(t6 - t5).count() / 1000.0;

        std::size_t space_approx =
            credits.size() +
            nameIndex.getBucketCount() +
            amountIndex.getBucketCount() +
            interestIndex.getBucketCount() +
            monthsIndex.getBucketCount();

        out << N << ","
            << build_ms << ","
            << view_ms << ","
            << insert_ms << ","
            << space_approx << "\n";

        std::cout << "[bench credits] N=" << N
                  << " build=" << build_ms << "ms"
                  << " view=" << view_ms << "ms"
                  << " insert=" << insert_ms << "ms\n";
    }

    std::cout << "✅ credits benchmark -> data/bench_credits.csv\n";
}

// ---------------------------------------------------------
// show main menu
// ---------------------------------------------------------
static void printMainMenu()
{
    std::cout << "=== FinCalc-DSA ===\n"
              << "1) Loans (ROI calc)\n"
              << "2) Deposits (FD/RD)\n"
              << "3) Credit/Debt metrics\n"
              << "4) Run benchmarks (write CSVs)\n"
              << "5) Exit\n"
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
            Credits::showMenu();
            break;
        case 4:
        {
            // sizes you want on the graph
            std::vector<int> sizes = {1000, 2000, 4000, 6000, 8000, 10000};
            std::cout << "\nRunning benchmarks...\n";
            runDepositBenchmark(sizes);
            runLoanBenchmark(sizes);
            runCreditBenchmark(sizes);
            std::cout << "All benchmark CSVs written to data/.\n";
            break;
        }
        case 5:
            std::cout << "Bye!\n";
            return 0;
        default:
            std::cout << "Invalid choice. Try again.\n";
            break;
        }
    }
}
