#pragma once
#include <memory>

template <typename T>
class DSLinkedList
{
    struct Node
    {
        T val;
        std::unique_ptr<Node> next;
        explicit Node(const T &v) : val(v), next(nullptr) {}
    };
    std::unique_ptr<Node> head_;

public:
    void pushFront(const T &v)
    {
        auto n = std::make_unique<Node>(v);
        n->next = std::move(head_);
        head_ = std::move(n);
    }

    // returns false if empty
    bool popFront(T &out)
    {
        if (!head_)
            return false;
        out = head_->val;
        head_ = std::move(head_->next);
        return true;
    }

    bool empty() const { return !head_; }
};
