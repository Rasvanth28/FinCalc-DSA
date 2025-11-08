
# 🧩 FinCalc-DSA — System Architecture

## 📘 Overview
**FinCalc-DSA** is a modular C++17 application for analyzing and benchmarking fundamental **data-structure and algorithm performance** in a realistic financial-computation context — including **Deposits**, **Loans**, and **Credits/Debts**.  
It provides both an **interactive menu system** for user operations and a **benchmark mode** for time-complexity and space-complexity visualization.

The project measures how different algorithmic operations (sorting, searching, hashing, and rebuilding indexes) scale with data size and compares them to theoretical complexity models (`O(n)`, `O(n log n)`).

---

## 🧱 Folder Structure

```
FinCalc-DSA/
├── include/
│   ├── deposits.hpp
│   ├── loans.hpp
│   ├── credit.hpp
│   ├── utilities.hpp
│   ├── ds_array.hpp
│   ├── ds_hash.hpp
│   ├── ds_linked_list.hpp
│   ├── storage.hpp
│
├── src/
│   ├── main.cpp
│   ├── deposits.cpp
│   ├── loans.cpp
│   ├── credit.cpp
│   ├── storage.cpp
│   ├── bench.cpp
│
├── data/
│   ├── deposits.txt
│   ├── loans.txt
│   ├── credits.txt
│   ├── bench_deposits.csv
│   ├── bench_loans.csv
│   ├── bench_credits.csv
│
├── plot_complexity.py
├── run_bench.sh
├── Makefile
└── architecture.md   ← (this file)
```

---

## ⚙️ Core Components

### 1. Data Structures
| File                 | Structure                                  | Purpose                                                                                   |
| -------------------- | ------------------------------------------ | ----------------------------------------------------------------------------------------- |
| `ds_array.hpp`       | Dynamic array (wrapper on `std::vector`)   | Sequential container supporting `push` / `removeAt`                                       |
| `ds_linked_list.hpp` | Singly linked list using `std::unique_ptr` | Used to track *recent user actions*                                                       |
| `ds_hash.hpp`        | Custom hash map + multi-map                | Provides `StringIntMap` (unique key) and `HashMultiMap<K>` (non-unique keys) for indexing |
| `storage.hpp / .cpp` | File I/O manager                           | Loads/saves vectors of deposits, loans, or credits as plain text                          |

---

### 2. Utilities & Algorithms
| File                                  | Functionality                                                                                                                                      | Complexity                                                 |
| ------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------- |
| `utilities.hpp`                       | Generic sorting & searching algorithms: <br>• QuickSort (optimized middle-pivot) <br>• InsertionSort <br>• BubbleSort <br>• Binary & Linear Search | QuickSort → `O(n log n)` <br> Search → `O(log n)` / `O(n)` |
| `DepositSort`, `LoanBy`, `CreditSort` | Attribute comparators (by name, amount, rate, etc.)                                                                                                | Used by templated algorithms                               |

---

### 3. Financial Modules
| Module            | Description                                                         | Internal Indexes                                           |
| ----------------- | ------------------------------------------------------------------- | ---------------------------------------------------------- |
| **Deposits**      | Manages fixed deposits and recurring deposits; calculates maturity. | `nameIndex`, `amountIndex`, `rateIndex`, `monthsIndex`     |
| **Loans**         | Computes EMI, simple & compound interest.                           | `nameIndex`, `principalIndex`, `rateIndex`, `yearsIndex`   |
| **Credits/Debts** | Tracks monthly repayment and outstanding balance.                   | `nameIndex`, `amountIndex`, `interestIndex`, `monthsIndex` |

Each module maintains:
- **Vector of records** (primary store)
- **Sorted viewOrder** (for display)
- **Hash indexes** for O(1) search on multiple fields

---

### 4. Benchmark Engine
**File:** `bench.cpp`  
**Purpose:** Measures runtime and space scaling across datasets of increasing size (`N = 1000…10000`).

For each domain (Deposits, Loans, Credits):
1. Generate synthetic records.
2. **Build Indexes** → sort by name + build hash maps → *O(n log n)*  
3. **View Sort** → sort viewOrder by numeric key → *O(n log n)*  
4. **Insert + Rebuild** → add one record & rebuild → *O(n log n)*  
5. Output timing + space to CSV.

Time measured using `std::chrono::high_resolution_clock`.

---

### 5. Plotting & Visualization
**File:** `plot_complexity.py`  
Uses **Pandas** and **Matplotlib** to visualize:
- Time vs `N` (records)
- Space vs `N`
- Theoretical reference curves (`O(n)` & `O(n log n)`)

Sample command:
```bash
python3 plot_complexity.py
```

Output:
- Six graphs (time + space for Deposits, Loans, Credits)
- Linear and log-linear reference curves

---

## 🧠 Design Principles

- **Modular Separation:** Each module (Deposit, Loan, Credit) shares the same algorithmic pattern for easy benchmarking.
- **Custom Data Structures:** All core algorithms are implemented manually — no STL sort or map — to analyze *true data-structure complexity*.
- **O(1) Indexing:** Hash maps store key → record index for constant-time retrieval.
- **Stable Sorting:** Middle-pivot quicksort minimizes worst-case performance spikes.
- **Efficient Memory Use:** Hash maps dynamically resize buckets to maintain low load factor.

---

## ⚡ Performance Characteristics

| Operation        | Theoretical | Observed            | Comments                               |
| ---------------- | ----------- | ------------------- | -------------------------------------- |
| Sort (QuickSort) | O(n log n)  | Matches theory      | Pivot randomization stabilized runtime |
| Hash insert/get  | O(1) avg    | Constant across N   | Resizable buckets prevent clustering   |
| Insert + rebuild | ~O(n log n) | Matches within ±10% | Due to resort + reindex                |
| Space            | O(n)        | Linear scaling      | Each index adds predictable overhead   |

---

## 🧮 Complexity Graphs
**Reference Results (N = 1000–10000):**
- Time plots show linear/log-linear growth (no spikes ✅)
- Space plots are perfectly linear (`O(n)`)
- Insert operations remain constant-time across tests

---

## 🧰 Run Instructions

```bash
# 1. Create and activate virtual environment
python3 -m venv venv
source venv/bin/activate

# 2. Install dependencies
pip install matplotlib pandas

# 3. Build benchmark binary
make bench

# 4. Run all benchmarks
./run_bench.sh

# 5. Plot results
python3 plot_complexity.py
```

---

## 🧾 Summary

**FinCalc-DSA** demonstrates how theoretical time and space complexities map onto *real-world data behavior* in C++.  
It serves as a benchmark framework combining **algorithm design**, **data-structure optimization**, and **financial computation modeling** — a complete educational and analytical toolkit.
