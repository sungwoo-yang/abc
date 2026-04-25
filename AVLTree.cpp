/*!
 * \file    AVLTree.cpp
 * \author  Sungwoo Yang
 * \brief   Implementation of the AVL Tree balancing methods.
 *
 * Course     : CS280 Data Structures
 * Assignment : BST AVL
 * Term & Year: 2026 Spring
 */

//---------------------------------------------------------------------------
#ifndef AVLTREECPP
#define AVLTREECPP
//---------------------------------------------------------------------------

/*!
 * \brief Constructor for the AVLTree.
 * \param oa Pointer to an existing ObjectAllocator.
 * \param ShareOA Boolean indicating whether the allocator is shared.
 */
template <typename T>
AVLTree<T>::AVLTree(ObjectAllocator *oa, bool ShareOA)
    : BSTree<T>(oa, ShareOA)
{
}

/*!
 * \brief Inserts a value into the AVL tree maintaining balance.
 * \param value The data to insert.
 */
template <typename T>
void AVLTree<T>::insert(const T &value)
{
    insert_avl(this->get_root(), value);
}

/*!
 * \brief Helper method for AVL insertion.
 * \param tree Reference to the current node pointer.
 * \param val The data to insert.
 */
template <typename T>
void AVLTree<T>::insert_avl(BinTree &tree, const T &val)
{
    if (!tree)
    {
        tree = this->make_node(val);
        return;
    }

    if (val < tree->data)
    {
        insert_avl(tree->left, val);
    }
    else if (val > tree->data)
    {
        insert_avl(tree->right, val);
    }
    else
    {
        return;
    }

    update_properties(tree);
    balance_tree(tree);
}

/*!
 * \brief Removes a value from the AVL tree maintaining balance.
 * \param value The data to remove.
 */
template <typename T>
void AVLTree<T>::remove(const T &value)
{
    remove_avl(this->get_root(), value);
}

/*!
 * \brief Helper method for AVL removal.
 * \param tree Reference to the current node pointer.
 * \param val The data to remove.
 */
template <typename T>
void AVLTree<T>::remove_avl(BinTree &tree, const T &val)
{
    if (!tree)
        return;

    if (val < tree->data)
    {
        remove_avl(tree->left, val);
    }
    else if (val > tree->data)
    {
        remove_avl(tree->right, val);
    }
    else
    {
        if (!tree->left || !tree->right)
        {
            BinTree temp = tree->left ? tree->left : tree->right;
            if (!temp)
            {
                temp = tree;
                tree = nullptr;
            }
            else
            {
                *tree = *temp;
            }
            this->free_node(temp);
        }
        else
        {
            BinTree pred = nullptr;
            this->find_predecessor(tree->left, pred);
            tree->data = pred->data;
            remove_avl(tree->left, pred->data);
        }
    }

    if (!tree)
        return;

    update_properties(tree);
    balance_tree(tree);
}

/*!
 * \brief Checks the balance factor and performs rotations if necessary.
 * \param tree Reference to the node to balance.
 */
template <typename T>
void AVLTree<T>::balance_tree(BinTree &tree)
{
    if (!tree)
        return;

    int balance = get_balance(tree);

    if (balance > 1)
    {
        if (get_balance(tree->left) < 0)
        {
            rotate_left(tree->left);
        }
        rotate_right(tree);
    }
    else if (balance < -1)
    {
        if (get_balance(tree->right) > 0)
        {
            rotate_right(tree->right);
        }
        rotate_left(tree);
    }
}

/*!
 * \brief Performs a right rotation on the given node.
 * \param tree Reference to the root of the unbalanced subtree.
 */
template <typename T>
void AVLTree<T>::rotate_right(BinTree &tree)
{
    BinTree pivot = tree->left;
    BinTree temp = pivot->right;

    pivot->right = tree;
    tree->left = temp;

    update_properties(tree);
    update_properties(pivot);

    tree = pivot;
}

/*!
 * \brief Performs a left rotation on the given node.
 * \param tree Reference to the root of the unbalanced subtree.
 */
template <typename T>
void AVLTree<T>::rotate_left(BinTree &tree)
{
    BinTree pivot = tree->right;
    BinTree temp = pivot->left;

    pivot->left = tree;
    tree->right = temp;

    update_properties(tree);
    update_properties(pivot);

    tree = pivot;
}

/*!
 * \brief Retrieves the height of a node safely.
 * \param tree The node to check.
 * \return The height, or -1 if the node is null.
 */
template <typename T>
int AVLTree<T>::get_height(BinTree tree) const
{
    return tree ? this->tree_height(tree) : -1;
}

/*!
 * \brief Calculates the balance factor of a node.
 * \param tree The node to check.
 * \return The balance factor (left height - right height).
 */
template <typename T>
int AVLTree<T>::get_balance(BinTree tree) const
{
    if (!tree)
        return 0;
    return get_height(tree->left) - get_height(tree->right);
}

/*!
 * \brief Updates the count and balance factor properties of a node.
 * \param tree The node to update.
 */
template <typename T>
void AVLTree<T>::update_properties(BinTree tree)
{
    if (tree)
    {
        unsigned left_c = tree->left ? tree->left->count : 0;
        unsigned right_c = tree->right ? tree->right->count : 0;
        tree->count = left_c + right_c + 1;
        tree->balance_factor = get_balance(tree);
    }
}

#endif
//---------------------------------------------------------------------------