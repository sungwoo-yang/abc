/*!
\file      SortedList.cpp
\par       CS280 Assignment 2
\author    YOUR NAME (YOUR.EMAIL@digipen.edu)
\date      DD-MM-YYYY
\par       Course: CS280
\copyright Copyright (C) 2026 DigiPen Institute of Technology

\brief Hours spent on this assignment: 9000

       Specific portions that gave you the most trouble:
           - If this was all easy, just say NONE
           - Otherwise, please fill this out!
*/

//******************************************************************************
//******************************************************************************
// static
template <typename T, typename Pred>
unsigned SortedList<T, Pred>::nodesize(void)
{
  return sizeof(Node);
}


//******************************************************************************
//******************************************************************************
template <typename T, typename Pred>
unsigned SortedList<T, Pred>::size(void) const
{
  return size_;
}

template <typename T, typename Pred>
template <typename Sorter>
void SortedList<T, Pred>::selection_sort(Sorter fn)
{
}

template <typename T, typename Pred>
template <typename Sorter>
void SortedList<T, Pred>::merge_sort(Sorter fn)
{
}
