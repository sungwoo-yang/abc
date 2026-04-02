/*!
\file      SortedList.cpp
\par       CS280 Assignment 2
\author    Sungwoo Yang (sungwoo.yang@digipen.edu)
\date      31-03-2026
\par       Course: CS280
\copyright Copyright (C) 2026 DigiPen Institute of Technology

\brief Hours spent on this assignment: 6

       Specific portions that gave you the most trouble:
           - Implementing the dummy nodes (end_, rend_) and managing iterators.
*/
#include <new> 

/*!
  \brief
    Constructor for the SortedList.
  
  \param sorter The functor used to sort the list.
  \param Allocator Pointer to a custom ObjectAllocator.
  \param SharedAllocator Flag indicating if the allocator is shared.
*/
template <typename T, typename Pred>
SortedList<T, Pred>::SortedList(Pred sorter, ObjectAllocator *Allocator, bool SharedAllocator)
    : size_(0), head_(nullptr), tail_(nullptr),
      objAllocator_(Allocator), freeAllocator_(false), shareAllocator_(SharedAllocator), sorter_(sorter)
{
  if (objAllocator_ == nullptr)
  {
    OAConfig config(true); 
    objAllocator_ = new ObjectAllocator(nodesize(), config);
    freeAllocator_ = true;
  }

  end_ = reinterpret_cast<Node*>(objAllocator_->Allocate());
  rend_ = reinterpret_cast<Node*>(objAllocator_->Allocate());
  end_->Next = nullptr;
  end_->Prev = rend_;
  rend_->Next = end_;
  rend_->Prev = nullptr;
}

/*!
  \brief
    Copy constructor for the SortedList.
  
  \param rhs The SortedList object to copy from.
*/
template <typename T, typename Pred>
SortedList<T, Pred>::SortedList(const SortedList &rhs)
    : size_(0), head_(nullptr), tail_(nullptr),
      objAllocator_(rhs.objAllocator_), freeAllocator_(false), shareAllocator_(rhs.shareAllocator_), sorter_(rhs.sorter_)
{
  if (rhs.shareAllocator_)
  {
    objAllocator_ = rhs.objAllocator_;
    freeAllocator_ = false;
  }
  else
  {
    OAConfig config(true);
    objAllocator_ = new ObjectAllocator(nodesize(), config);
    freeAllocator_ = true;
  }

  end_ = reinterpret_cast<Node*>(objAllocator_->Allocate());
  rend_ = reinterpret_cast<Node*>(objAllocator_->Allocate());
  end_->Next = nullptr;
  end_->Prev = rend_;
  rend_->Next = end_;
  rend_->Prev = nullptr;

  for (Node *curr = rhs.head_; curr && curr != rhs.end_; curr = curr->Next)
  {
    push_back(curr->Data);
  }
}

/*!
  \brief
    Destructor for the SortedList. Clears all nodes and frees the allocator.
*/
template <typename T, typename Pred>
SortedList<T, Pred>::~SortedList()
{
  clear();
  objAllocator_->Free(end_);
  objAllocator_->Free(rend_);
  
  if (freeAllocator_)
  {
    delete objAllocator_;
  }
}

/*!
  \brief
    Assignment operator for the SortedList.
  
  \param rhs The SortedList object to assign from.
  \return A reference to the updated SortedList.
*/
template <typename T, typename Pred>
SortedList<T, Pred> &SortedList<T, Pred>::operator=(const SortedList &rhs)
{
  if (this == &rhs)
    return *this;

  clear();
  for (Node *curr = rhs.head_; curr && curr != rhs.end_; curr = curr->Next)
  {
    push_back(curr->Data);
  }
  sorter_ = rhs.sorter_;

  return *this;
}
/*!
  \brief
    Adds a new node with the specified value to the end of the list.
  
  \param value The data value to be added.
*/
template <typename T, typename Pred>
void SortedList<T, Pred>::push_back(const T &value)
{
  Node *newNode = nullptr;
  try
  {
    void *mem = objAllocator_->Allocate();
    newNode = new (mem) Node(value);
  }
  catch (const OAException &e)
  {
    throw SortedListException(SortedListException::E_NO_MEMORY, e.what());
  }

  newNode->Prev = tail_ ? tail_ : rend_;
  newNode->Next = end_;

  if (tail_)
    tail_->Next = newNode;
  else
    head_ = newNode;

  tail_ = newNode;
  end_->Prev = tail_;
  rend_->Next = head_;
  size_++;
}

/*!
  \brief
    Inserts a new node with the specified value into its sorted position.
  
  \param value The data value to be inserted.
*/
template <typename T, typename Pred>
void SortedList<T, Pred>::insert(const T &value)
{
  Node *newNode = nullptr;
  try
  {
    void *mem = objAllocator_->Allocate();
    newNode = new (mem) Node(value);
  }
  catch (const OAException &e)
  {
    throw SortedListException(SortedListException::E_NO_MEMORY, e.what());
  }

  if (!head_)
  {
    head_ = tail_ = newNode;
    newNode->Prev = rend_;
    newNode->Next = end_;
  }
  else
  {
    Node *curr = head_;
    while (curr != end_ && sorter_(curr->Data, value))
    {
      curr = curr->Next;
    }

    if (curr == end_)
    {
      newNode->Prev = tail_;
      newNode->Next = end_;
      tail_->Next = newNode;
      tail_ = newNode;
    }
    else if (curr == head_)
    { 
      newNode->Next = head_;
      newNode->Prev = rend_;
      head_->Prev = newNode;
      head_ = newNode;
    }
    else
    { 
      newNode->Next = curr;
      newNode->Prev = curr->Prev;
      curr->Prev->Next = newNode;
      curr->Prev = newNode;
    }
  }
  
  end_->Prev = tail_;
  rend_->Next = head_;
  size_++;
}

/*!
  \brief
    Returns the number of elements in the list.
  
  \return The size of the list.
*/
template <typename T, typename Pred>
unsigned SortedList<T, Pred>::size(void) const
{
  return size_;
}

/*!
  \brief
    Clears all elements from the list and frees memory using the allocator.
*/
template <typename T, typename Pred>
void SortedList<T, Pred>::clear()
{
  Node *curr = head_;
  while (curr && curr != end_)
  {
    Node *next = curr->Next;
    curr->~Node();
    objAllocator_->Free(curr);
    curr = next;
  }
  head_ = nullptr;
  tail_ = nullptr;
  size_ = 0;
  
  end_->Prev = rend_;
  rend_->Next = end_;
}

/*!
  \brief
    Sorts the list using the selection sort algorithm.
  
  \param fn The functor used to compare elements during the sort.
*/
template <typename T, typename Pred>
template <typename Sorter>
void SortedList<T, Pred>::selection_sort(Sorter fn)
{
  if (!head_ || head_ == tail_)
    return;

  tail_->Next = nullptr;
  head_->Prev = nullptr;

  Node *sortedTail = nullptr;
  Node *unsortedHead = head_;

  while (unsortedHead)
  {
    Node *curr = unsortedHead;
    Node *minNode = curr;

    while (curr)
    {
      if (fn(curr->Data, minNode->Data))
      {
        minNode = curr;
      }
      curr = curr->Next;
    }

    if (minNode == unsortedHead)
    {
      unsortedHead = unsortedHead->Next;
    }
    else
    {
      minNode->Prev->Next = minNode->Next;
      if (minNode->Next)
        minNode->Next->Prev = minNode->Prev;
    }

    minNode->Next = nullptr;
    minNode->Prev = sortedTail;

    if (sortedTail)
      sortedTail->Next = minNode;
    else
      head_ = minNode;

    sortedTail = minNode;
  }
  tail_ = sortedTail;

  head_->Prev = rend_;
  tail_->Next = end_;
  rend_->Next = head_;
  end_->Prev = tail_;
}

/*!
  \brief
    Helper function to merge two sorted linked lists.
  
  \param a The head of the first sorted list.
  \param b The head of the second sorted list.
  \param fn The functor used to compare elements.
  \return The head of the newly merged sorted list.
*/
template <typename T, typename Pred>
template <typename Sorter>
typename SortedList<T, Pred>::Node *SortedList<T, Pred>::merge(Node *a, Node *b, Sorter fn)
{
  Node *mergeHead = nullptr;
  Node **mergeTail = &mergeHead;

  while (a && b)
  {
    if (fn(b->Data, a->Data))
    {
      *mergeTail = b;
      b = b->Next;
    }
    else
    {
      *mergeTail = a;
      a = a->Next;
    }
    mergeTail = &((*mergeTail)->Next);
  }

  *mergeTail = (a ? a : b);
  return mergeHead;
}

/*!
  \brief
    Recursive helper function to divide and sort the list.
  
  \param h The head of the list to be sorted.
  \param fn The functor used to compare elements.
  \return The head of the sorted list.
*/
template <typename T, typename Pred>
template <typename Sorter>
typename SortedList<T, Pred>::Node *SortedList<T, Pred>::merge_sort_recursive(Node *h, Sorter fn)
{
  if (!h || !h->Next)
    return h;

  Node *slow = h;
  Node *fast = h->Next;

  while (fast && fast->Next)
  {
    slow = slow->Next;
    fast = fast->Next->Next;
  }

  Node *mid = slow->Next;
  slow->Next = nullptr;

  Node *left = merge_sort_recursive(h, fn);
  Node *right = merge_sort_recursive(mid, fn);

  return merge(left, right, fn);
}

/*!
  \brief
    Sorts the list using the merge sort algorithm.
  
  \param fn The functor used to compare elements during the sort.
*/
template <typename T, typename Pred>
template <typename Sorter>
void SortedList<T, Pred>::merge_sort(Sorter fn)
{
  if (!head_ || head_ == tail_)
    return;

  tail_->Next = nullptr;
  head_->Prev = nullptr;

  head_ = merge_sort_recursive(head_, fn);

  Node *curr = head_;
  curr->Prev = nullptr;
  while (curr->Next)
  {
    curr->Next->Prev = curr;
    curr = curr->Next;
  }
  tail_ = curr;

  head_->Prev = rend_;
  tail_->Next = end_;
  rend_->Next = head_;
  end_->Prev = tail_;
}

/*!
  \brief
    Indicates whether the extra credit iterator was implemented.
  
  \return True, as the extra credit (Iterator) is implemented.
*/
template <typename T, typename Pred>
bool SortedList<T, Pred>::ImplementedExtraCredit()
{
  return true;
}

/*!
  \brief
    Returns the size of a single Node.
  
  \return The size of the Node struct in bytes.
*/
template <typename T, typename Pred>
unsigned SortedList<T, Pred>::nodesize(void)
{
  return static_cast<unsigned>(sizeof(Node));
}

/*!
  \brief
    Constructor for the const_iterator.
  
  \param p Pointer to the current node.
*/
template <typename T, typename Pred>
SortedList<T, Pred>::const_iterator::const_iterator(Node *p) : current_(p) 
{
}

/*!
  \brief
    Dereference operator.
  
  \return A constant reference to the data in the current node.
*/
template <typename T, typename Pred>
const T& SortedList<T, Pred>::const_iterator::operator*() const 
{
  return current_->Data;
}

/*!
  \brief
    Pre-increment operator.
  
  \return A reference to the updated iterator.
*/
template <typename T, typename Pred>
typename SortedList<T, Pred>::const_iterator& SortedList<T, Pred>::const_iterator::operator++() 
{
  current_ = current_->Next;
  return *this;
}

/*!
  \brief
    Pre-decrement operator.
  
  \return A reference to the updated iterator.
*/
template <typename T, typename Pred>
typename SortedList<T, Pred>::const_iterator& SortedList<T, Pred>::const_iterator::operator--() 
{
  current_ = current_->Prev;
  return *this;
}

/*!
  \brief
    Inequality operator.
  
  \param rhs The other iterator to compare against.
  \return True if the iterators point to different nodes, false otherwise.
*/
template <typename T, typename Pred>
bool SortedList<T, Pred>::const_iterator::operator!=(const const_iterator &rhs) const 
{
  return current_ != rhs.current_;
}

/*!
  \brief
    Gets an iterator to the first real node in the list.
  
  \return An iterator to the beginning of the list.
*/
template <typename T, typename Pred>
typename SortedList<T, Pred>::const_iterator SortedList<T, Pred>::begin() const 
{
  return head_ ? const_iterator(head_) : const_iterator(end_);
}

/*!
  \brief
    Gets an iterator to the dummy end node.
  
  \return An iterator to one past the end of the list.
*/
template <typename T, typename Pred>
typename SortedList<T, Pred>::const_iterator SortedList<T, Pred>::end() const 
{
  return const_iterator(end_);
}

/*!
  \brief
    Gets an iterator to the last real node in the list.
  
  \return An iterator to the end of the list (reverse begin).
*/
template <typename T, typename Pred>
typename SortedList<T, Pred>::const_iterator SortedList<T, Pred>::rbegin() const 
{
  return tail_ ? const_iterator(tail_) : const_iterator(rend_);
}

/*!
  \brief
    Gets an iterator to the dummy rend node.
  
  \return An iterator to one before the beginning of the list.
*/
template <typename T, typename Pred>
typename SortedList<T, Pred>::const_iterator SortedList<T, Pred>::rend() const 
{
  return const_iterator(rend_);
}