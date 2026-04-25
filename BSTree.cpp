/*!
 * \file    BSTree.cpp
 * \author  Sungwoo Yang
 * \brief   Implementation of the Binary Search Tree (BST) methods.
 *
 * Course     : CS280 Data Structures
 * Assignment : BST AVL
 * Term & Year: 2026 Spring
 */

//---------------------------------------------------------------------------
#ifndef BSTREECPP
#define BSTREECPP
//---------------------------------------------------------------------------

/*!
 * \brief Constructor for the BSTree.
 * \param oa Pointer to an existing ObjectAllocator.
 * \param ShareOA Boolean indicating whether the allocator is shared.
 */
template <typename T>
BSTree<T>::BSTree(ObjectAllocator *oa, bool ShareOA)
    : root_(nullptr), share_allocator_(ShareOA)
{
    if (oa)
    {
        allocator_ = oa;
    }
    else
    {
        OAConfig config;
        config.UseCPPMemManager_ = true;
        allocator_ = new ObjectAllocator(sizeof(BinTreeNode), config);
        share_allocator_ = false;
    }
}

/*!
 * \brief Copy constructor for the BSTree.
 * \param rhs The right-hand side BSTree to copy from.
 */
template <typename T>
BSTree<T>::BSTree(const BSTree &rhs)
    : root_(nullptr), allocator_(rhs.allocator_), share_allocator_(true)
{
    root_ = copy_tree(rhs.root_);
}

/*!
 * \brief Destructor for the BSTree. Clears nodes and deletes allocator if owned.
 */
template <typename T>
BSTree<T>::~BSTree()
{
    clear();
    if (!share_allocator_ && allocator_)
    {
        delete allocator_;
    }
}

/*!
 * \brief Assignment operator.
 * \param rhs The right-hand side BSTree to assign from.
 * \return Reference to the current tree.
 */
template <typename T>
BSTree<T> &BSTree<T>::operator=(const BSTree &rhs)
{
    if (this != &rhs)
    {
        clear();
        root_ = copy_tree(rhs.root_);
    }
    return *this;
}

/*!
 * \brief Deep copies a subtree.
 * \param tree The root of the subtree to copy.
 * \return Pointer to the newly created subtree root.
 */
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::copy_tree(const BinTree tree)
{
    if (!tree)
        return nullptr;
    BinTree new_node = make_node(tree->data);
    new_node->count = tree->count;
    new_node->balance_factor = tree->balance_factor;
    new_node->left = copy_tree(tree->left);
    new_node->right = copy_tree(tree->right);
    return new_node;
}

/*!
 * \brief Subscript operator to retrieve the node at the specified index.
 * \param index The 0-based index of the node to find.
 * \return Pointer to the found node or nullptr.
 */
template <typename T>
const typename BSTree<T>::BinTreeNode *BSTree<T>::operator[](int index) const
{
    BinTree current = root_;
    int curr_idx = index;

    while (current)
    {
        int left_subtree_count = current->left ? static_cast<int>(current->left->count) : 0;

        if (curr_idx == left_subtree_count)
        {
            return current;
        }
        if (curr_idx < left_subtree_count)
        {
            current = current->left;
        }
        else
        {
            curr_idx -= (left_subtree_count + 1);
            current = current->right;
        }
    }

    return reinterpret_cast<const BinTreeNode *>(this);
}

/*!
 * \brief Public interface to insert a value into the tree.
 * \param value The data to insert.
 */
template <typename T>
void BSTree<T>::insert(const T &value)
{
    insert_node(root_, value);
}

/*!
 * \brief Helper method to recursively insert a value.
 * \param tree Reference to the current node pointer.
 * \param val The data to insert.
 */
template <typename T>
void BSTree<T>::insert_node(BinTree &tree, const T &val)
{
    if (!tree)
    {
        tree = make_node(val);
        return;
    }
    if (val < tree->data)
    {
        insert_node(tree->left, val);
    }
    else if (val > tree->data)
    {
        insert_node(tree->right, val);
    }
    update_count(tree);
}

/*!
 * \brief Public interface to remove a value from the tree.
 * \param value The data to remove.
 */
template <typename T>
void BSTree<T>::remove(const T &value)
{
    remove_node(root_, value);
}

/*!
 * \brief Helper method to recursively remove a node.
 * \param tree Reference to the current node pointer.
 * \param val The data to remove.
 */
template <typename T>
void BSTree<T>::remove_node(BinTree &tree, const T &val)
{
    if (!tree)
        return;

    if (val < tree->data)
    {
        remove_node(tree->left, val);
    }
    else if (val > tree->data)
    {
        remove_node(tree->right, val);
    }
    else
    {
        if (!tree->left && !tree->right)
        {
            free_node(tree);
            tree = nullptr;
            return;
        }
        else if (!tree->left)
        {
            BinTree temp = tree;
            tree = tree->right;
            free_node(temp);
            return;
        }
        else if (!tree->right)
        {
            BinTree temp = tree;
            tree = tree->left;
            free_node(temp);
            return;
        }
        else
        {
            BinTree pred = nullptr;
            find_predecessor(tree->left, pred);
            tree->data = pred->data;
            remove_node(tree->left, tree->data);
        }
    }
    if (tree)
    {
        update_count(tree);
    }
}

/*!
 * \brief Clears the entire tree structure.
 */
template <typename T>
void BSTree<T>::clear()
{
    clear_tree(root_);
}

/*!
 * \brief Helper method to recursively clear nodes.
 * \param tree Reference to the current node pointer.
 */
template <typename T>
void BSTree<T>::clear_tree(BinTree &tree)
{
    if (tree)
    {
        clear_tree(tree->left);
        clear_tree(tree->right);
        free_node(tree);
        tree = nullptr;
    }
}

/*!
 * \brief Finds a value in the tree and tracks comparisons.
 * \param value The data to search for.
 * \param compares Counter reference for the number of comparisons made.
 * \return True if the value is found, false otherwise.
 */
template <typename T>
bool BSTree<T>::find(const T &value, unsigned &compares) const
{
    BinTree current = root_;
    while (true)
    {
        compares++;
        if (!current)
        {
            return false;
        }
        if (value == current->data)
        {
            return true;
        }
        else if (value < current->data)
        {
            current = current->left;
        }
        else
        {
            current = current->right;
        }
    }
}

/*!
 * \brief Checks if the tree is empty.
 * \return True if empty, false otherwise.
 */
template <typename T>
bool BSTree<T>::empty() const
{
    return root_ == nullptr;
}

/*!
 * \brief Returns the total number of nodes in the tree.
 * \return The total node count.
 */
template <typename T>
unsigned int BSTree<T>::size() const
{
    return root_ ? root_->count : 0;
}

/*!
 * \brief Returns the height of the tree.
 * \return Tree height (-1 if empty).
 */
template <typename T>
int BSTree<T>::height() const
{
    return tree_height(root_);
}

/*!
 * \brief Returns the root of the tree.
 * \return Pointer to the root node.
 */
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::root() const
{
    return root_;
}

/*!
 * \brief Indicates whether indexing is implemented.
 * \return True for this implementation.
 */
template <typename T>
bool BSTree<T>::ImplementedIndexing()
{
    return false;
}

/*!
 * \brief Returns a reference to the root pointer.
 * \return Reference to root_.
 */
template <typename T>
typename BSTree<T>::BinTree &BSTree<T>::get_root()
{
    return root_;
}

/*!
 * \brief Allocates and initializes a new node.
 * \param value The data to store.
 * \return Pointer to the new node.
 * \throws BSTException If memory allocation fails.
 */
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::make_node(const T &value) const
{
    try
    {
        void *mem = allocator_->Allocate();
        return new (mem) BinTreeNode(value);
    }
    catch (const BSTException &)
    {
        throw;
    }
    catch (...)
    {
        throw BSTException(BSTException::E_NO_MEMORY, "Out of memory in make_node");
    }
}

/*!
 * \brief Destroys and deallocates a node.
 * \param node Pointer to the node to free.
 */
template <typename T>
void BSTree<T>::free_node(BinTree node)
{
    if (node)
    {
        node->~BinTreeNode();
        allocator_->Free(node);
    }
}

/*!
 * \brief Helper method to calculate the height of a subtree.
 * \param tree The root of the subtree.
 * \return Height of the subtree.
 */
template <typename T>
int BSTree<T>::tree_height(BinTree tree) const
{
    if (!tree)
        return -1;
    int lh = tree_height(tree->left);
    int rh = tree_height(tree->right);
    return 1 + (lh > rh ? lh : rh);
}

/*!
 * \brief Finds the inorder predecessor of a given node.
 * \param tree The left child of the node to find the predecessor for.
 * \param predecessor Reference pointer to store the result.
 */
template <typename T>
void BSTree<T>::find_predecessor(BinTree tree, BinTree &predecessor) const
{
    predecessor = tree;
    while (predecessor && predecessor->right)
    {
        predecessor = predecessor->right;
    }
}

/*!
 * \brief Updates the node count based on its children.
 * \param tree Pointer to the node to update.
 */
template <typename T>
void BSTree<T>::update_count(BinTree tree)
{
    if (tree)
    {
        unsigned left_c = tree->left ? tree->left->count : 0;
        unsigned right_c = tree->right ? tree->right->count : 0;
        tree->count = left_c + right_c + 1;
    }
}

#endif
//---------------------------------------------------------------------------