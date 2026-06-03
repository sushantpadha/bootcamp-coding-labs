template <typename T>
struct FriendAllocator {
    using value_type = T;
    FriendAllocator() noexcept = default;
    template <typename U> FriendAllocator(const FriendAllocator<U>&) noexcept {}

    [[nodiscard]] T* allocate(size_t n) {
        if (n == 0) return nullptr;
        if (auto ptr = static_cast<T*>(::operator new(n * sizeof(T)))) {
            return ptr;
        }
        throw std::bad_alloc();
    }

    void deallocate(T* p, size_t n) noexcept {
        ::operator delete(p);
    }

    template <typename U>
    bool operator==(const FriendAllocator<U>&) { return true; }
    // automagically generates !=
};
