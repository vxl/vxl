/*
  fsm@robots.ox.ac.uk
*/
#include <vcl/vcl_iostream.h>
#define use_vbl 1
#if use_vbl
# include <vbl/vbl_array.h>
# define container vbl_array
#else
# include <vcl/vcl_vector.h>
# define container vcl_vector
#endif

struct X 
{
  // NB no default constructor
  X(int a, int b) : x(a + b) { }
  X(float a, float b) : x(a + b) { }
  X(double a, double b) : x(a + b) { }
  void method() { vcl_cerr << '[' << x << ']' << vcl_flush;; }
private:
  double x;
};

int main(int, char **)
{
  container<X> v;

  vcl_cerr << "size " << v.size() << vcl_endl
       << "capacity " << v.capacity() << vcl_endl;

  v.push_back(X(1, 2));
  v.push_back(X(1.0f, 2.0f));
  v.push_back(v.front());
  v.push_back(X(1.0, 2.0));

  vcl_cerr << "size " << v.size() << vcl_endl
       << "capacity " << v.capacity() << vcl_endl;

  for (container<X>::iterator i=v.begin(); i!=v.end(); ++i)
    (*i).method();
  
  return 0;
}

#if use_vbl
# include <vbl/vbl_array.txx>
VBL_ARRAY_INSTANTIATE(X);
#endif
