// This is vcl/tests/test_stlfwd.cxx
#include <iostream>
#include <functional>
#include <string>
#include <map>
#include <set>
#include <list>
#include <vcl_compiler.h>

#if defined(TEST) && TEST == 2
// stl included first

#include <vcl_stlfwd.h>

#else
// Normal
#include <vcl_stlfwd.h>

#endif

void f(std::map<int, std::string,std::less<int> >*,
       std::set<int,std::less<int> >*,
       std::list<int>*
      )
{
}


int test_stlfwd_main(int /*argc*/,char* /*argv*/[])
{
  return 0;
}
