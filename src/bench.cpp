// src/bench.cpp
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cmath>
#include <cstdlib>

#include "deposits.hpp"
#include "loans.hpp"
#include "credit.hpp"
#include "utilities.hpp"
#include "ds_hash.hpp"

// decide how many runs to average for a given N
static int runs_for(int N)
{
    if (N >= 8000)
        return 5;
    if (N >= 4000)
        return 10;
    return 30;
}

// ---------- tiny helpers to make fake data ----------
static Deposit makeDeposit(int i)
{
    Deposit d;
    d.name = "Dep_" + std::to_string(i);
    d.amount = 1000 + (i % 5000);
    d.rate = 5.0 + (i % 10);
    d.months = 6 + (i % 36);
    return d;
}

static Loan makeLoan(int i)
{
    Loan l;
    l.name = "Loan_" + std::to_string(i);
    l.principal = 10000 + (i % 20000);
    l.rate = 7.5 + (i % 5);
    l.years = 1 + (i % 10);
    return l;
}

static CreditRecord makeCredit(int i)
{
    CreditRecord c;
    c.name = "Cred_" + std::to_string(i);
    c.amount = 500 + (i % 2500);
    c.interest = 1.5 + (i % 6);
    c.months = 3 + (i % 24);
    return c;
}

// ------------- timing helper -------------
template <typename F>
static long long time_ms(F &&fn, int runs = 1)
{
    using clock = std::chrono::high_resolution_clock;
    long long total = 0;
    for (int r = 0; r < runs; ++r)
    {
        auto t1 = clock::now();
        fn();
        auto t2 = clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    // return average in ms
    return total / runs / 1000; // micro -> ms
}

// ---------- benchmark for DEPOSITS ----------
static void benchDeposits(int N)
{
    // generate
    std::vector<Deposit> deps;
    deps.reserve(N);
    for (int i = 0; i < N; ++i)
        deps.push_back(makeDeposit(i));

    std::vector<std::size_t> viewOrder;
    viewOrder.reserve(N);

    // scale buckets with N to avoid collisions
    std::size_t bucketBase = std::max(211, N / 4);

    StringIntMap nameIndex(bucketBase);
    HashMultiMap<double> amountIndex(bucketBase);
    HashMultiMap<double> rateIndex(bucketBase);
    HashMultiMap<int> monthsIndex(bucketBase);

    int runs = runs_for(N);

    // WARMUP (not measured)
    {
        Utilities::quickSort(deps, DepositSort::byName);
        viewOrder.clear();
        for (std::size_t i = 0; i < deps.size(); ++i)
            viewOrder.push_back(i);
        Utilities::quickSortIndices(viewOrder, deps, DepositSort::byName);

        nameIndex = StringIntMap(bucketBase);
        amountIndex.clear();
        rateIndex.clear();
        monthsIndex.clear();
        for (std::size_t i = 0; i < deps.size(); ++i)
        {
            nameIndex.put(deps[i].name, i);
            amountIndex.put(deps[i].amount, i);
            rateIndex.put(deps[i].rate, i);
            monthsIndex.put(deps[i].months, i);
        }
    }

    // build indexes (sort + all 4 maps)
    long long ms_build = time_ms([&]()
                                 {
        Utilities::quickSort(deps, DepositSort::byName);

        nameIndex = StringIntMap(bucketBase);
        amountIndex.clear();
        rateIndex.clear();
        monthsIndex.clear();

        for (std::size_t i = 0; i < deps.size(); ++i)
        {
            nameIndex.put(deps[i].name, i);
            amountIndex.put(deps[i].amount, i);
            rateIndex.put(deps[i].rate, i);
            monthsIndex.put(deps[i].months, i);
        } }, runs);

    // build view and sort by amount
    long long ms_view = time_ms([&]()
                                {
        viewOrder.clear();
        for (std::size_t i = 0; i < deps.size(); ++i)
            viewOrder.push_back(i);
        Utilities::quickSortIndices(viewOrder, deps, DepositSort::byAmount); }, runs);

    // insert + rebuild (simulate your real code)
    long long ms_insert = time_ms([&]()
                                  {
                                      deps.push_back(makeDeposit(N + 99));
                                      Utilities::quickSort(deps, DepositSort::byName);
                                      nameIndex = StringIntMap(bucketBase);
                                      amountIndex.clear();
                                      rateIndex.clear();
                                      monthsIndex.clear();
                                      for (std::size_t i = 0; i < deps.size(); ++i)
                                      {
                                          nameIndex.put(deps[i].name, i);
                                          amountIndex.put(deps[i].amount, i);
                                          rateIndex.put(deps[i].rate, i);
                                          monthsIndex.put(deps[i].months, i);
                                      }
                                      deps.pop_back(); // restore size for next run
                                  },
                                  runs);

    // space estimate
    std::size_t space_units =
        deps.size() + nameIndex.getBucketCount() + amountIndex.getBucketCount() + rateIndex.getBucketCount() + monthsIndex.getBucketCount();

    // CSV: N,build_ms,view_ms,insert_ms,space
    std::cout << N << "," << ms_build << "," << ms_view << "," << ms_insert << "," << space_units << "\n";
}

// ---------- benchmark for LOANS ----------
static void benchLoans(int N)
{
    std::vector<Loan> loans;
    loans.reserve(N);
    for (int i = 0; i < N; ++i)
        loans.push_back(makeLoan(i));

    std::vector<std::size_t> viewOrder;
    viewOrder.reserve(N);

    std::size_t bucketBase = std::max(101, N / 4);

    StringIntMap nameIndex(bucketBase);
    HashMultiMap<double> principalIndex(bucketBase);
    HashMultiMap<double> rateIndex(bucketBase);
    HashMultiMap<int> yearsIndex(bucketBase);

    int runs = runs_for(N);

    // warmup
    {
        Utilities::quickSort(loans, LoanBy::byName);
        viewOrder.clear();
        for (std::size_t i = 0; i < loans.size(); ++i)
            viewOrder.push_back(i);
        Utilities::quickSortIndices(viewOrder, loans, LoanBy::byName);

        nameIndex = StringIntMap(bucketBase);
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
    }

    long long ms_build = time_ms([&]()
                                 {
        Utilities::quickSort(loans, LoanBy::byName);
        nameIndex = StringIntMap(bucketBase);
        principalIndex.clear();
        rateIndex.clear();
        yearsIndex.clear();
        for (std::size_t i = 0; i < loans.size(); ++i)
        {
            nameIndex.put(loans[i].name, i);
            principalIndex.put(loans[i].principal, i);
            rateIndex.put(loans[i].rate, i);
            yearsIndex.put(loans[i].years, i);
        } }, runs);

    long long ms_view = time_ms([&]()
                                {
        viewOrder.clear();
        for (std::size_t i = 0; i < loans.size(); ++i)
            viewOrder.push_back(i);
        Utilities::quickSortIndices(viewOrder, loans, LoanBy::byPrincipal); }, runs);

    long long ms_insert = time_ms([&]()
                                  {
        loans.push_back(makeLoan(N + 99));
        Utilities::quickSort(loans, LoanBy::byName);
        nameIndex = StringIntMap(bucketBase);
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
        loans.pop_back(); }, runs);

    std::size_t space_units =
        loans.size() + nameIndex.getBucketCount() + principalIndex.getBucketCount() + rateIndex.getBucketCount() + yearsIndex.getBucketCount();

    std::cout << N << "," << ms_build << "," << ms_view << "," << ms_insert << "," << space_units << "\n";
}

// ---------- benchmark for CREDITS ----------
static void benchCredits(int N)
{
    std::vector<CreditRecord> creds;
    creds.reserve(N);
    for (int i = 0; i < N; ++i)
        creds.push_back(makeCredit(i));

    std::vector<std::size_t> viewOrder;
    viewOrder.reserve(N);

    std::size_t bucketBase = std::max(211, N / 4);

    StringIntMap nameIndex(bucketBase);
    HashMultiMap<double> amountIndex(bucketBase);
    HashMultiMap<double> interestIndex(bucketBase);
    HashMultiMap<int> monthsIndex(bucketBase);

    int runs = runs_for(N);

    // warmup
    {
        Utilities::quickSort(creds, CreditSort::byName);
        viewOrder.clear();
        for (std::size_t i = 0; i < creds.size(); ++i)
            viewOrder.push_back(i);
        Utilities::quickSortIndices(viewOrder, creds, CreditSort::byName);

        nameIndex = StringIntMap(bucketBase);
        amountIndex.clear();
        interestIndex.clear();
        monthsIndex.clear();
        for (std::size_t i = 0; i < creds.size(); ++i)
        {
            nameIndex.put(creds[i].name, i);
            amountIndex.put(creds[i].amount, i);
            interestIndex.put(creds[i].interest, i);
            monthsIndex.put(creds[i].months, i);
        }
    }

    long long ms_build = time_ms([&]()
                                 {
        Utilities::quickSort(creds, CreditSort::byName);
        nameIndex = StringIntMap(bucketBase);
        amountIndex.clear();
        interestIndex.clear();
        monthsIndex.clear();
        for (std::size_t i = 0; i < creds.size(); ++i)
        {
            nameIndex.put(creds[i].name, i);
            amountIndex.put(creds[i].amount, i);
            interestIndex.put(creds[i].interest, i);
            monthsIndex.put(creds[i].months, i);
        } }, runs);

    long long ms_view = time_ms([&]()
                                {
        viewOrder.clear();
        for (std::size_t i = 0; i < creds.size(); ++i)
            viewOrder.push_back(i);
        Utilities::quickSortIndices(viewOrder, creds, CreditSort::byAmount); }, runs);

    long long ms_insert = time_ms([&]()
                                  {
        creds.push_back(makeCredit(N + 99));
        Utilities::quickSort(creds, CreditSort::byName);
        nameIndex = StringIntMap(bucketBase);
        amountIndex.clear();
        interestIndex.clear();
        monthsIndex.clear();
        for (std::size_t i = 0; i < creds.size(); ++i)
        {
            nameIndex.put(creds[i].name, i);
            amountIndex.put(creds[i].amount, i);
            interestIndex.put(creds[i].interest, i);
            monthsIndex.put(creds[i].months, i);
        }
        creds.pop_back(); }, runs);

    std::size_t space_units =
        creds.size() + nameIndex.getBucketCount() + amountIndex.getBucketCount() + interestIndex.getBucketCount() + monthsIndex.getBucketCount();

    std::cout << N << "," << ms_build << "," << ms_view << "," << ms_insert << "," << space_units << "\n";
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cerr << "usage: bench <deposits|loans|credits> N1 N2 ...\n";
        return 1;
    }

    std::string which = argv[1];

    for (int i = 2; i < argc; ++i)
    {
        int N = std::atoi(argv[i]);
        if (N <= 0)
            continue;

        if (which == "deposits")
            benchDeposits(N);
        else if (which == "loans")
            benchLoans(N);
        else if (which == "credits")
            benchCredits(N);
        else
        {
            std::cerr << "unknown bench: " << which << "\n";
            return 1;
        }
    }
    return 0;
}
