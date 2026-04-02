template <typename T, int Size = 1>
class BList
{
public:
    struct BNode
    {
        BNode *next;
        BNode *prev;
        int count; // number of items currently in the node
        T values[Size];
        BNode() : next(0), prev(0), count(0) {} // default ctor
    };
    Blist();                 // default constructor
    BList(const BList &rhs); // copy constructor
    ~Blist();                // destructor
    // arrays will be unsorted when using these methods
    void push_back(const T &value);
    void push_front(const T &value);
    // arrays will be sorted when using this method
    void insert(const T &value);
    void remove(int index);
    BList &operator=(const BList &rhs);
    size_t size() const;
    void clear();
    static size_t nodesize();

private:
    BNode *head_;
    BNode *tail_;
    // Other private fields and methods
};
#include "BList.cpp"