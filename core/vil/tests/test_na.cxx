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
  double na_d = vil_na(double());
  float qnan_f = vcl_numeric_limits<float>::quiet_NaN();
  float na_f = vil_na(float());

#define print_hex(p) \
  vcl_hex<<vcl_setfill('0')<<vcl_setw(2)<<(short)reinterpret_cast<unsigned char*>(&p)[sizeof(p)-1]; \
  for (unsigned int i=2; i<=sizeof(p); ++i) \
    vcl_cout<<vcl_setfill('0')<<vcl_setw(2)<<(short)(reinterpret_cast<unsigned char*>(&p))[sizeof(p)-i]; \
  vcl_cout<<vcl_dec

  vcl_cout << "qnan_d = " << qnan_d << " = " << print_hex(qnan_d) << vcl_endl
           << "na_d   = " << na_d   << " = " << print_hex(na_d)   << vcl_endl
           << "qnan_f = " << qnan_f << " = " << print_hex(qnan_f) << vcl_endl
           << "na_f   = " << na_f   << " = " << print_hex(na_f)   << vcl_endl
           << vcl_endl;
  vcl_cout << "**** double" << vcl_endl;
  TEST("!isna(0)", !vil_na_isna(0.0), true);
  TEST("!isna(-0)", !vil_na_isna(-0.0), true);
  TEST("!isna(-1.0)", !vil_na_isna(-1.0), true);
  TEST("!isna(inf)", !vil_na_isna(vcl_numeric_limits<double>::infinity()), true);
  TEST("!isna(NaN)", !vil_na_isna(qnan_d), true);
  TEST("isna(NA)", vil_na_isna(na_d), true);
  vcl_cout << "**** float" << vcl_endl;
  TEST("!isna(0)", !vil_na_isna(0.0f), true);
  TEST("!isna(-0)", !vil_na_isna(-0.0f), true);
  TEST("!isna(-1.0)", !vil_na_isna(-1.0f), true);
  TEST("!isna(inf)", !vil_na_isna(vcl_numeric_limits<float>::infinity()), true);
  TEST("!isna(NaN)", !vil_na_isna(qnan_f), true);
  TEST("isna(NA)", vil_na_isna(na_f), true);

#undef print_hex
}

TESTMAIN(test_na);
