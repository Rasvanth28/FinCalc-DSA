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

    inline void getlineSafe(std::string &out)
    {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::getline(std::cin, out);
    }

    // ---------------- search ----------------
    template <typename T, typename Compare = std::less<T>>
    int linearSearch(const std::vector<T> &arr, const T &key, Compare comp = Compare())
    {
        for (std::size_t i = 0; i < arr.size(); ++i)
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
            if (!comp(arr[mid], key) && !comp(key, arr[mid]))
                return mid;
            else if (comp(arr[mid], key))
                low = mid + 1;
            else
                high = mid - 1;
        }
        return -1;
    }

    // ---------------- simple sorts ----------------
    template <typename T, typename Compare = std::less<T>>
    void bubbleSort(std::vector<T> &arr, Compare comp = Compare())
    {
        bool swapped = true;
        for (std::size_t n = arr.size(); swapped && n > 1; --n)
        {
            swapped = false;
            for (std::size_t i = 1; i < n; ++i)
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
        for (std::size_t i = 1; i < arr.size(); ++i)
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

    // ---------------- improved quicksort ----------------
    constexpr int QS_INSERTION_THRESHOLD = 16;

    template <typename T, typename Compare>
    int partition(std::vector<T> &arr, int low, int high, Compare comp)
    {
        // use middle element as pivot to avoid worst-case on sorted data
        T pivot = arr[low + (high - low) / 2];
        int i = low;
        int j = high;
        while (i <= j)
        {
            while (comp(arr[i], pivot))
                ++i;
            while (comp(pivot, arr[j]))
                --j;
            if (i <= j)
            {
                std::swap(arr[i], arr[j]);
                ++i;
                --j;
            }
        }
        return i;
    }

    template <typename T, typename Compare>
    void quickSortRec(std::vector<T> &arr, int low, int high, Compare comp)
    {
        while (low < high)
        {
            // small section → insertion sort
            if (high - low < QS_INSERTION_THRESHOLD)
            {
                for (int i = low + 1; i <= high; ++i)
                {
                    T key = arr[i];
                    int j = i - 1;
                    while (j >= low && comp(key, arr[j]))
                    {
                        arr[j + 1] = arr[j];
                        --j;
                    }
                    arr[j + 1] = key;
                }
                return;
            }

            int mid = partition(arr, low, high, comp);

            // recurse on smaller side first (better stack)
            if (mid - low < high - mid)
            {
                quickSortRec(arr, low, mid - 1, comp);
                low = mid;
            }
            else
            {
                quickSortRec(arr, mid, high, comp);
                high = mid - 1;
            }
        }
    }

    template <typename T, typename Compare>
    void quickSort(std::vector<T> &arr, Compare comp)
    {
        if (!arr.empty())
            quickSortRec(arr, 0, static_cast<int>(arr.size()) - 1, comp);
    }

    // ---------------- index-based quicksort (improved) ----------------
    template <typename Data, typename Compare>
    int partitionIdx(std::vector<std::size_t> &idx, int low, int high,
                     const std::vector<Data> &data, Compare comp)
    {
        std::size_t pivotIdx = idx[low + (high - low) / 2];
        int i = low;
        int j = high;
        while (i <= j)
        {
            while (comp(data[idx[i]], data[pivotIdx]))
                ++i;
            while (comp(data[pivotIdx], data[idx[j]]))
                --j;
            if (i <= j)
            {
                std::swap(idx[i], idx[j]);
                ++i;
                --j;
            }
        }
        return i;
    }

    template <typename Data, typename Compare>
    void quickSortIdxRec(std::vector<std::size_t> &idx, int low, int high,
                         const std::vector<Data> &data, Compare comp)
    {
        while (low < high)
        {
            if (high - low < QS_INSERTION_THRESHOLD)
            {
                for (int i = low + 1; i <= high; ++i)
                {
                    std::size_t key = idx[i];
                    int j = i - 1;
                    while (j >= low && comp(data[key], data[idx[j]]))
                    {
                        idx[j + 1] = idx[j];
                        --j;
                    }
                    idx[j + 1] = key;
                }
                return;
            }

            int mid = partitionIdx(idx, low, high, data, comp);
            if (mid - low < high - mid)
            {
                quickSortIdxRec(idx, low, mid - 1, data, comp);
                low = mid;
            }
            else
            {
                quickSortIdxRec(idx, mid, high, data, comp);
                high = mid - 1;
            }
        }
    }

    template <typename Data, typename Compare>
    void quickSortIndices(std::vector<std::size_t> &idx,
                          const std::vector<Data> &data,
                          Compare comp)
    {
        if (!idx.empty())
            quickSortIdxRec(idx, 0, static_cast<int>(idx.size()) - 1, data, comp);
    }
}
