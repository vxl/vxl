#include <vcl/vcl_iostream.h>
#include <vcl/vcl_map.h>

int main()
{
  typedef vcl_map<int, double, vcl_less<int> > mymap;
  mymap m;
  
  m.insert(mymap::value_type(1, 2718));
  m.insert(mymap::value_type(2, 3141));

  for(mymap::iterator p = m.begin(); p != m.end(); ++p)
    cout << (*p).first << " " << (*p).second << endl;

  return 0;
}
