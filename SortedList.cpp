/*!
\file      SortedList.cpp
\par       CS280 Assignment 2
\author    Sungwoo Yang (sungwoo.yang@digipen.edu)
\date      31-03-2026
\par       Course: CS280
\copyright Copyright (C) 2026 DigiPen Institute of Technology

\brief Hours spent on this assignment: 6

       Specific portions that gave you the most trouble:
           - Managing memory correctly using the custom ObjectAllocator and placement new.
*/
#include <new>

/*!
  \brief
    Constructor for the SortedList.

  \param sorter
    The functor used to sort the list.
  \param Allocator
    Pointer to a custom ObjectAllocator.
  \param SharedAllocator
    Flag indicating if the allocator is shared with other lists.
*/
template <typename T, typename Pred>
SortedList<T, Pred>::SortedList(Pred sorter, ObjectAllocator *Allocator, bool SharedAllocator)
    : head_(nullptr), tail_(nullptr), size_(0),
      objAllocator_(Allocator), freeAllocator_(false), shareAllocator_(SharedAllocator), sorter_(sorter)
{
  if (objAllocator_ == nullptr)
  {
    OAConfig config(true);
    objAllocator_ = new ObjectAllocator(nodesize(), config);
    freeAllocator_ = true;
  }
}

/*!
  \brief
    Copy constructor for the SortedList.

  \param rhs
    The SortedList object to copy from.
*/
template <typename T, typename Pred>
SortedList<T, Pred>::SortedList(const SortedList &rhs)
    : head_(nullptr), tail_(nullptr), size_(0),
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

  for (Node *curr = rhs.head_; curr; curr = curr->Next)
  {
    push_back(curr->Data);
  }
}

/*!
  \brief
    Destructor for the SortedList. Clears all nodes and frees the allocator if owned.
*/
template <typename T, typename Pred>
SortedList<T, Pred>::~SortedList()
{
  clear();
  if (freeAllocator_)
  {
    delete objAllocator_;
  }
}

/*!
  \brief
    Assignment operator for the SortedList.

  \param rhs
    The SortedList object to assign from.
  \return
    A reference to the updated SortedList.
*/
template <typename T, typename Pred>
SortedList<T, Pred> &SortedList<T, Pred>::operator=(const SortedList &rhs)
{
  if (this == &rhs)
    return *this;

  clear();
  for (Node *curr = rhs.head_; curr; curr = curr->Next)
  {
    push_back(curr->Data);
  }
  sorter_ = rhs.sorter_;

  return *this;
}

/*!
  \brief
    Subscript operator to access elements by index.

  \param index
    The position of the element to access.
  \return
    A constant reference to the element at the specified index.
*/
template <typename T, typename Pred>
const T &SortedList<T, Pred>::operator[](size_t index) const
{
  Node *curr = head_;
  for (size_t i = 0; i < index && curr; i++)
  {
    curr = curr->Next;
  }
  return curr->Data;
}

/*!
  \brief
    Adds a new node with the specified value to the end of the list.

  \param value
    The data value to be added.
  \exception SortedListException
    Thrown if memory allocation fails.
*/
template <typename T, typename Pred>
void SortedList<T, Pred>::push_back(const T &value)
{
  Node *newNode = nullptr;
  try
  {
    void *mem = objAllocator_->Allocate();
    newNode = new (mem) Node(value);

    newNode->Next = nullptr;
    newNode->Prev = nullptr;
  }
  catch (const OAException &e)
  {
    throw SortedListException(SortedListException::E_NO_MEMORY, e.what());
  }

  newNode->Prev = tail_;

  if (tail_)
    tail_->Next = newNode;
  else
    head_ = newNode;

  tail_ = newNode;
  size_++;
}

/*!
  \brief
    Inserts a new node with the specified value into its sorted position.

  \param value
    The data value to be inserted.
  \exception SortedListException
    Thrown if memory allocation fails.
*/
template <typename T, typename Pred>
void SortedList<T, Pred>::insert(const T &value)
{
  Node *newNode = nullptr;
  try
  {
    void *mem = objAllocator_->Allocate();
    newNode = new (mem) Node(value);

    newNode->Next = nullptr;
    newNode->Prev = nullptr;
  }
  catch (const OAException &e)
  {
    throw SortedListException(SortedListException::E_NO_MEMORY, e.what());
  }

  if (!head_)
  {
    head_ = tail_ = newNode;
  }
  else
  {
    Node *curr = head_;
    while (curr && sorter_(curr->Data, value))
    {
      curr = curr->Next;
    }

    if (!curr)
    {
      newNode->Prev = tail_;
      tail_->Next = newNode;
      tail_ = newNode;
    }
    else if (curr == head_)
    {
      newNode->Next = head_;
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
  size_++;
}

/*!
  \brief
    Returns the number of elements in the list.

  \return
    The size of the list.
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
  while (curr)
  {
    Node *next = curr->Next;
    curr->~Node();
    objAllocator_->Free(curr);
    curr = next;
  }
  head_ = tail_ = nullptr;
  size_ = 0;
}

/*!
  \brief
    Sorts the list using the selection sort algorithm.

  \param fn
    The functor used to compare elements during the sort.
*/
template <typename T, typename Pred>
template <typename Sorter>
void SortedList<T, Pred>::selection_sort(Sorter fn)
{
  if (!head_ || !head_->Next)
    return;

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
}

/*!
  \brief
    Helper function to merge two sorted linked lists.

  \param a
    The head of the first sorted list.
  \param b
    The head of the second sorted list.
  \param fn
    The functor used to compare elements.
  \return
    The head of the newly merged sorted list.
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

  \param h
    The head of the list to be sorted.
  \param fn
    The functor used to compare elements.
  \return
    The head of the sorted list.
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

  \param fn
    The functor used to compare elements during the sort.
*/
template <typename T, typename Pred>
template <typename Sorter>
void SortedList<T, Pred>::merge_sort(Sorter fn)
{
  if (!head_ || !head_->Next)
    return;

  head_ = merge_sort_recursive(head_, fn);

  Node *curr = head_;
  curr->Prev = nullptr;
  while (curr->Next)
  {
    curr->Next->Prev = curr;
    curr = curr->Next;
  }
  tail_ = curr;
}

/*!
  \brief
    Indicates whether the extra credit iterator was implemented.

  \return
    True if the extra credit is implemented, false otherwise.
*/
template <typename T, typename Pred>
bool SortedList<T, Pred>::ImplementedExtraCredit()
{
  return false;
}

/*!
  \brief
    Returns the size of a single Node.

  \return
    The size of the Node struct in bytes.
*/
template <typename T, typename Pred>
unsigned SortedList<T, Pred>::nodesize(void)
{
  return static_cast<unsigned>(sizeof(Node));
}