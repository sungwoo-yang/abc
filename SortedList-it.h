//---------------------------------------------------------------------------
#ifndef SORTEDLISTH
#define SORTEDLISTH
//---------------------------------------------------------------------------

#include <string>     // error strings
#include <functional> // std::less

#include "ObjectAllocator.h"

/*!
  The exception class for the SortedList
*/
class SortedListException : public std::exception
{
  private:  
    int m_ErrCode;             //!< The specific error code
    std::string m_Description; //!< Human-readable string

  public:
    /*!
      Constructor

      \param ErrCode
        The error code which is either E_NO_MEMORY or E_CLIENT_ERROR

      \param Description
        The human-readable string
    */
    SortedListException(int ErrCode, const std::string& Description) :
    m_ErrCode(ErrCode), m_Description(Description) {};

    /*!
      Get the code from the exception object.

      \return
        The error code which is either E_NO_MEMORY or E_CLIENT_ERROR
    */
    virtual int code() const { 
      return m_ErrCode; 
    }

    /*!
      Get the human-readable string from the exception object

      \return
        The human-readable string
    */
    virtual const char *what() const noexcept {
      return m_Description.c_str();
    }

    /*!
      Destructor
    */
    virtual ~SortedListException() noexcept {
    }

    /*!
      The two kinds of exceptions.
      E_NO_MEMORY    - No memory (from ObjectAllocator)
      E_CLIENT_ERROR - Other error from client
    */
    enum SORTEDLIST_EXCEPTION {E_NO_MEMORY, E_CLIENT_ERROR};
};

//! A templatized linked-list that can be sorted
template <typename T, typename Pred = std::less<T> >
class SortedList
{
  struct Node; //!< Forward decl for the private Node
  public:
      // Constructor
    SortedList(Pred sorter = std::less<T>(), ObjectAllocator *Allocator = 0,
               bool SharedAllocator = false);

      // Copy constructor
    SortedList(const SortedList &rhs);

      // Destructor
    ~SortedList();
      
      // Assignment operator
    SortedList& operator=(const SortedList &rhs);

    //const T& operator[](size_t index) const; 

    void push_back(const T& value);
    void insert(const T& value);
    unsigned size() const;
    void clear();

    template <typename Sorter>
    void selection_sort(Sorter sorter = std::less<T>()); 

    template <typename Sorter>
    void merge_sort(Sorter sorter = std::less<T>());

    static bool ImplementedExtraCredit();

    static unsigned nodesize();

    //**************************************************
    //! const_iterator class for the extra credit
    class const_iterator
    {
      public:
        const_iterator(Node *p);                          // conversion ctor
        const T& operator*() const;                       // dereference op
        const_iterator& operator++();                     // pre-increment
        const_iterator& operator--();                     // pre-decrement
        bool operator!=(const const_iterator &rhs) const; // inequality op
      private:
        Node* current_; //!< pointer to the current node
    };

    /*!

      \return
        An iterator to the first node
    */
    const_iterator begin() const;  // the first node
    const_iterator end() const;    // one past the end
    const_iterator rbegin() const; // the last node
    const_iterator rend() const;   // one before the first
    //**************************************************

  private:
    //! The node structure used in the SortedList
    struct Node
    {
      Node *Next; //!< Pointer to the next node
      Node *Prev; //!< Pointer to the previous node
      T Data;     //!<  The payload
      /*!
        Constructor

        \param data
          The data to put into the node
      */
      Node(const T& data) : Data(data) {}
    };

      // Some "suggested" private data
      
    unsigned size_; //!< count of nodes
    Node *head_;    //!< the real head
    Node *tail_;    //!< the real tail
    Node *end_;     //!< for ++iter, this is the dummy tail
    Node *rend_;    //!< for --iter, this is the dummy head

      // Don't swap these
    ObjectAllocator *objAllocator_; //!< The OA to use/share
    bool freeAllocator_;            //!< Do we own it, then free it.
    bool shareAllocator_;           //!< Share it with copies?

    // Other private data and helper functions...

};

#include "SortedList.cpp"

#endif
