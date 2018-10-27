#include <iostream>
#include <functional>
#include <set>
#include <vcl_compiler.h>

int test_set_main(int /*argc*/,char* /*argv*/[])
{
  typedef std::set<int, std::less<int> > myset;
  myset s;

  s.insert(1);

  for (std::__1::__tree_const_iterator<int, std::__1::__tree_node<int, void *> *, long>::value_type p : s)
    std::cout << p << std::endl;
  return 0;
}
