#include <vcl_iostream.h>
#include <vnl/vnl_numeric_limits.h>
#include <vnl/vnl_test.h>

void test_numeric_limits()
{
  vcl_cout << "dmax  = " << vnl_numeric_limits<double>::max() << vcl_endl
           << "dmin  = " << vnl_numeric_limits<double>::min() << vcl_endl
           << "deps  = " << vnl_numeric_limits<double>::epsilon() << vcl_endl
           << "dnmin = " << vnl_numeric_limits<double>::denorm_min() << vcl_endl
           << "dnan  = " << vnl_numeric_limits<double>::quiet_NaN() << vcl_endl
           << "dinf  = " << vnl_numeric_limits<double>::infinity() << vcl_endl
           << "dninf = " << -vnl_numeric_limits<double>::infinity() << vcl_endl

           << "fmax  = " << vnl_numeric_limits<float>::max() << vcl_endl
           << "fmin  = " << vnl_numeric_limits<float>::min() << vcl_endl
           << "feps  = " << vnl_numeric_limits<float>::epsilon() << vcl_endl
           << "fnmin = " << vnl_numeric_limits<float>::denorm_min() << vcl_endl
           << "fnan  = " << vnl_numeric_limits<float>::quiet_NaN() << vcl_endl
           << "finf  = " << vnl_numeric_limits<float>::infinity() << vcl_endl
           << "fninf = " << -vnl_numeric_limits<float>::infinity() << vcl_endl;

  TEST("dmax", vnl_numeric_limits<double>::max() > 1e300, true);
  TEST("dmin", vnl_numeric_limits<double>::min() < 1e-300, true);
  TEST("fmax", vnl_numeric_limits<double>::max() > 1e38, true);
  TEST("fmin", vnl_numeric_limits<double>::min() < 1e-38, true);
}

TESTMAIN(test_numeric_limits);
