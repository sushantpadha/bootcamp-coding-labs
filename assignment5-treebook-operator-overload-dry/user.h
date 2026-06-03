/*
 * Assignment 4: TreeBook
 * Adapted by Tinkercademy from Stanford CS106L Assignment 5
 * (originally by Fabio Ibanez, with modifications by Jacob Roberts-Baca).
 */

#include <iostream>
#include <string>
#include <utility>
#include "friend_list.h"

class User
{
public:
  User(const std::string& name);
  void add_friend(const std::string& name);
  std::string get_name() const;
  size_t size() const;
  void set_friend(size_t index, const std::string& name);

  /** 
   * STUDENT TODO:
   * Your custom operators and special member functions will go here!
   */
  friend std::ostream& operator<<(std::ostream& out, User const& user);
  bool operator<(const User& other) const;
  User& operator+=(User& other);

// ! LEARNING OPPORTUNITY HERE
// (1) if no SMFs declared: User's move ctor is implicitly-defined-as-deleted (coz FriendList is move=disabled)
//     BUT implicitly-deleted move is IGNORED by overload resolution, so T(rvalue) falls back to const T&
//     => copy ctor selected, move_constructible<User> is true - test fails

// (2) fix: explicitly =delete move ctor/= so they participate in overload resolution and hard-error on rvalue
//     BUT declaring move suppresses implicit copy, so must also explicitly =default copy ctor/=

  User(const User& user)            = default;
  User& operator=(const User& user) = default;

  User(User&& user)             = delete;
  User& operator=(User&& user)  = delete;
//   ~User();
//   // helpers
//   friend void swap(User& lhs, User& rhs) noexcept;
  
private:
  std::string _name;
  FriendList _friends;
};