#!/usr/bin/env bash
set -e

# Step 1: Build benchmark binary
make bench

# Step 2: Create data directory
mkdir -p data

# Step 3: Run all benchmarks
echo "▶ Running FinCalc-DSA benchmarks..."
./bin/bench deposits 1000 2000 4000 6000 8000 10000 > data/bench_deposits.csv
./bin/bench loans     1000 2000 4000 6000 8000 10000 > data/bench_loans.csv
./bin/bench credits   1000 2000 4000 6000 8000 10000 > data/bench_credits.csv

echo "✅ Benchmarks complete. CSVs saved under data/."

# Step 4: Plot results automatically
if command -v python3 &>/dev/null; then
    echo "📊 Generating performance graphs..."
    python3 plot_complexity.py
else
    echo "⚠️ Python3 not found. Install it to plot results automatically."
fi
