
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_functional.h>
#include <vcl/vcl_set.h>

int main()
{
  typedef vcl_set<int, vcl_less<int> > myset;
  myset s;
  
  s.insert(1);

  for(myset::iterator p = s.begin(); p != s.end(); ++p)
    vcl_cout << *p << vcl_endl;
  return 0;
}
