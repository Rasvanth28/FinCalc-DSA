#pragma once
#include <vector>
#include <string>
#include "deposits.hpp"

namespace Utilities
{
    void clearStdin();

    template <typename T>
    int linearSearch(const std::vector<T> &arr, const T &key);

    template <typename T>
    void bubbleSort(std::vector<T> &arr);

    template <typename T>
    void insertionSort(std::vector<T> &arr);

    template <typename T>
    int binarySearch(const std::vector<T> &arr, const T &key);

}
