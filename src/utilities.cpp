#include "utilities.hpp"
#include <iostream>
#include <utility>
#include <limits>

namespace Utilities
{
    void clearStdin()
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    template <typename T>
    int linearSearch(const std::vector<T> &arr, const T &key)
    {
        for (size_t i = 0; i < arr.size(); ++i)
        {
            if (arr[i] == key)
                return static_cast<int>(i);
        }
        return -1;
    }

    template <typename T>
    int binarySearch(const std::vector<T> &arr, const T &key)
    {
        int low = 0;
        int high = static_cast<int>(arr.size()) - 1;
        while (low <= high)
        {
            int mid = low + (high - low) / 2;
            if (arr[mid] == key)
                return mid;
            else if (arr[mid] < key)
                low = mid + 1;
            else
                high = mid - 1;
        }
        return -1;
    }

    template <typename T>
    void bubbleSort(std::vector<T> &arr)
    {
        bool swapped = true;
        for (size_t n = arr.size(); swapped && n > 1; --n)
        {
            swapped = false;
            for (size_t i = 1; i < n; ++i)
            {
                if (arr[i - 1] > arr[i])
                {
                    std::swap(arr[i - 1], arr[i]);
                    swapped = true;
                }
            }
        }
    }

    template <typename T>
    void insertionSort(std::vector<T> &arr)
    {
        for (size_t i = 1; i < arr.size(); ++i)
        {
            T key = arr[i];
            int j = static_cast<int>(i) - 1;
            while (j >= 0 && key < arr[j])
            {
                arr[j + 1] = arr[j];
                --j;
            }
            arr[j + 1] = key;
        }
    }

    // Explicit instantiations for common types (expand as needed)
    template int linearSearch<int>(const std::vector<int> &, const int &);
    template int binarySearch<int>(const std::vector<int> &, const int &);
    template void bubbleSort<int>(std::vector<int> &);
    template void insertionSort<int>(std::vector<int> &);

    template int linearSearch<std::string>(const std::vector<std::string> &, const std::string &);
    template int binarySearch<std::string>(const std::vector<std::string> &, const std::string &);
    template void bubbleSort<std::string>(std::vector<std::string> &);
    template void insertionSort<std::string>(std::vector<std::string> &);

    template void insertionSort<Deposit>(std::vector<Deposit> &);
    template int binarySearch<Deposit>(const std::vector<Deposit> &, const Deposit &);
}
