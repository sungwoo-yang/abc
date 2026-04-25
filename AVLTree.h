//---------------------------------------------------------------------------
#ifndef AVLTREEH
#define AVLTREEH
//---------------------------------------------------------------------------
#include <stack>
#include "BSTree.h"

/*!
  Definition for the AVL Tree 
*/
template <typename T>
class AVLTree : public BSTree<T>
{
  public:
    AVLTree(ObjectAllocator *oa = 0, bool ShareOA = false);
    virtual ~AVLTree() = default; // DO NOT IMPLEMENT
    virtual void insert(const T& value) override;
    virtual void remove(const T& value) override;

  private:
    // private stuff...

};

#include "AVLTree.cpp"

#endif
//---------------------------------------------------------------------------
