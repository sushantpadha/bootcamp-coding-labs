#include "user.h"

/**
 * Creates a new User with the given name and no friends.
 */
User::User(const std::string& name)
  : _name(name)
  , _friends()
{
}

/**
 * Adds a friend to this User's list of friends.
 * @param name The name of the friend to add.
 */
void
User::add_friend(const std::string& name)
{
  _friends.add(name);
}

/**
 * Returns the name of this User.
 */
std::string
User::get_name() const
{
  return _name;
}

/**
 * Returns the number of friends this User has.
 */
size_t User::size() const
{
  return _friends.size();
}

/**
 * Sets the friend at the given index to the given name.
 * @param index The index of the friend to set.
 * @param name The name to set the friend to.
 */
void User::set_friend(size_t index, const std::string& name)
{
  _friends[index] = name;
}

/** 
 * STUDENT TODO:
 * The definitions for your custom operators and special member functions will go here!
 */
std::ostream& operator<<(std::ostream& out, User const& user) {
    out << "User(name=" << user._name << ", friends=[";
    out << user._friends;
    return (out << "])");
}

// void swap(User& lhs, User& rhs) noexcept {
//     using std::swap;
//     swap(lhs._name, rhs._name);
//     swap(lhs._friends, rhs._friends);
//     swap(lhs._size, rhs._size);
//     swap(lhs._capacity, rhs._capacity);
// }

// // must be defined
// User::User(const User& user) {
//     _name = user._name;
//     _size = user._size;
//     _capacity = user._capacity;
//     _friends = new std::string[_capacity];
//     for(size_t i = 0; i < _size; i++) {
//         _friends[i] = user._friends[i];
//     }
// }

// User& User::operator=(User user) {
//     swap(*this, user);
//     return *this;
// }

// User::~User() {
//     delete[] _friends;
// }

bool User::operator<(const User& other) const {
    return _name < other._name;
}

// symmetricl
User& User::operator+=(User& other) {
    add_friend(other._name);
    other.add_friend(_name);
    return *this;
}