/*
  fsm@robots.ox.ac.uk
*/
#include <vcl_iostream.h>

class X {
public:
  X(int v) : x(v) {
    vcl_cerr << (void*)this << " X::X(" << x << ")\n";
  }
  ~X() {
    vcl_cerr << (void*)this << " X::~X() : x=" << x << vcl_endl;
  }

  int x;
};
vcl_ostream &operator<<(vcl_ostream &os, X const &x) {
  return os << "X[" << x.x << "]";
}

#include <vgui/vgui_smartptr.h>
SPECIALIZE_VGUI_smartptr_traits__using_global_count(X);

int main(int, char **) {
  vgui_smartptr<X> a(new X(3141));
  vgui_smartptr<X> b(new X(2718));

  vcl_cerr << "---\n";

  vcl_cerr << "a => " << (void*)a.as_pointer() << " : " << *a << vcl_endl;
  vcl_cerr << "b => " << (void*)b.as_pointer() << " : " << *b << vcl_endl;

  vcl_cerr << "---\n";

  vgui_smartptr<X> c = a;
  a = b;

  vcl_cerr << "---\n";

  vcl_cerr << "a => " << (void*)a.as_pointer() << " : " << *a << vcl_endl;
  vcl_cerr << "b => " << (void*)b.as_pointer() << " : " << *b << vcl_endl;

  vcl_cerr << "---\n";

  c = 0; // 3141 gets deleted now.

  vcl_cerr << "---\n";

  vcl_cerr << "a => " << (void*)a.as_pointer() << " : " << *a << vcl_endl;
  vcl_cerr << "b => " << (void*)b.as_pointer() << " : " << *b << vcl_endl;

  vcl_cerr << "---\n";

  return 0;
}
