#include <vcl/vcl_iostream.h>
#include <vcl/vcl_map.h>

typedef vcl_multimap<int, double, vcl_less<int> > mymap;

vcl_ostream &operator<<(vcl_ostream &s, mymap::value_type const &x) 
{
  return s << '(' << x.first << ',' << x.second << ')';
}

int main()
{
  mymap m;
  m.insert(mymap::value_type(0, 2.71828182845905));
  m.insert(mymap::value_type(2, 3.14159265358979));
  m.insert(mymap::value_type(2, 1.41421356237310));
  m.insert(mymap::value_type(3, 1.61803398874989));

  mymap::iterator b = m.begin();
  mymap::iterator e = m.end();

  cout << "the whole container:" << endl;
  for (mymap::iterator p = b; p != e; ++p)
    cout << *p << endl;
  
  cout << "lower_bound() and upper_bound():" << endl;
  for (int k=-1; k<=4; ++k) {  
    cout << "k=" << k << endl;

    mymap::iterator lo = m.lower_bound(k);
    cout << "  lo: ";
    if (lo==b) cout << "begin";
    else if (lo==e) cout << "end";
    else cout << *lo;
    cout << endl;

    mymap::iterator hi = m.upper_bound(k);
    cout << "  hi: ";
    if (hi==b) cout << "begin";
    else if (hi==e) cout << "end";
    else cout << *hi;
    cout << endl;
  }
  
  return 0;
}
