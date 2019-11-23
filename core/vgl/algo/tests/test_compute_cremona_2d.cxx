#include <iostream>
#include <testlib/testlib_test.h>
#include <vgl/algo/vgl_compute_cremona_2d.h>
#include <vgl/algo/vgl_cremona_trans_2d.h>
#include <vnl/vnl_vector.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_random.h>
static void test_compute_cremona_2d() {
  vnl_random rand;
  vgl_compute_cremona_2d<double, 3> cc;
  double x = 2, y=3;
  vnl_vector<double> pv = vgl_cremona_trans_2d<double,3>::power_vector(x, y);
  std::vector<double> tv = {1, 3, 9, 27, 2, 6, 18, 4, 12, 8};
  double er = 0.0;
  size_t npv = pv.size();
  for(size_t i = 0; i<npv; ++i)
    er += fabs(pv[i]-tv[i]);
  TEST("power vector", er, 0.0);
  vnl_vector<double> x_neu(npv, 0.0), x_den(npv,0.0), y_neu(npv,0.0), y_den(npv,0.0);
  x_neu[1]=10;  x_neu[4]=20; x_neu[9]=1;
  x_den[0]=5;   x_den[3]=1;  x_den[5]=1;
  y_neu[1]=10.1;  y_neu[4]=20.1; y_neu[9]=1.1;
  y_den[0]=5.1;   y_den[3]=1.1;  y_den[5]=1.1;                                      
  size_t npts = 1000;
  std::vector<vgl_homg_point_2d<double> > from_pts, to_pts;
  double r = 1.0;
  for(size_t i = 0; i<npts; ++i){
    double X = rand.drand32(-r, r);
    double Y = rand.drand32(-r, r);
    vnl_vector<double> pv = vgl_cremona_trans_2d<double, 3>::power_vector(X, Y);
    double x = dot_product(x_neu,pv)/ dot_product(x_den,pv);
    double y = dot_product(y_neu,pv)/ dot_product(y_den,pv);
    from_pts.emplace_back(X, Y);
    to_pts.emplace_back(x, y);
  }
  bool good = cc.compute_linear(from_pts, to_pts);
  vnl_vector<double> lin_coeff = cc.linear_coeff();
  
  TEST("linear computation deg = 3", good, true);

  std::cout << "Solution error deg = 3" << cc.linear_error() << std::endl;
  TEST_NEAR("linear solution error deg = 3", cc.linear_error(), 0.0, 1.0e-6);

  vgl_cremona_trans_2d<double, 3> ct = cc.linear_trans();
  x = 0.0; y = 0.0;
  ct.project(from_pts[0].x(), from_pts[0].y(), x, y);
  double ter = fabs(to_pts[0].x()-x) + fabs(to_pts[0].y()-y);
  std::cout << "cremona_trans err deg = 3 " << ter << std::endl;
  TEST_NEAR("cremona_trans class constructor deg = 3 ", ter,0.0,1.0e-6);
  
  // TEST the second order transform //
  std::cout << "\ntest cremona with deg = 2" << std::endl;
  vgl_compute_cremona_2d<double, 2> cc2;
  good = cc2.compute_linear(from_pts, to_pts);
  TEST("linear computation deg = 2 ", good, true);
  std::cout << "Solution error deg = 2 " << cc2.linear_error() << std::endl;
  TEST_NEAR("linear solution error deg = 2", cc2.linear_error(), 0.0, 0.2);
}

TESTMAIN(test_compute_cremona_2d);
