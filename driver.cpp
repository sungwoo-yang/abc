#include "spvector.h"
#include <iostream>
#include <string>

namespace {
void test0(); void test1();  void test2();  void test3();
void test4(); void test5();  void test6();  void test7();
void test8();  void test9(); void test10(); void test11(); void test12();
}

int main( int argc, char ** argv ) {
  void (*pTests[])() = { 
    test0, test1, test2, test3, test4,  test5,
    test6, test7, test8, test9, test10, test11, test12
  };

  if (argc > 1) {
    int test = std::stoi(std::string(argv[1]));
    if (test < 13) {
      pTests[test]();
    } else {
      for (size_t i{}; i < sizeof(pTests)/sizeof(pTests[0]); ++i) {
        pTests[i]();
      }
    }
  }
}

namespace {
void test0() {
  std::cout << "\n====== INSERTS BACK =============================\n";
  ElementNode_handle pE1=0;
  for(int i{};i<20;++i) insert_element(&pE1,3*i,i*i);
  std::cout << "vector 1 formatted:\n";
  print_elements_width(pE1, 4, 80); std::cout << "\n";
  std::cout << "vector 1 raw:\n";
  print_elements(pE1); std::cout << "\n";

  free_elements(pE1);
}

void test1() {
  std::cout << "\n====== INSERTS BACK =============================\n";
  ElementNode_handle pE2=0;
  for(int i{};i<20;++i) insert_element(&pE2,20-i,i);
  std::cout << "vector 2 formatted:\n";
  print_elements_width(pE2,4,20); std::cout << "\n";
  std::cout << "vector 2 raw:\n";
  print_elements(pE2); std::cout << "\n";
 
  free_elements(pE2);
}

void test2() {
  std::cout << "\n====== INSERTS MIDDLE =============================\n";
  ElementNode_handle pE3=0;
  for(int i{};i<20;++i) {
    if (i%2) insert_element(&pE3,i,i);
    else     insert_element(&pE3,20-i,-i);
  }
  std::cout << "vector 3 formatted:\n";
  print_elements_width(pE3,4,20); std::cout << "\n";
  std::cout << "vector 3 raw:\n";
  print_elements(pE3); std::cout << "\n";

  free_elements(pE3);
}

void test3() {
  printf("\n====== DELETE FRONT ====================\n");
  ElementNode_handle pE2=0;
  for(int i{};i<5;++i) insert_element(&pE2,i,i+1); // no 0s in the list
  std::cout << "vector 2 formatted:\n";
  print_elements_width(pE2,4,5); std::cout << "\n";
  std::cout << "vector 2 raw:\n";
  print_elements(pE2); std::cout << "\n";
 
  delete_element(&pE2,0);
  std::cout << "vector 2 formatted:\n";
  print_elements_width(pE2,4,5); std::cout << "\n";
  std::cout << "vector 2 raw:\n";
  print_elements(pE2); std::cout << "\n";

  free_elements(pE2);
}

void test4() {
  std::cout << "\n====== DELETE BACK ====================\n";
  ElementNode_handle pE2=0;
  for(int i{};i<5;++i) insert_element(&pE2,i,i+1); // no 0s in the list
  std::cout << "vector 2 formatted:\n";
  print_elements_width(pE2,4,5); std::cout << "\n";
  std::cout << "vector 2 raw:\n";
  print_elements(pE2); std::cout << "\n";
 
  delete_element(&pE2,4);
  std::cout << "vector 2 formatted:\n";
  print_elements_width(pE2,4,5); std::cout << "\n";
  std::cout << "vector 2 raw:\n";
  print_elements(pE2); std::cout << "\n";

  free_elements(pE2);
}

void test5() {
  std::cout << "\n====== DELETE MIDDLE ====================\n";
  ElementNode_handle pE2=0;
  for(int i{};i<5;++i) insert_element(&pE2,i,i+1); // no 0s in the list
  std::cout << "vector 2 formatted:\n";
  print_elements_width(pE2,4,5); std::cout << "\n";
  std::cout << "vector 2 raw:\n";
  print_elements(pE2); std::cout << "\n";
 
  delete_element(&pE2,2);
  std::cout << "vector 2 formatted:\n";
  print_elements_width(pE2,4,5); std::cout << "\n";
  std::cout << "vector 2 raw:\n";
  print_elements(pE2); std::cout << "\n";

  free_elements(pE2);
}

void test6() {
  std::cout << "\n==== SCALAR MULTIPLICATION =================\n";
  ElementNode_handle pE1=0,pE2=0,pE3=0;
  for(int i{};i<20;++i) insert_element(&pE1,i,i*i);
  std::cout << "vector pE1 formatted:\n";
  print_elements_width(pE1,4,20); std::cout << "\n";

  for(int i{};i<20;++i) insert_element(&pE2,40-2*i,i);
  std::cout << "vector pE2 formatted:\n";
  print_elements_width(pE2,4,40); std::cout << "\n";
 
  for(int i{};i<20;++i) {
    if (i%2) insert_element(&pE3,2*i,i);
    else     insert_element(&pE3,41-2*i,-i);
  }
  std::cout << "vector pE3 formatted:\n";
  print_elements_width(pE3,4,40); std::cout << "\n";
  std::cout << "scalar product pE1 and pE2 = " << scalar_product(pE1,pE2) << "\n";
  std::cout << "scalar product pE2 and pE3 = " << scalar_product(pE2,pE3) << "\n";
  std::cout << "scalar product pE3 and pE1 = " << scalar_product(pE3,pE1) << "\n";
  std::cout << "scalar product pE1 and pE1 = " << scalar_product(pE1,pE1) << "\n";

  free_elements(pE1);
  free_elements(pE2);
  free_elements(pE3);
}

void test7() {
  std::cout << "\n==== SCALAR MULTIPLICATION 2 =================\n";
  ElementNode_handle pE1=0,pE2=0;
  for(int i{};i<20;++i) insert_element(&pE1,1000*i,i*i);
  for(int i{};i<20;++i) insert_element(&pE2,500*i,i);
 
  std::cout << "scalar product pE1 and pE2 = " << scalar_product(pE1,pE2) << "\n";
  std::cout << "scalar product pE2 and pE1 = " << scalar_product(pE2,pE1) << "\n";
  std::cout << "scalar product pE1 and pE1 = " << scalar_product(pE1,pE1) << "\n";

  free_elements(pE1);
  free_elements(pE2);
}

void test8() {
  std::cout << "\n==== VECTOR ADDITION =================\n";
  ElementNode_handle pE1=0,pE2=0,pE3=0,pRes;
  for(int i{};i<20;++i) insert_element(&pE1,i,i*i);
  std::cout << "vector pE1 formatted:\n";
  print_elements_width(pE1,4,20); std::cout << "\n";

  for(int i{};i<10;++i) insert_element(&pE2,20-2*i,i);
  std::cout << "vector pE2 formatted:\n";
  print_elements_width(pE2,4,20); std::cout << "\n";
 
  for(int i{};i<5;++i) {
    if (i%2) insert_element(&pE3,4*i,i);
    else     insert_element(&pE3,21-4*i,-i);
  }
  std::cout << "vector pE3 formatted:\n";
  print_elements_width(pE3,4,20); std::cout << "\n";

  pRes=add(pE1,pE2);
  std::cout << "pE1 + pE2 = \n"; print_elements_width(pRes,4,20); std::cout << "\n";
  // std::cout << "raw:\n"; print_elements(pRes); std::cout << "\n";
  free_elements(pRes);
  pRes=add(pE2,pE3);
  std::cout << "pE2 + pE3 = \n"; print_elements_width(pRes,4,20); std::cout << "\n";
  // std::cout << "raw:\n"; print_elements(pRes); std::cout << "\n";
  free_elements(pRes);
  pRes=add(pE3,pE1);
  std::cout << "pE3 + pE1 = \n"; print_elements_width(pRes,4,20); std::cout << "\n";
  // std::cout << "raw:\n"; print_elements(pRes); std::cout << "\n";
  free_elements(pRes);
  pRes=add(pE1,pE3);
  std::cout << "pE1 + pE3 = \n"; print_elements_width(pRes,4,20); std::cout << "\n";
  // std::cout << "raw:\n"; print_elements(pRes); std::cout << "\n";
  free_elements(pRes);
  free_elements(pE1);
  free_elements(pE2);
  free_elements(pE3);
}

void test9() {
  std::cout << "\n==== VECTOR ADDITION 2 =================\n";
  ElementNode_handle pE1=0,pE2=0,pRes;
  for(int i{};i<5;++i) insert_element(&pE1,2000*i,i);
  for(int i{};i<10;++i) insert_element(&pE2,1000*i,i);
 
  pRes=add(pE1,pE2);
  std::cout << "pE1 + pE2 = \n"; 
  std::cout << "raw:\n"; print_elements(pRes); std::cout << "\n";
  free_elements(pRes);

  pRes=add(pE2,pE1);
  std::cout << "pE2 + pE1 = \n"; 
  std::cout << "raw:\n"; print_elements(pRes); std::cout << "\n"; 
  free_elements(pRes);

  pRes=add(pE1,pE1);
  std::cout << "pE1 + pE1 = \n";
  std::cout << "raw:\n"; print_elements(pRes); std::cout << "\n"; 
  free_elements(pRes);

  free_elements(pE1);
  free_elements(pE2);
}

void test10() {
  std::cout << "\n==== VECTOR ADDITION (ZEROES) ========\n";
  ElementNode_handle pE1=0,pE2=0,pRes;
  for(int i{};i<20;++i) insert_element(&pE1,i,i*i);
  std::cout << "vector pE1 formatted:\n";
  print_elements_width(pE1,4,20); std::cout << "\n";

  for(int i{};i<20;++i) insert_element(&pE2,i,-i*i);
  std::cout << "vector pE2 formatted:\n";
  print_elements_width(pE2,4,20); std::cout << "\n";
 

  pRes=add(pE1,pE2);
  std::cout << "pE1 + pE2 = \n"; print_elements_width(pRes,4,20); std::cout << "\n";
  std::cout << "raw:\n"; print_elements(pRes); std::cout << "\n";
  free_elements(pRes);
  pRes=add(pE2,pE1);
  std::cout << "pE2 + pE1 = \n"; print_elements_width(pRes,4,20); std::cout << "\n";
  std::cout << "raw:\n"; print_elements(pRes); std::cout << "\n";
  free_elements(pRes);
  free_elements(pE1);
  free_elements(pE2);
}

void test11() {
  std::cout << "\n==== TESTING FIND ========\n";
  ElementNode_handle pE1=0;
  for(int i{};i<20;i+=2) insert_element(&pE1,i,i*i);
  print_elements_width(pE1,4,20); std::cout << "\n";
  std::cout << "print vector using get - very inefficient!!!\n";
  for(int i{};i<20;++i) std::cout << "index " << i << ", value " << get(pE1, i) << "\n";
  free_elements(pE1);
}

void test12() {
  std::cout << "\n==== TESTING FIND 2 ========\n";
  ElementNode_handle pE1=0;
  int pos=10000;
  insert_element(&pE1,pos,5);
  print_elements(pE1); std::cout << "\n";

  std::cout << "value " << get(pE1, pos) << " at position " << pos << "\n";
  free_elements(pE1);
}
}
