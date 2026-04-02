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
