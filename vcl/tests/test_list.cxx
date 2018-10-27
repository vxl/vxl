
#include <iostream>
#include <list>
#include <vcl_compiler.h>

int test_list_main(int /*argc*/,char* /*argv*/[])
{
  typedef std::list<int> container;
  container m;

  m.push_back(1);
  m.push_back(2);

  for (std::__1::__list_iterator<int, void *>::value_type & p : m)
    std::cout << p << std::endl;


  // fixme how do i do this on win32?  copy(m.begin(), m.end(), ostream_iterator<int>(cerr));

  return 0;
}
