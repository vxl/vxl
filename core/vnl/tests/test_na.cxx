#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_sstream.h>
#include <vcl_limits.h> // for infinity()
#include <vnl/vnl_math.h>
#include <vnl/vnl_na.h>
#include <testlib/testlib_test.h>

void test_na()
{
  // Create NaN, NA
  double qnan_d = vcl_numeric_limits<double>::quiet_NaN();
  double na_d = vnl_na();

#define print_hex(p) \
  vcl_hex<<vcl_setfill('0')<<vcl_setw(2)<<(short)reinterpret_cast<unsigned char*>(&p)[sizeof(p)-1]; \
  for (unsigned int i=2; i<=sizeof(p); ++i) \
    vcl_cout<<vcl_setfill('0')<<vcl_setw(2)<<(short)(reinterpret_cast<unsigned char*>(&p))[sizeof(p)-i]; \
  vcl_cout<<vcl_dec

  vcl_cout << "qnan_d = " << qnan_d << " = " << print_hex(qnan_d) << vcl_endl
           << "na_d   = " << na_d   << " = " << print_hex(na_d)   << vcl_endl
           << vcl_endl;

  testlib_test_assert("isnan(NA)", vnl_math_isnan(vnl_na()));
  testlib_test_assert("isnan(NA2)", vnl_math_isnan(na_d));
  testlib_test_assert("isnan(1/NA2)", vnl_math_isnan(1.0/na_d));
  testlib_test_assert("isna(NA)", vnl_na_isna(vnl_na()));
  testlib_test_assert("isna(NA2)", vnl_na_isna(na_d));
  testlib_test_assert("isna(1/NA2)", vnl_na_isna(1.0/na_d));
  testlib_test_assert("!isfinite(NA)", !vnl_math_isfinite(na_d));
  testlib_test_assert("!isinf(NA)", !vnl_math_isinf(na_d));

  testlib_test_assert("!isna(0)", !vnl_na_isna(0));
  testlib_test_assert("!isna(-0)", !vnl_na_isna(-0));
  testlib_test_assert("!isna(-1.0)", !vnl_na_isna(-1.0));
  testlib_test_assert("!isna(inf)", !vnl_na_isna(vcl_numeric_limits<double>::infinity()));
  testlib_test_assert("!isna(NaN)", !vnl_na_isna(qnan_d));

  {
    double x=0.0;
    vcl_istringstream ss("NA");
    vnl_na_double_parse(ss, x);
    testlib_test_assert("x=\"NA\"", vnl_na_isna(x));
    vcl_cout << "x = " << x << " = " << print_hex(x) << vcl_endl;
  }

  {
    double x=0.0;
    vcl_istringstream ss("NA  ");
    vnl_na_double_parse(ss, x);
    testlib_test_assert("x=\"NA  \"", vnl_na_isna(x));
    vcl_cout << "x = " << x << " = " << print_hex(x) << vcl_endl;
  }

  {
    double x=0.0;
    vcl_istringstream ss("1.0   ");
    vnl_na_double_parse(ss, x);
    testlib_test_assert("x=\"1.0\"", x==1.0);
    vcl_cout << "x = " << x << " = " << print_hex(x) << vcl_endl;
  }

  {
    double x=0.0, y=0.0;
    vcl_istringstream ss("NA1.0");
    vnl_na_double_parse(ss, x);
    vnl_na_double_parse(ss, y);
    testlib_test_assert("x,y=\"NA1.0\"", vnl_na_isna(x) && y==1.0);
    vcl_cout << "x = " << x << " = " << print_hex(x) << vcl_endl;
  }

  {
    double x=0.0, y=0.0;
    vcl_istringstream ss("1.0NA");
    vnl_na_double_parse(ss, x);
    vnl_na_double_parse(ss, y);
    testlib_test_assert("x,y=\"1.0NA\"", vnl_na_isna(y) && x==1.0);
    vcl_cout << "y = " << x << " = " << print_hex(x) << vcl_endl;
  }

  {
    double x=0.0, y=0.0;
    vcl_istringstream ss("NANA");
    vnl_na_double_parse(ss, x);
    vnl_na_double_parse(ss, y);
    testlib_test_assert("x,y=\"NANA\"", vnl_na_isna(x) && vnl_na_isna(y));
    vcl_cout << "x = " << x << " = " << print_hex(x) << vcl_endl;
  }

  {
    double x=0.0, y=0.0;
    vcl_istringstream ss("NA 1.0");
    vnl_na_double_parse(ss, x);
    vnl_na_double_parse(ss, y);
    testlib_test_assert("x,y=\"NA 1.0\"", vnl_na_isna(x) && y==1.0);
    vcl_cout << "x = " << x << " = " << print_hex(x) << vcl_endl;
  }

  {
    double x=0.0, y=0.0;
    vcl_istringstream ss("1.0 NA");
    vnl_na_double_parse(ss, x);
    vnl_na_double_parse(ss, y);
    testlib_test_assert("x,y=\"1.0 NA\"", vnl_na_isna(y) && x==1.0);
    vcl_cout << "y = " << y << " = " << print_hex(y) << vcl_endl;
  }

  {
    double x=0.0, y=0.0;
    vcl_istringstream ss("NA NA");
    vnl_na_double_parse(ss, x);
    vnl_na_double_parse(ss, y);
    testlib_test_assert("x,y=\"NA NA\"", vnl_na_isna(x) && vnl_na_isna(y));
    vcl_cout << "x = " << x << " = " << print_hex(x) << vcl_endl;
  }

  {
    double x=0.0, y=0.0;
    vcl_istringstream ss("-1.0-1.0");
    vnl_na_double_parse(ss, x);
    vnl_na_double_parse(ss, y);
    testlib_test_assert("x,y=\"-1.0-1.0\"", x==-1.0 && y==-1.0);
    vcl_cout << "x = " << x << " = " << print_hex(x) << vcl_endl;
  }
}
#undef print_hex

TESTMAIN(test_na);
