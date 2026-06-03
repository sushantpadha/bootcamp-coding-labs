#include "friend_list.h"
#include <memory>

void FriendList::add(const std::string& name) {
    _ptr.push_back(name);
}

size_t FriendList::size() const {
    return _ptr.size();
}

std::ostream& operator<<(std::ostream& out, FriendList const& fl) {
    for(size_t i = 0; i < fl.size(); i++) {
        out << fl._ptr[i] << (i == fl.size() - 1 ? "" : ", ");
    }
    return out;
}

// -------------------------------------------------------------------------
// ------------------ version with allocators and shit ---------------------
// -------------------------------------------------------------------------

__FriendList::__FriendList(size_t n_)
    : _alloc{}, _size{}, _capacity{n_}
{
    _ptr = traits::allocate(_alloc, _capacity);
}

void __FriendList::add(const std::string& name) {
    if (_size == _capacity)
        grow( _capacity ? 2 * _capacity : 1);
    traits::construct(_alloc, _ptr + _size, name);
    _size++;
}

size_t __FriendList::size() const {
    return _size;
}

void swap(__FriendList& lhs, __FriendList& rhs) noexcept {
    using std::swap;
    swap(lhs._ptr, rhs._ptr);
    swap(lhs._size, rhs._size);
    swap(lhs._capacity, rhs._capacity);
}

__FriendList::__FriendList(const __FriendList& other) {
    _size = other._size;
    _capacity = other._capacity;
    _ptr = traits::allocate(_alloc, _capacity);
    try {
        std::uninitialized_copy(other._ptr, other._ptr + _size, _ptr);
    } catch (...) {
        traits::deallocate(_alloc, _ptr, _capacity);
        throw;
    }
}

__FriendList& __FriendList::operator=(__FriendList other) {
    swap(*this, other);
    return *this;
}

__FriendList::~__FriendList() {
    std::destroy(_ptr, _ptr + _size);
    traits::deallocate(_alloc, _ptr, _capacity);
}

void __FriendList::grow(size_t new_capacity) {
    // std::uninitialized_move and destroy does this for us
    auto new_ptr = traits::allocate(_alloc, new_capacity);

    try {
        std::uninitialized_move(_ptr, _ptr + _size, new_ptr);
    } catch(...) {
        traits::deallocate(_alloc, new_ptr, new_capacity);
        throw;
    }

    std::destroy(_ptr, _ptr + _size);
    traits::deallocate(_alloc, _ptr, _capacity);
    
    _ptr      = new_ptr;
    _capacity = new_capacity;
}

std::ostream& operator<<(std::ostream& out, __FriendList const& fl) {
    for(size_t i = 0; i < fl._size; i++) {
        out << fl._ptr[i] << (i == fl._size - 1 ? "" : ", ");
    }
    return out;
}
