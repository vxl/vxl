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

  TEST("isnan(NA)", vnl_math_isnan(vnl_na()), true);
  TEST("isnan(NA2)", vnl_math_isnan(na_d), true);
  TEST("isnan(1/NA2)", vnl_math_isnan(1.0/na_d), true);
  TEST("isna(NA)", vnl_na_isna(vnl_na()), true);
  TEST("isna(NA2)", vnl_na_isna(na_d), true);
  TEST("isna(1/NA2)", vnl_na_isna(1.0/na_d), true);
  TEST("!isfinite(NA)", !vnl_math_isfinite(na_d), true);
  TEST("!isinf(NA)", !vnl_math_isinf(na_d), true);

  TEST("!isna(0)", !vnl_na_isna(0), true);
  TEST("!isna(-0)", !vnl_na_isna(-0), true);
  TEST("!isna(-1.0)", !vnl_na_isna(-1.0), true);
  TEST("!isna(inf)", !vnl_na_isna(vcl_numeric_limits<double>::infinity()), true);
  TEST("!isna(NaN)", !vnl_na_isna(qnan_d), true);

  {
    double x=0.0;
    vcl_istringstream ss("NA");
    vnl_na_double_extract(ss, x);
    TEST("x=\"NA\"", vnl_na_isna(x), true);
    vcl_cout << "x = " << x << " = " << print_hex(x) << vcl_endl;
  }

  {
    double x=0.0;
    vcl_istringstream ss("NA  ");
    vnl_na_double_extract(ss, x);
    TEST("x=\"NA  \"", vnl_na_isna(x), true);
    vcl_cout << "x = " << x << " = " << print_hex(x) << vcl_endl;
  }

  {
    double x=0.0;
    vcl_istringstream ss("1.0   ");
    vnl_na_double_extract(ss, x);
    TEST("x=\"1.0\"", x, 1.0);
    vcl_cout << "x = " << x << " = " << print_hex(x) << vcl_endl;
  }

  {
    double x=0.0, y=0.0;
    vcl_istringstream ss("NA1.0");
    vnl_na_double_extract(ss, x);
    vnl_na_double_extract(ss, y);
    TEST("x,y=\"NA1.0\"", vnl_na_isna(x) && y==1.0, true);
    vcl_cout << "x = " << x << " = " << print_hex(x) << vcl_endl;
  }

  {
    double x=0.0, y=0.0;
    vcl_istringstream ss("1.0NA");
    vnl_na_double_extract(ss, x);
    vnl_na_double_extract(ss, y);
    TEST("x,y=\"1.0NA\"", vnl_na_isna(y) && x==1.0, true);
    vcl_cout << "y = " << x << " = " << print_hex(x) << vcl_endl;
  }

  {
    double x=0.0, y=0.0;
    vcl_istringstream ss("NANA");
    vnl_na_double_extract(ss, x);
    vnl_na_double_extract(ss, y);
    TEST("x,y=\"NANA\"", vnl_na_isna(x) && vnl_na_isna(y), true);
    vcl_cout << "x = " << x << " = " << print_hex(x) << vcl_endl;
  }

  {
    double x=0.0, y=0.0;
    vcl_istringstream ss("NA 1.0");
    vnl_na_double_extract(ss, x);
    vnl_na_double_extract(ss, y);
    TEST("x,y=\"NA 1.0\"", vnl_na_isna(x) && y==1.0, true);
    vcl_cout << "x = " << x << " = " << print_hex(x) << vcl_endl;
  }

  {
    double x=0.0, y=0.0;
    vcl_istringstream ss("1.0 NA");
    vnl_na_double_extract(ss, x);
    vnl_na_double_extract(ss, y);
    TEST("x,y=\"1.0 NA\"", vnl_na_isna(y) && x==1.0, true);
    vcl_cout << "y = " << y << " = " << print_hex(y) << vcl_endl;
  }

  {
    double x=0.0, y=0.0;
    vcl_istringstream ss("NA NA");
    vnl_na_double_extract(ss, x);
    vnl_na_double_extract(ss, y);
    TEST("x,y=\"NA NA\"", vnl_na_isna(x) && vnl_na_isna(y), true);
    vcl_cout << "x = " << x << " = " << print_hex(x) << vcl_endl;
  }

  {
    double x=0.0, y=0.0;
    vcl_istringstream ss("-1.0-1.0");
    vnl_na_double_extract(ss, x);
    vnl_na_double_extract(ss, y);
    TEST("x,y=\"-1.0-1.0\"", x==-1.0 && y==-1.0, true);
    vcl_cout << "x = " << x << " = " << print_hex(x) << vcl_endl;
  }

  {
    vcl_ostringstream ss;
    vnl_na_double_insert(ss, -1.5);
    vnl_na_double_insert(ss, vnl_na());
    TEST("output \"-1.5NA\"", ss.str(), "-1.5NA");
    vcl_cout << "ss = " << ss.str() << vcl_endl;
  }

  {
    vcl_stringstream ss;
    ss << vnl_na_stream(-1.0) << ' ' << vnl_na_stream(vnl_na());
    double x=0.0, y=0.0;
    ss >> vnl_na_stream(x) >> vnl_na_stream(y);
    TEST("x,y=\"-1.0 NA\"", vnl_na_isna(y) && x==-1.0, true);
    vcl_cout << "y = " << y << " = " << print_hex(y) << vcl_endl;
    vcl_cout << "ss = " << ss.str() << vcl_endl;
  }
#undef print_hex
}

TESTMAIN(test_na);
