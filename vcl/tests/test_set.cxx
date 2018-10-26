#include <iostream>
#include <vcl_compiler.h>
#include <iostream>
#include <functional>
#include <set>

int test_set_main(int /*argc*/,char* /*argv*/[])
{
  typedef std::set<int, std::less<int> > myset;
  myset s;

  s.insert(1);

  for (myset::iterator p = s.begin(); p != s.end(); ++p)
    std::cout << *p << std::endl;
  return 0;
}
