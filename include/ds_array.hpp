#pragma once
#include <vector>

template <typename T>
class DSArray
{
public:
    void push(const T &v) { data_.push_back(v); }

    bool removeAt(std::size_t idx)
    {
        if (idx >= data_.size())
            return false;
        data_.erase(data_.begin() + idx); // O(n)
        return true;
    }

    const std::vector<T> &raw() const { return data_; }

private:
    std::vector<T> data_;
};
