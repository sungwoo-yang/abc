/*!
 * \file    AVLTree.h
 * \author  Sungwoo Yang
 * \brief   AVL Tree class definition extending BSTree.
 *
 * Course     : CS280 Data Structures
 * Assignment : BST AVL
 * Term & Year: 2026 Spring
 */

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
  virtual void insert(const T &value) override;
  virtual void remove(const T &value) override;

private:
  typedef typename BSTree<T>::BinTree BinTree;

  void insert_avl(BinTree &tree, const T &val);
  void remove_avl(BinTree &tree, const T &val);

  void balance_tree(BinTree &tree);
  void rotate_left(BinTree &tree);
  void rotate_right(BinTree &tree);

  int get_height(BinTree tree) const;
  int get_balance(BinTree tree) const;
  void update_properties(BinTree tree);
};

#include "AVLTree.cpp"

#endif
//---------------------------------------------------------------------------
