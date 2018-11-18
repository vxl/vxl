// This is core/vbl/tests/vbl_test_local_minima.cxx
#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_array_1d.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_array_3d.h>
#include <vbl/vbl_local_minima.h>
static void vbl_test_local_minima_1d()
{
  std::cout << "\n Testing vbl_local_minima 1d\n+++++++++++++++++++++++++\n\n";
  unsigned n = 10;
  vbl_array_1d<double> v(n,1.0);
  double thresh = 0.01;
  // set up minima at middle and two ends
  v[0]=0.06;   v[n/2]= 0.05;  v[n-1]=0.04;
  vbl_array_1d<double> minima = vbl_local_minima(v, thresh);
  bool success = !minima.empty();
  for (double i : minima)
    std::cout << i << ' ';
  std::cout << '\n';
  success = success && minima[0]>0 && minima[n/2]>0 && minima[n-1]>0;
  TEST("1d minima double", success, true);
  vbl_array_1d<unsigned> vi(n, 1);
  // set up minima at middle and two ends
  vi[0]=0;   vi[n/2]= 0;  vi[n-1]=0;
  unsigned tu = 0;
  vbl_array_1d<unsigned> minun = vbl_local_minima(vi, tu);
  success = !minun.empty();
  for (unsigned int i : minun)
    std::cout << i << ' ';
  std::cout << '\n';
  success = success && minun[0]>0 && minun[n/2]>0 && minun[n-1]>0;
  TEST("1d minima unsigned", success, true);
}

static void vbl_test_local_minima_2d()
{
  std::cout << "\n Testing vbl_local_minima 2d\n+++++++++++++++++++++++++\n\n";
  unsigned nr = 10, nc = 5;
  vbl_array_2d<double> v(nr, nc, 1.0);

  // minimum in middle of array
  v[nr/2][nc/2]=0.05;
  // minima in middle of edges
  v[0][nc/2] = 0.05;
  v[nr/2][0] = 0.05;
  v[nr-1][nc/2] = 0.05;
  v[nr/2][nc-1] = 0.05;
  // minima at corners
  v[0][0] = 0.05;
  v[nr-1][0] = 0.05;
  v[0][nc-1] = 0.05;
  v[nr-1][nc-1] = 0.05;
  vbl_array_2d<double> minima = vbl_local_minima(v);
  bool success = minima.rows() > 0;
  for (unsigned r = 0; r<minima.rows(); ++r) {
    for (unsigned c = 0; c<minima.cols(); ++c)
      std::cout << minima[r][c] << ' ';
    std::cout << '\n';
  }
  std::cout << '\n';
  success = success && minima[0][nc/2] > 0 &&  minima[nr/2][0] > 0 &&
    minima[nr-1][nc/2] > 0 && minima[nr/2][nc-1] > 0 && minima[0][0] > 0 &&
    minima[nr-1][0] > 0 &&  minima[0][nc-1] > 0 && minima[nr-1][nc-1] > 0 ;
  TEST("minima 2d", success, true);
  // test two rows
  nr = 2;
  vbl_array_2d<double> v2r(nr, nc, 1.0);

  //lower left corner
  v2r[nr-1][0] = 0.05;
  //upper right corner
  v2r[0][nc-1] = 0.05;
  // lower middle
  v2r[nr-1][nc/2] = 0.05;
  vbl_array_2d<double> minima2r = vbl_local_minima(v2r);
  TEST("vbl_local_minima rows", minima2r.rows() > 0, true);
  for (unsigned r = 0; r<minima2r.rows(); ++r) {
    for (unsigned c = 0; c<minima2r.cols(); ++c)
      std::cout << minima2r[r][c] << ' ';
    std::cout << '\n';
  }
  success = minima2r[nr-1][0]&&minima2r[0][nc-1]&&minima2r[nr-1][nc/2];
  TEST("test 2-d minima with two rows", success, true);
}

static void vbl_test_local_minima_3d()
{
  std::cout << "\n Testing vbl_local_minima 3d\n+++++++++++++++++++++++++\n\n";
  double thresh = 0.01;
  unsigned n1 = 5, n2 = 7, n3 = 9;
  vbl_array_3d<double> v(n1, n2, n3, 1.0);

  //set middle of array
  v[n1/2][n2/2][n3/2]=0.05;

  //set corners
  v[0][0][0] = 0.05;       v[n1-1][0][0] = 0.05; v[0][n2-1][0] = 0.05;
  v[n1-1][n2-1][0] = 0.05; v[0][0][n3-1] = 0.05; v[n1-1][0][n3-1] = 0.05;
  v[0][n2-1][n3-1] = 0.05; v[n1-1][n2-1][n3-1] = 0.05;

  // set (some)edge middles
  v[n1/2][0][0] = 0.05;   v[0][n2/2][0] = 0.05; v[0][0][n3/2] = 0.05;
  v[n1/2][n2-1][0] = 0.05;   v[n1-1][n2/2][0] = 0.05; v[n1-1][0][n3/2] = 0.05;

  // set face centers
  v[n1/2][n2/2][0] = 0.05;   v[n1/2][n2/2][n3-1] = 0.05;
  v[0][n2/2][n3/2] = 0.05;   v[n1-1][n2/2][n3/2] = 0.05;
  v[n1/2][0][n3/2] = 0.05;   v[n1/2][n2-1][n3/2] = 0.05;

  vbl_array_3d<double> minima = vbl_local_minima(v, thresh);
  bool success = minima.get_row1_count() > 0;
  for (unsigned x1 = 0; x1<minima.get_row1_count(); ++x1) {
    std::cout << "\n************************\n";
    for (unsigned x2 = 0; x2<minima.get_row2_count(); ++x2) {
      for (unsigned x3 = 0; x3<minima.get_row3_count(); ++x3)
        std::cout << minima[x1][x2][x3] << ' ';
      std::cout << '\n';
    }
    std::cout << "************************\n";
  }
  success = success && minima[n1/2][n2/2][n3/2]>0
    && minima[0][0][0] > 0 && minima[n1-1][0][0] > 0 && minima[0][n2-1][0] > 0
    && minima[n1-1][n2-1][0] > 0 && minima[0][0][n3-1] > 0 &&
    minima[n1-1][0][n3-1] > 0 &&  minima[0][n2-1][n3-1] > 0
    && minima[n1-1][n2-1][n3-1] > 0;

  success = success && minima[n1/2][0][0] > 0 && minima[0][n2/2][0] > 0 &&
    minima[0][0][n3/2] > 0 && minima[n1/2][n2-1][0] > 0 &&
    minima[n1-1][n2/2][0] > 0 && minima[n1-1][0][n3/2] > 0;

  // set face centers
  success = success && minima[n1/2][n2/2][0] > 0 &&
    minima[n1/2][n2/2][n3-1] > 0 && minima[0][n2/2][n3/2] > 0 &&
    minima[n1-1][n2/2][n3/2] > 0 && minima[n1/2][0][n3/2] > 0 &&
    minima[n1/2][n2-1][n3/2] > 0;
  TEST("test 3-d maxima", success, true);
}

static void vbl_test_local_minima()
{
  vbl_test_local_minima_1d();
  vbl_test_local_minima_2d();
  vbl_test_local_minima_3d();
}

TESTMAIN(vbl_test_local_minima);
