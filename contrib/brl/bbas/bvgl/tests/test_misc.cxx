//:
// \file
#include <vcl_vector.h>
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

class misc
{
  const unsigned cx_;
  unsigned bx_;
 public:
  misc() : cx_(1), bx_(4) {}
  const unsigned& cx(unsigned const& a) const
  { if (a>4) return cx_; else return bx_; }
};

static void test_misc()
{
  misc m;
  const unsigned & x = m.cx(0);
  unsigned y = m.cx(7);
  vcl_cout << " x " << x  << " y " << y << vcl_endl;
}

TESTMAIN( test_misc );
