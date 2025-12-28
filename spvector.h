#ifndef SPARSE_VECTOR_H
#define SPARSE_VECTOR_H

/* Since definition of Node is hidden from the client we need to provide some
* way to use Node struct, and here we do it through typedef.  At this point Node is
* NOT KNOWN to the compiler (which makes Node an INCOMPLETE TYPE, but
* declaring a pointer to an incomplete type is allowed by C/C++.
* See http://en.wikipedia.org/wiki/Opaque_pointer for more information
* about opaque pointers.
*/
 
typedef struct ElementNode *       ElementNode_handle;
// ConstElementNode_handle is "non-const-pointer-to-const-object-of-type struct ElementNode"
typedef struct ElementNode const * ConstElementNode_handle; // see explanation below

/*
* We're using storage specifier typedef to create new names for existing types.
* Although typedefs do simplify type names, they still have the same confusing
* syntax as other declarations. Consider the following declaration:
* 
* typedef pint int*;
* const pint ci;
*
* What is the type of ci? Using textual expansion, it would seem to logically
* be const int*
* No!!!
* Remember const-ness is applied to the object of type pint [which is int*].
* Therefore the declaration "const pint ci;" declares ci to be a "const-pointer-to-an-int."
* 
* Hence, we also need a special type ConstElementNode_handle because even though
* "struct ElementNode*" is same as "ElementNode_handle", "const struct ElementNode*"
* and "const ElementNode_handle" are DIFFERENT [as explained earlier]!!!
* Let's analyze this again:
* The declaration "const struct ElementNode* p;" declares p to be non-const pointer to
* a const object of type "struct ElementNode" while
* "const ElementNode_handle p;" declares p to be a "const-pointer-to-an-object-of-type struct ElementNode."
*/

/*===================*
 * element functions *
 *===================*/

/* print elements as a vector, adding 0's for non-existing positions
* assume the length of the vector is specified by parameter dim
* doesn't check if the list contains positions beyond dim
*/
void print_elements_width( ConstElementNode_handle, int width, int dim); 

/* 
* print all elements in the list with positions [doesn't add 0's for non-existing positions]
*/
void print_elements(ConstElementNode_handle);

/* insert an element into a list 
* list is ordered using pos
* if position pos is already occupied, the value of the node
* should be updated with val
* if val=0, then the element should be deleted
* return 0 if operation is succesfull 
*        1 if malloc failed
*/
int insert_element(ElementNode_handle *,int,int);

/* 
* delete an element at position pos if it exists
*/
void delete_element( ElementNode_handle *,int );

/*
* get the value at the given position,
* p_e is the head pointer 
*/
int  get( ConstElementNode_handle, int );

/* 
* scalar product of 2 lists.
*
*/
int scalar_product( ConstElementNode_handle, ConstElementNode_handle );

/* 
* adds 2 lists as vectors, returns a new list
*/
ElementNode_handle add( ConstElementNode_handle,ConstElementNode_handle );

/* 
* deallocate a list
*/
void free_elements( ElementNode_handle );

#endif
