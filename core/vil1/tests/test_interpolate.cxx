/*
  fsm
*/
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vil1/vil1_interpolate.h>
#include <testlib/testlib_test.h>

static double const avg[5] = { 0.8709, 1.25178114, 1.0130145969, 1.0978098765, 1.0605102418 };

static double f(double i, double j, int d)
{
  static double const taylor[5][5] = {
    {  0.8709,   -0.2943,   -0.6026,    0.4936,    0.6924 },
    {  0.8338,    0.6263,    0.2076,   -0.1098,    0.0503 },
    { -0.1795,   -0.9803,   -0.4556,    0.8636,   -0.5947 },
    {  0.7873,   -0.7222,   -0.6024,   -0.0680,    0.3443 },
    { -0.8842,   -0.5945,   -0.9695,   -0.1627,    0.6762 }
  };

  assert(0<=d && d<=4);

  i /= 10;
  j /= 10;

  double out = 0;
  for (int m=0; m<=d; ++m)
    for (int n=0; n<=d; ++n)
      out += taylor[m][n] * vcl_pow(i, m) * vcl_pow(j, n);

  return out;
}

static void test_interpolate()
{
  vil1_memory_image_of<double> I(10, 10);

  double r = 4.6;
  double c = 4.3;

  for (int d=0; d<=4; ++d)
  {
    vcl_cout << "d=" << d << vcl_endl;

    for (int i=0; i<I.rows(); ++i)
      for (int j=0; j<I.cols(); ++j)
        I[i][j] = f(i, j, d);

#define dn(u) int(vcl_floor(u))
#define up(u) int(vcl_ceil (u))
    vcl_cout << "  " << I[dn(r)][dn(c)] << ' ' << I[up(r)][dn(c)] << vcl_endl
             << "  " << I[dn(r)][up(c)] << ' ' << I[up(r)][up(c)] << vcl_endl;
#undef dn
#undef up

    double out = f(r, c, d);
    vcl_cout << "  true    : " << out << "\t\t-->   ";
    testlib_test_perform(vcl_fabs(out - avg[d]) < 1e-9);

    vil1_interpolate_bilinear(I, c, r, &out);
    vcl_cout << "  bilinear: " << out << "\t\t-->   ";
    testlib_test_perform(vcl_fabs(out - avg[d]) < 0.005*avg[d]);

    vil1_interpolate_bicubic (I, c, r, &out);
    vcl_cout << "  bicubic : " << out << "\t\t-->   ";
    testlib_test_perform(vcl_fabs(out - avg[d]) < 0.0005*avg[d]);
  }
}

TESTMAIN(test_interpolate);
