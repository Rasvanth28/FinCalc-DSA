#pragma once
#include <string>
#include <vector>
#include <type_traits>
#include <functional>
#include <utility>

// ---------------------------------------------------------
// Single-value hash map: string -> index
// now with load-factor check + rehash so it doesn't degrade
// ---------------------------------------------------------
struct StringIntMap
{
    struct Node
    {
        std::string key;
        std::size_t value;
    };

    std::vector<std::vector<Node>> buckets;
    std::size_t bucketCount;
    std::size_t size_; // number of pairs

    explicit StringIntMap(std::size_t n = 101)
        : bucketCount(n), size_(0)
    {
        buckets.resize(bucketCount);
    }

    std::size_t hash(const std::string &s) const
    {
        return std::hash<std::string>{}(s) % bucketCount;
    }

    double loadFactor() const
    {
        return bucketCount == 0 ? 0.0
                                : static_cast<double>(size_) / static_cast<double>(bucketCount);
    }

    void rehash(std::size_t newBucketCount)
    {
        std::vector<std::vector<Node>> newBuckets;
        newBuckets.resize(newBucketCount);

        for (auto &bucket : buckets)
        {
            for (auto &node : bucket)
            {
                std::size_t idx = std::hash<std::string>{}(node.key) % newBucketCount;
                newBuckets[idx].push_back(std::move(node));
            }
        }

        buckets = std::move(newBuckets);
        bucketCount = newBucketCount;
    }

    void maybeGrow()
    {
        // keep it simple: grow when > 0.75
        if (loadFactor() > 0.75)
        {
            rehash(bucketCount * 2 + 1);
        }
    }

    void put(const std::string &key, std::size_t value)
    {
        std::size_t idx = hash(key);
        auto &bucket = buckets[idx];
        for (auto &node : bucket)
        {
            if (node.key == key)
            {
                node.value = value;
                return;
            }
        }
        bucket.push_back({key, value});
        ++size_;
        maybeGrow();
    }

    bool get(const std::string &key, std::size_t &out) const
    {
        std::size_t idx = hash(key);
        const auto &bucket = buckets[idx];
        for (const auto &node : bucket)
        {
            if (node.key == key)
            {
                out = node.value;
                return true;
            }
        }
        return false;
    }

    std::size_t getBucketCount() const { return bucketCount; }
    std::size_t size() const { return size_; }
};

// ---------------------------------------------------------
// generic hash for template multi-map
// ---------------------------------------------------------
template <typename K>
inline std::size_t genericHash(const K &k)
{
    return std::hash<K>{}(k);
}

template <>
inline std::size_t genericHash<std::string>(const std::string &s)
{
    return std::hash<std::string>{}(s);
}

// ---------------------------------------------------------
// HashMultiMap<K> : key -> vector<indices>
// also with rehashing
// ---------------------------------------------------------
template <typename K>
class HashMultiMap
{
    struct Node
    {
        K key;
        std::vector<std::size_t> values;
    };

    std::vector<std::vector<Node>> buckets;
    std::size_t bucketCount;
    std::size_t size_; // number of value insertions

    std::size_t doHash(const K &k) const
    {
        return genericHash<K>(k) % bucketCount;
    }

    void rehash(std::size_t newBucketCount)
    {
        std::vector<std::vector<Node>> newBuckets;
        newBuckets.resize(newBucketCount);

        for (auto &bucket : buckets)
        {
            for (auto &node : bucket)
            {
                std::size_t idx = genericHash<K>(node.key) % newBucketCount;
                newBuckets[idx].push_back(std::move(node));
            }
        }

        buckets = std::move(newBuckets);
        bucketCount = newBucketCount;
    }

    void maybeGrow()
    {
        double lf = bucketCount == 0 ? 0.0
                                     : static_cast<double>(size_) / static_cast<double>(bucketCount);
        if (lf > 0.75)
        {
            rehash(bucketCount * 2 + 1);
        }
    }

public:
    explicit HashMultiMap(std::size_t n = 101)
        : bucketCount(n), size_(0)
    {
        buckets.resize(bucketCount);
    }

    void put(const K &key, std::size_t valueIdx)
    {
        std::size_t h = doHash(key);
        auto &bucket = buckets[h];
        for (auto &node : bucket)
        {
            if (node.key == key)
            {
                node.values.push_back(valueIdx);
                ++size_;
                maybeGrow();
                return;
            }
        }
        Node nnode;
        nnode.key = key;
        nnode.values.push_back(valueIdx);
        bucket.push_back(std::move(nnode));
        ++size_;
        maybeGrow();
    }

    bool get(const K &key, std::vector<std::size_t> &out) const
    {
        std::size_t h = doHash(key);
        const auto &bucket = buckets[h];
        for (const auto &node : bucket)
        {
            if (node.key == key)
            {
                out = node.values;
                return true;
            }
        }
        return false;
    }

    void clear()
    {
        for (auto &b : buckets)
            b.clear();
        size_ = 0;
    }

    std::size_t getBucketCount() const { return bucketCount; }
    std::size_t size() const { return size_; }
};
