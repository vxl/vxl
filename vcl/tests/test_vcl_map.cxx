
#include <iostream.h>
#include <vcl/vcl_map.h>

int main()
{
  typedef vcl_map<int, int, vcl_less<int> > mymap;
  mymap m;
  
  m.insert(mymap::value_type(1, 2));

  for(mymap::iterator p = m.begin(); p != m.end(); ++p)
    cout << (*p).first << " " << (*p).second << endl;

  return 0;
}
