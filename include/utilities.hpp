#pragma once
#include <vector>
#include <string>

namespace Utilities
{
    void clearStdin();
    // Stubs for Day 2-3 (search/sort playground)
    template <typename T>
    int linearSearch(const std::vector<T> &arr, const T &key);
    template <typename T>
    void bubbleSort(std::vector<T> &arr);
}
