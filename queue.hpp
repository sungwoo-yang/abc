#ifndef QUEUE_HPP
#define QUEUE_HPP

namespace HLP3 {

class Queue {
public:
  using value_type = char;
  using reference  = value_type&;
  using const_reference = const value_type&;
  using pointer    = value_type*;
  using const_pointer = const value_type*;
  using size_type = unsigned long;

// provide rest of interface ...
// don't declare any data members or anything else to be of type short or int ...

// if size_type is changed from unsigned long to unsigned int, your implementation
// should work perfectly without any other edits to queue.hpp and queue.cpp ...

// if value_type is changed from char to any other type, your implementation
// should work perfectly without any other edits to queue.hpp and queue.cpp ...
};

// declare necessary non-member function(s) ...

}
#endif
