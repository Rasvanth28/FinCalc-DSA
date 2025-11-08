#!/bin/bash
set -e

echo "== Compiling =="

# compile everything from src/ and tell the compiler to look in include/
g++ -std=c++17 -Wall -Wextra -O2 \
  -Isrc -Iinclude \
  src/main.cpp \
  src/deposits.cpp \
  src/loans.cpp \
  src/credit.cpp \
  src/storage.cpp \
  -o fincalc

# make sure data folder exists
mkdir -p data

# make sure the main data files exist (your program expects them)
touch data/deposits.txt data/loans.txt data/credits.txt

echo "== Running deposits error test =="
./fincalc < tests/error_deposits.txt

echo "== Running loans error test =="
./fincalc < tests/error_loans.txt

echo "== Running credits error test =="
./fincalc < tests/error_credits.txt

echo "✅ All scripted tests done."
