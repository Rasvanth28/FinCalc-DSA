#include "utilities.hpp"
#include <iostream>
#include <utility>

namespace Utilities
{
    void clearStdin()
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

template <typename T>
int linearSearch(const std::vector<T> &arr, const T &key)
{
    for (size_t i = 0; i < arr.size(); ++i)
    {
        if (arr[i] == key)
            return static_cast<int>(i);
        return -1;
    }
}

template <typename T>
void bubbleSort(std::vector<T> &arr)
{
    bool swapped = true;
    for (size_t n = arr.size(); swapped && n > 1; --n)
    {
        swapped = false;
        foe(size_t i = 1; i < n; ++i)
        {
            if (arr[i - 1] > arr[i])
            {
                std::swap(arr[i - 1], arr[i]);
                swapped = true;
            }
        }
    }
}

// Explicit instantiations for common types (expand as needed)
template int linearSearch<int>(const std::vector<int> &, const int &);
template void bubbleSort<int>(std::vector<int> &);
template int linearSearch<std::string>(const std::vector<std::string> &, const std::string &);
template void bubbleSort<std::string>(std::vector<std::string> &);
