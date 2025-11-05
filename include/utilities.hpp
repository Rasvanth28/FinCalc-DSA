#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <limits>
#include "deposits.hpp"
#include "loans.hpp"
#include "credit.hpp"

namespace Utilities
{
    inline void clearStdin()
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    template <typename T, typename Compare = std::less<T>>
    int linearSearch(const std::vector<T> &arr, const T &key, Compare comp = Compare())
    {
        for (size_t i = 0; i < arr.size(); ++i)
        {
            if (!comp(arr[i], key) && !comp(key, arr[i]))
                return static_cast<int>(i);
        }
        return -1;
    }

    template <typename T, typename Compare = std::less<T>>
    int binarySearch(const std::vector<T> &arr, const T &key, Compare comp = Compare())
    {
        int low = 0;
        int high = static_cast<int>(arr.size()) - 1;
        while (low <= high)
        {
            int mid = low + (high - low) / 2;

            // equality = neither is less than the other
            if (!comp(arr[mid], key) && !comp(key, arr[mid]))
                return mid;
            else if (comp(arr[mid], key))
                low = mid + 1;
            else
                high = mid - 1;
        }
        return -1;
    }

    template <typename T, typename Compare = std::less<T>>
    void bubbleSort(std::vector<T> &arr, Compare comp = Compare())
    {
        bool swapped = true;
        for (size_t n = arr.size(); swapped && n > 1; --n)
        {
            swapped = false;
            for (size_t i = 1; i < n; ++i)
            {
                if (comp(arr[i], arr[i - 1]))
                {
                    std::swap(arr[i - 1], arr[i]);
                    swapped = true;
                }
            }
        }
    }

    template <typename T, typename Compare = std::less<T>>
    void insertionSort(std::vector<T> &arr, Compare comp = Compare())
    {
        for (size_t i = 1; i < arr.size(); ++i)
        {
            T key = arr[i];
            int j = static_cast<int>(i) - 1;
            while (j >= 0 && comp(key, arr[j]))
            {
                arr[j + 1] = arr[j];
                --j;
            }
            arr[j + 1] = key;
        }
    }
}
