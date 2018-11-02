#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_na.h>
#include <testlib/testlib_test.h>

void test_na()
{
  // Create NaN, NA
  double qnan_d = std::numeric_limits<double>::quiet_NaN();
  double na_d = vil_na(double());
  float qnan_f = std::numeric_limits<float>::quiet_NaN();
  float na_f = vil_na(float());

#define print_hex(p) \
  std::hex<<std::setfill('0')<<std::setw(2)<<(short)reinterpret_cast<unsigned char*>(&(p))[sizeof(p)-1]; \
  for (unsigned int i=2; i<=sizeof(p); ++i) \
    std::cout<<std::setfill('0')<<std::setw(2)<<(short)(reinterpret_cast<unsigned char*>(&(p)))[sizeof(p)-i]; \
  std::cout<<std::dec

  std::cout << "qnan_d = " << qnan_d << " = " << print_hex(qnan_d) << std::endl
           << "na_d   = " << na_d   << " = " << print_hex(na_d)   << std::endl
           << "qnan_f = " << qnan_f << " = " << print_hex(qnan_f) << std::endl
           << "na_f   = " << na_f   << " = " << print_hex(na_f)   << std::endl
           << std::endl;
  std::cout << "**** double" << std::endl;
  TEST("!isna(0)", !vil_na_isna(0.0), true);
  TEST("!isna(-0)", !vil_na_isna(-0.0), true);
  TEST("!isna(-1.0)", !vil_na_isna(-1.0), true);
  TEST("!isna(inf)", !vil_na_isna(std::numeric_limits<double>::infinity()), true);
  TEST("!isna(NaN)", !vil_na_isna(qnan_d), true);
  TEST("isna(NA)", vil_na_isna(na_d), true);
  std::cout << "**** float" << std::endl;
  TEST("!isna(0)", !vil_na_isna(0.0f), true);
  TEST("!isna(-0)", !vil_na_isna(-0.0f), true);
  TEST("!isna(-1.0)", !vil_na_isna(-1.0f), true);
  TEST("!isna(inf)", !vil_na_isna(std::numeric_limits<float>::infinity()), true);
  TEST("!isna(NaN)", !vil_na_isna(qnan_f), true);
  TEST("isna(NA)", vil_na_isna(na_f), true);

#undef print_hex
}

TESTMAIN(test_na);
