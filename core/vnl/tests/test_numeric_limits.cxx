#include <vcl/vcl_iostream.h>
#include <vnl/vnl_numeric_limits.h>
//#include <math/numeric_limits_float.h>
//#include <math/numeric_limits_double.h>

main()
{
  cout << "dmax  = " << vnl_numeric_limits<double>::max() << endl;
  cout << "dmin  = " << vnl_numeric_limits<double>::min() << endl;
  cout << "deps  = " << vnl_numeric_limits<double>::epsilon() << endl;
  cout << "dnmin = " << vnl_numeric_limits<double>::denorm_min() << endl;
  cout << "dnan  = " << vnl_numeric_limits<double>::quiet_NaN() << endl;
  cout << "dinf  = " << vnl_numeric_limits<double>::infinity() << endl;
  cout << "dninf = " << -vnl_numeric_limits<double>::infinity() << endl;

  cout << "fmax  = " << vnl_numeric_limits<float>::max() << endl;
  cout << "fmin  = " << vnl_numeric_limits<float>::min() << endl;
  cout << "feps  = " << vnl_numeric_limits<float>::epsilon() << endl;
  cout << "fnmin = " << vnl_numeric_limits<float>::denorm_min() << endl;
  cout << "fnan  = " << vnl_numeric_limits<float>::quiet_NaN() << endl;
  cout << "finf  = " << vnl_numeric_limits<float>::infinity() << endl;
  cout << "fninf = " << -vnl_numeric_limits<float>::infinity() << endl;
  return 0;
}
