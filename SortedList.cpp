/*!
\file      SortedList.cpp
\par       CS280 Assignment 2
\author    Sungwoo Yang (sungwoo.yang@digipen.edu)
\date      31-03-2026
\par       Course: CS280
\copyright Copyright (C) 2026 DigiPen Institute of Technology

\brief Hours spent on this assignment: 6

       Specific portions that gave you the most trouble:
           - If this was all easy, just say NONE
           - Otherwise, please fill this out!
*/
#include <new> // for placement new

//******************************************************************************
// Constructor
//******************************************************************************
template <typename T, typename Pred>
SortedList<T, Pred>::SortedList(Pred sorter, ObjectAllocator *Allocator, bool SharedAllocator)
    : head_(nullptr), tail_(nullptr), size_(0),
      objAllocator_(Allocator), freeAllocator_(false), shareAllocator_(SharedAllocator), sorter_(sorter)
{
  if (objAllocator_ == nullptr)
  {
    OAConfig config(true); // UseCPPMemManager = true
    objAllocator_ = new ObjectAllocator(nodesize(), config);
    freeAllocator_ = true;
  }
}

//******************************************************************************
// Copy Constructor
//******************************************************************************
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

//******************************************************************************
// Destructor
//******************************************************************************
template <typename T, typename Pred>
SortedList<T, Pred>::~SortedList()
{
  clear();
  if (freeAllocator_)
  {
    delete objAllocator_;
  }
}

//******************************************************************************
// Assignment Operator
//******************************************************************************
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

//******************************************************************************
// Subscript operator
//******************************************************************************
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

//******************************************************************************
// push_back
//******************************************************************************
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

//******************************************************************************
// insert
//******************************************************************************
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
    { // Insert at head
      newNode->Next = head_;
      head_->Prev = newNode;
      head_ = newNode;
    }
    else
    { // Insert before curr
      newNode->Next = curr;
      newNode->Prev = curr->Prev;
      curr->Prev->Next = newNode;
      curr->Prev = newNode;
    }
  }
  size_++;
}

//******************************************************************************
// size
//******************************************************************************
template <typename T, typename Pred>
unsigned SortedList<T, Pred>::size(void) const
{
  return size_;
}

//******************************************************************************
// clear
//******************************************************************************
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

//******************************************************************************
// selection_sort
//******************************************************************************
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

//******************************************************************************
// merge helper
//******************************************************************************
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

//******************************************************************************
// merge_sort_recursive helper
//******************************************************************************
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

//******************************************************************************
// merge_sort
//******************************************************************************
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

//******************************************************************************
// ImplementedExtraCredit
//******************************************************************************
template <typename T, typename Pred>
bool SortedList<T, Pred>::ImplementedExtraCredit()
{
  return false;
}

//******************************************************************************
// nodesize
//******************************************************************************
template <typename T, typename Pred>
unsigned SortedList<T, Pred>::nodesize(void)
{
  return static_cast<unsigned>(sizeof(Node));
}