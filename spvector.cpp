#include "spvector.h"
#include <iostream>
#include <iomanip>

/* definition of Node is hidden from the client (driver) so that client CANNOT
 * use INTERNALS of the Node.  this is poor-man encapsulation - hiding
 * implementation details.  The main reason for encapsulation -- if I ever
 * decide to change Node struct no client code will break. Example: I decide to
 * change "data" to "Value", all I do is change spvector.h and spvector.cpp, 
 * no client code is affected, since clients were FORCED to use function get(),
 * rather than structure member name "data".
 */
struct ElementNode {
  int    data;
  int    pos;
  struct ElementNode* next;
};

typedef struct ElementNode ElementNode;

// print functions are implemented for you ...
void print_elements_width(ConstElementNode_handle p_e, int width, int dim) {
	int last_pos=-1;
	while (p_e) {
		for (int i=last_pos+1;i<p_e->pos;++i) { std::cout << std::setw(width) << 0; }
    std::cout << std::setw(width) << p_e->data;
		last_pos=p_e->pos;
		p_e = p_e->next;
	}
	for (int i=last_pos+1;i<dim;++i) { std::cout << std::setw(width) << 0; }
}

void print_elements(ConstElementNode_handle p_e) {
	while (p_e) {
    std::cout << p_e->data << " at pos " << p_e->pos << ", ";
		p_e = p_e->next;
	}
}
