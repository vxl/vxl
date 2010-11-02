#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_sstream.h>
#include <vcl_limits.h> // for infinity()
#include <vil/vil_na.h>
#include <testlib/testlib_test.h>

void test_na()
{
  // Create NaN, NA
  double qnan_d = vcl_numeric_limits<double>::quiet_NaN();
  double na_d = vil_na();

#define print_hex(p) \
  vcl_hex<<vcl_setfill('0')<<vcl_setw(2)<<(short)reinterpret_cast<unsigned char*>(&p)[sizeof(p)-1]; \
  for (unsigned int i=2; i<=sizeof(p); ++i) \
    vcl_cout<<vcl_setfill('0')<<vcl_setw(2)<<(short)(reinterpret_cast<unsigned char*>(&p))[sizeof(p)-i]; \
  vcl_cout<<vcl_dec

  vcl_cout << "qnan_d = " << qnan_d << " = " << print_hex(qnan_d) << vcl_endl
           << "na_d   = " << na_d   << " = " << print_hex(na_d)   << vcl_endl
           << vcl_endl;

  TEST("!isna(0)", !vil_na_isna(0), true);
  TEST("!isna(-0)", !vil_na_isna(-0), true);
  TEST("!isna(-1.0)", !vil_na_isna(-1.0), true);
  TEST("!isna(inf)", !vil_na_isna(vcl_numeric_limits<double>::infinity()), true);
  TEST("!isna(NaN)", !vil_na_isna(qnan_d), true);

#undef print_hex
}

TESTMAIN(test_na);
