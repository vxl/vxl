#include <mbl/mbl_arb_length_int.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vnl/vnl_test.h>

void test_arb_length_int() {
  {
    mbl_arb_length_int a(-5l);
    mbl_arb_length_int b(-7l), c, d(256l), e(65536l);
    TEST("==", a==-5l, true);
    -a;
    TEST("==", 5==(-a), true);
    TEST("==", b==-7l, true);
    TEST("==", -7l==b, true);
    c = a; c*=7; TEST("*=", c, -35l);
    c = d; c/=7; TEST("/=", c, 36ul);
    c = d; TEST("%", d%(unsigned char)7, (unsigned char)4);
    c = e; c*=7; TEST("*=", c, 0x70000ul);
    TEST("<", a<d, true);
    TEST("<", a<1L, true);
    TEST("<", a<-1l, true);
    TEST(">", -b<d, true);
    TEST("<=", c<=e, false);
    TEST(">=", b>=-7L, true);
    TEST("(long)", e.fits_in_unsigned_long(), true);
    TEST("(long)", (long)(d) , 256);
    vcl_cout << "a=" << a << vcl_endl
             << "b=" << b << vcl_endl
             << "c=" << c << vcl_endl
             << "d=" << d << vcl_endl
             << "e=" << e << vcl_endl;
    vcl_cout << vcl_endl;
  }
  mbl_arb_length_int d(-4l);
  d++; d++; d++; d++; 
  TEST("++", d , 0ul);
  for (unsigned i = 0; i < 70000; ++i) ++d;
  vcl_cout << "d= " << d << vcl_endl;
  TEST("++", d, 70000l);
  for (unsigned i = 0; i < 80000; ++i)
  {
    if (i == 70255)
      vcl_cout << "s";
    assert (d ==(long)(70000-i));
    --d;
  }
  vcl_cout << "d= " << d << vcl_endl;
  TEST("--", d, -10000l);

}

TESTMAIN(test_arb_length_int);
