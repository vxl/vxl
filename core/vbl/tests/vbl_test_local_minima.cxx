// This is core/vbl/tests/vbl_test_local_minima.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vbl/vbl_array_1d.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_array_3d.h>
#include <vbl/vbl_local_minima.h>
static void vbl_test_local_minima_1d()
{
  vcl_cout << "\n Testing vbl_local_minima 1d\n+++++++++++++++++++++++++\n\n";
  unsigned n = 10;
  vbl_array_1d<double> v(n,1.0);
  double thresh = 0.01;
  // set up minima at middle and two ends
  v[0]=0.06;   v[n/2]= 0.05;  v[n-1]=0.04; 
  vbl_array_1d<double> minima(n,0.0);
  bool success = local_minima(v, minima, thresh);
  for(unsigned i = 0; i<n; ++i)
    vcl_cout << minima[i] << ' ';
  vcl_cout << '\n';
  success = success && minima[0]>0 && minima[n/2]>0 && minima[n-1]>0;
  TEST("1d minima double ", success, true);
  vbl_array_1d<unsigned> vi(n, 1), minun(n,0);
  // set up minima at middle and two ends
  vi[0]=0;   vi[n/2]= 0;  vi[n-1]=0; 
  unsigned tu = 0;
  success = local_minima(vi, minun, tu);
  for(unsigned i = 0; i<n; ++i)
    vcl_cout << minun[i] << ' ';
  vcl_cout << '\n';
  success = success && minun[0]>0 && minun[n/2]>0 && minun[n-1]>0;
  TEST("1d minima unsigned", success, true);
}

static void vbl_test_local_minima_2d()
{
  vcl_cout << "\n Testing vbl_local_minima 2d\n+++++++++++++++++++++++++\n\n";
  unsigned nr = 10, nc = 5;
  vbl_array_2d<double> v(nr, nc, 1.0);
  vbl_array_2d<double> minima(nr, nc, 0.0);
  
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
  bool success = local_minima(v, minima);
  for(unsigned r = 0; r<nr; ++r){
    for(unsigned c = 0; c<nc; ++c)
      vcl_cout << minima[r][c] << ' ';
    vcl_cout << '\n';
  }
  vcl_cout << '\n';
  success = success && minima[0][nc/2] > 0 &&  minima[nr/2][0] > 0 &&
    minima[nr-1][nc/2] > 0 && minima[nr/2][nc-1] > 0 && minima[0][0] > 0 &&
    minima[nr-1][0] > 0 &&  minima[0][nc-1] > 0 && minima[nr-1][nc-1] > 0 ;
  TEST("minima 2d", success, true);
  // test two rows
  nr = 2;
  vbl_array_2d<double> v2r(nr, nc, 1.0);
  vbl_array_2d<double> minima2r(nr, nc, 0.0);
  
  //lower left corner
  v2r[nr-1][0] = 0.05;
  //upper right corner
  v2r[0][nc-1] = 0.05;
  // lower middle
  v2r[nr-1][nc/2] = 0.05;
  success = local_minima(v2r, minima2r);
  for(unsigned r = 0; r<nr; ++r){
    for(unsigned c = 0; c<nc; ++c)
      vcl_cout << minima2r[r][c] << ' ';
    vcl_cout << '\n';
  }
  success = minima2r[nr-1][0]&&minima2r[0][nc-1]&&minima2r[nr-1][nc/2];
  TEST("test 2-d minima with two rows", success, true);
}

static void vbl_test_local_minima_3d()
{
  vcl_cout << "\n Testing vbl_local_minima 3d\n+++++++++++++++++++++++++\n\n";
  double thresh = 0.01;
  unsigned n1 = 5, n2 = 7, n3 = 9;
  vbl_array_3d<double> v(n1, n2, n3, 1.0);
  vbl_array_3d<double> minima(n1, n2, n3, 0.0);

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

  bool success = local_minima(v, minima, thresh);
  for(unsigned x1 = 0; x1<n1; ++x1){
    vcl_cout << "\n************************\n";
    for(unsigned x2 = 0; x2<n2; ++x2){
      for(unsigned x3 = 0; x3<n3; ++x3)
        vcl_cout << minima[x1][x2][x3] << ' ';
      vcl_cout << '\n';
        }
    vcl_cout << "************************\n";
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

