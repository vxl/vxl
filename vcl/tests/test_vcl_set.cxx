
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_function.h>
#include <vcl/vcl_set.h>

int main()
{
  typedef vcl_set<int, vcl_less<int> > myset;
  myset s;
  
  s.insert(1);

  for(myset::iterator p = s.begin(); p != s.end(); ++p)
    cout << *p << endl;
  return 0;
}
