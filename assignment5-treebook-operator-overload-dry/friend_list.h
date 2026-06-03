#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <memory>
#include "friend_alloc.h"

class FriendList {
private:
    using type = std::string;
    std::vector<type> _ptr;

public:
    void add(const std::string& name);
    size_t size() const;

    friend std::ostream& operator<<(std::ostream& out, FriendList const& fl);
    
    type&       operator[](size_t index)       { return _ptr[index]; }
    const type& operator[](size_t index) const { return _ptr[index]; }
};

// -------------------------------------------------------------------------
// ------------------ version with allocators and shit ---------------------
// -------------------------------------------------------------------------

class __FriendList {
private:
    using type      = std::string;
    using allocator = FriendAllocator<type>;
    using traits    = std::allocator_traits<allocator>;

    [[no_unique_address]] allocator _alloc; // dont alloc 1 byte
    type* _ptr;
    size_t _size;
    size_t _capacity;

    void grow(size_t new_capacity);

public:
    __FriendList(size_t n_ = 1);
    void add(const std::string& name);
    size_t size() const;
    
    friend std::ostream& operator<<(std::ostream& out, __FriendList const& fl);
    friend void swap(__FriendList& lhs, __FriendList& rhs) noexcept;
    
    type&       operator[](size_t index)       { return _ptr[index]; }
    const type& operator[](size_t index) const { return _ptr[index]; }

    __FriendList(const __FriendList& other);
    __FriendList& operator=(__FriendList other);

    __FriendList(__FriendList&& other)              = delete;
    __FriendList& operator=(__FriendList&& other)   = delete;

    ~__FriendList();
};